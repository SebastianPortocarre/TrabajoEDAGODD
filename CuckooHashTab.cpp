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
    tables.resize(num_tables, std::vector<Entry>(size, Entry{0, 0}));
    seed1 = 0xA1B2C3D4;
    seed2 = 0xD4C3B2A1;
}

size_t CuckooHashTab::hash1(uint32_t dni) const {
    return murmur3_32(dni, seed1) % size;
}

size_t CuckooHashTab::hash2(uint32_t dni) const {
    return murmur3_32(dni, seed2) % size;
}

size_t CuckooHashTab::default_hash(uint32_t dni, int table_idx) const {
    uint32_t dynamic_seed = murmur3_32(table_idx, seed1) ^ murmur3_32(dni, seed2);
    return murmur3_32(dni, dynamic_seed) % size;
}

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

void CuckooHashTab::insertar(uint32_t dni, uint32_t offset) {
    Entry entry = { dni, offset };
    Entry temp_entry = entry;

    for (int count = 0; count < rehash_limit; ++count) {
        for (int i = 0; i < num_tables; ++i) {
            int h = hash(i, temp_entry.dni);
            if (tables[i][h].dni == 0) { 
                tables[i][h] = temp_entry;
                return;
            }
            std::swap(temp_entry, tables[i][h]);
        }
    }

    std::cerr << "Rehash limit reached. Performing rehashing...\n";
    rehash();
    insertar(dni, offset); 
}

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

void CuckooHashTab::eliminar(uint32_t dni) {
    for (int i = 0; i < num_tables; ++i) {
        int h = hash(i, dni);
        if (tables[i][h].dni == dni) {
            tables[i][h].dni = 0;    
            tables[i][h].offset = 0;
            return;
        }
    }
    std::cerr << "Error: DNI " << dni << " no encontrado para eliminar.\n";
}

void CuckooHashTab::rehash() {
    std::cout << "Rehashing tables... Current size per table: " << size
              << " -> New size per table: " << size * 2 << "\n";
    size *= 2; 
    // Crear nuevas tablas con el tamaño actualizado
    std::vector<std::vector<Entry>> new_tables(num_tables, std::vector<Entry>(size, Entry{0, 0}));

    for (int i = 0; i < num_tables; ++i) {
        for (const auto& entry : tables[i]) {
            if (entry.dni != 0) { 
                bool inserted = false;
                Entry temp_entry = entry;

                for (int count = 0; count < rehash_limit; ++count) {
                    for (int j = 0; j < num_tables; ++j) {
                        int h = hash(j, temp_entry.dni);
                        if (new_tables[j][h].dni == 0) { 
                            new_tables[j][h] = temp_entry;
                            inserted = true;
                            break;
                        }
                        std::swap(temp_entry, new_tables[j][h]);
                    }
                    if (inserted) break;
                }

                if (!inserted) {
                    std::cerr << "Fallo en reinsertar DNI " << entry.dni << " en el rehashing.\n";
                }
            }
        }
    }

    // Reemplazar las tablas antiguas por las nuevas
    tables = std::move(new_tables);
}

bool CuckooHashTab::insertRehash(const Entry& entry, std::vector<std::vector<Entry>>& new_tables, int rehash_count) {
    if (rehash_count >= rehash_limit) {
        return false;
    }

    Entry temp_entry = entry;

    for (int count = 0; count < rehash_limit; ++count) {
        for (int i = 0; i < num_tables; ++i) {
            int h = hash(i, temp_entry.dni);
            if (new_tables[i][h].dni == 0) { 
                new_tables[i][h] = temp_entry;
                return true;
            }
            std::swap(temp_entry, new_tables[i][h]);
        }
    }

    return false;
}
