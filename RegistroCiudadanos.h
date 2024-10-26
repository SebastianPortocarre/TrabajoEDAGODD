#ifndef REGISTROCIUDADANOS_H
#define REGISTROCIUDADANOS_H

#include <cstdint>
#include <string>
#include "CuckooHashTab.h"
#include "Ciudadano.h"
#include "Tablas.h"

class RegistroCiudadanos {
public:
    RegistroCiudadanos();
    ~RegistroCiudadanos();

    // Funciones principales
    void generarCiudadanosAleatorios(int cantidad);    // Generar ciudadanos aleatorios
    void insertarCiudadanoManual();                     // Insertar ciudadano manualmente
    CiudadanoOptimizado* buscarCiudadano(uint32_t dni); // Buscar ciudadano por DNI
    bool eliminarCiudadano(uint32_t dni);              // Eliminar ciudadano por DNI
    void imprimirTresDniAleatorios();                   // Imprimir tres DNIs aleatorios
    void exportarACSV(const std::string& nombre_archivo); // Exportar datos a CSV

    Tablas tablas;

private:
    CuckooHashTab cuckooHash; // Tabla hash de Cuckoo
    std::vector<uint32_t> dnis; // Vector para almacenar los DNIs (para funciones como imprimir DNIs aleatorios)

    static const uint32_t DNI_MIN = 10000000;
    static const uint32_t DNI_MAX = 99999999;

    // Archivo binario para almacenar los datos comprimidos
    std::string nombre_archivo_binario;

    // Funciones auxiliares para generar datos aleatorios
    uint32_t generarDniAleatorio();
    std::string generarNombreAleatorio();
    std::string generarApellidoAleatorio();
    uint32_t generarTelefonoAleatorio();
    std::string generarEmailAleatorio(const std::string& nombre, const std::string& dominio);
    std::string generarUbicacionAleatoria();

    // Funciones para cargar y guardar datos
    bool cargarDesdeArchivo();
    bool guardarEnArchivo();
};

#endif // REGISTROCIUDADANOS_H
