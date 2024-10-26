#ifndef REGISTROCIUDADANOS_H
#define REGISTROCIUDADANOS_H

#include <cstdint>
#include <string>
#include <vector>
#include "CuckooHashTab.h"
#include "Ciudadano.h"
#include "Tablas.h"

class RegistroCiudadanos {
public:
    RegistroCiudadanos();
    ~RegistroCiudadanos();

    void generarCiudadanosAleatorios(int cantidad);  
    void insertarCiudadanoManual();                   
    CiudadanoOptimizado* buscarCiudadano(uint32_t dni);
    bool eliminarCiudadano(uint32_t dni);             
    void imprimirTresDniAleatorios();                  
    void exportarACSV(const std::string& nombre_archivo); 

    Tablas tablas;

private:
    CuckooHashTab cuckooHash;
    std::vector<uint32_t> dnis;

    static const uint32_t DNI_MIN = 10000000;
    static const uint32_t DNI_MAX = 99999999;

    std::string nombre_archivo_data;
    std::string nombre_archivo_index;

    uint32_t generarDniAleatorio();
    std::string generarNombreAleatorio();
    std::string generarApellidoAleatorio();
    uint32_t generarTelefonoAleatorio();
    std::string generarEmailAleatorio(const std::string& nombre, const std::string& dominio);
    std::string generarUbicacionAleatoria();

    bool cargarDesdeArchivo();
    bool guardarEnArchivo();
};

#endif
