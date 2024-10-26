#include "CuckooHashTab.h"
#include <iostream>
#include <random>
#include <algorithm>

// Implementación de MurmurHash3 para uint32_t
uint32_t CuckooHashTab::murmur3_32(uint32_t key, uint32_t seed) const {
    key ^= seed;
    key ^= key >> 16;
    key *= 0x85ebca6b;
    key ^= key >> 13;
    key *= 0xc2b2ae35;
    key ^= key >> 16;
    return key;
}

CuckooHashTab::CuckooHashTab(int initial_size, int num_tables)
    : size(initial_size), num_tables(num_tables) {
    tables.resize(num_tables, std::vector<Entry>(size));

    // Inicializar seeds con valores aleatorios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(1, UINT32_MAX); // Semillas no nulas

    seed1 = dis(gen);
    seed2 = dis(gen);
}
//Funcion 1 de hash
size_t CuckooHashTab::hash1(uint32_t dni) const {
    return murmur3_32(dni, seed1) % size;
}

//Funcion 2 de hash
size_t CuckooHashTab::hash2(uint32_t dni) const {
    return murmur3_32(dni, seed2) % size;
}

size_t CuckooHashTab::default_hash(uint32_t dni, int table_idx) const {
    // Generar una semilla dinámica basada en el índice de la tabla y el DNI
    uint32_t dynamic_seed = murmur3_32(table_idx, seed1) ^ murmur3_32(dni, seed2);
    return murmur3_32(dni, dynamic_seed) % size;
}

// Cálculo del índice hash basado en el número de tabla
int CuckooHashTab::hash(int table_idx, uint32_t dni) const {
    switch (table_idx) {
        case 0:
            return hash1(dni);
        case 1:
            return hash2(dni);
        default:
            return default_hash(dni, table_idx);
    }
}

// Función para insertar un elemento en la tabla hash
void CuckooHashTab::insertar(uint32_t dni, uint32_t offset) {
    Entry entry = { dni, offset };
    Entry temp_entry = entry;

    for (int count = 0; count < rehash_limit; ++count) {
        for (int i = 0; i < num_tables; ++i) {
            int h = hash(i, temp_entry.dni);
            if (tables[i][h].dni == 0) { // Slot vacío
                tables[i][h] = temp_entry;
                return;
            }
            // Intercambiar la entrada actual con la del slot ocupado
            std::swap(temp_entry, tables[i][h]);
        }
    }

    // Si no se pudo insertar después de rehash_limit intentos, realizar rehash y reintentar
    std::cerr << "Rehash limit reached. Performing rehashing...\n";
    rehash();
    insertar(dni, offset); // Llamada recursiva después del rehash
}

// Función para buscar un elemento en la tabla hash
bool CuckooHashTab::buscar(uint32_t dni, uint32_t& offset) const {
    for (int i = 0; i < num_tables; ++i) {
        int h = hash(i, dni);
        if (tables[i][h].dni == dni) {
            offset = tables[i][h].offset;
            return true;
        }
    }
    return false;
}

// Función para eliminar un elemento de la tabla hash
void CuckooHashTab::eliminar(uint32_t dni) {
    for (int i = 0; i < num_tables; ++i) {
        int h = hash(i, dni);
        if (tables[i][h].dni == dni) {
            tables[i][h].dni = 0;    // Marcar como vacío
            tables[i][h].offset = 0; // Opcional: reiniciar el offset
            return;
        }
    }
    std::cerr << "Error: DNI " << dni << " no encontrado para eliminar.\n";
}

// Función para rehashing de las tablas
void CuckooHashTab::rehash() {
    std::cout << "Rehashing tables... Current size per table: " << size
              << " -> New size per table: " << size * 2 << "\n";
    size *= 2; // Duplicar el tamaño de las tablas

    // Generar nuevas seeds para evitar patrones de colisión
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(1, UINT32_MAX);
    seed1 = dis(gen);
    seed2 = dis(gen);

    // Crear nuevas tablas con el tamaño actualizado
    std::vector<std::vector<Entry>> new_tables(num_tables, std::vector<Entry>(size));

    // Reinsertar todas las entradas existentes en las nuevas tablas
    for (int i = 0; i < num_tables; ++i) {
        for (const auto& entry : tables[i]) {
            if (entry.dni != 0) { // Solo reinserta entradas ocupadas
                bool inserted = false;
                Entry temp_entry = entry;

                for (int count = 0; count < rehash_limit; ++count) {
                    for (int j = 0; j < num_tables; ++j) {
                        int h = hash(j, temp_entry.dni);
                        if (new_tables[j][h].dni == 0) { // Slot vacío
                            new_tables[j][h] = temp_entry;
                            inserted = true;
                            break;
                        }
                        // Intercambiar si el slot está ocupado
                        std::swap(temp_entry, new_tables[j][h]);
                    }
                    if (inserted) break;
                }

                if (!inserted) {
                    std::cerr << "Failed to reinsert DNI " << entry.dni << " during rehashing.\n";
                    // Opcional: incrementar tamaño y rehash nuevamente
                }
            }
        }
    }

    // Reemplazar las tablas antiguas por las nuevas
    tables = std::move(new_tables);
}

// Función auxiliar para insertar durante el rehashing
bool CuckooHashTab::insertRehash(const Entry& entry, std::vector<std::vector<Entry>>& new_tables, int rehash_count) {
    if (rehash_count >= rehash_limit) {
        return false;
    }

    Entry temp_entry = entry;

    for (int count = 0; count < rehash_limit; ++count) {
        for (int i = 0; i < num_tables; ++i) {
            int h = hash(i, temp_entry.dni);
            if (new_tables[i][h].dni == 0) { // Slot vacío
                new_tables[i][h] = temp_entry;
                return true;
            }
            std::swap(temp_entry, new_tables[i][h]);
        }
    }

    return false;
}
