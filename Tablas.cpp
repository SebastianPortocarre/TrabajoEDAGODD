#include "Tablas.h"
using namespace std;
// Función para obtener o asignar índice a Nombre y Apellido
uint32_t Tablas::obtenerIndiceNombreApellido(const string& nombre_apellido) {
    auto nom = mapa_nombres_apellidos.find(nombre_apellido);
    if (nom != mapa_nombres_apellidos.end()) {
        return nom->second;
    } else {
        uint32_t indice = nombres_apellidos.size();
        nombres_apellidos.push_back(nombre_apellido);
        mapa_nombres_apellidos[nombre_apellido] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Lugar de Nacimiento
uint32_t Tablas::obtenerIndiceLugarNacimiento(const string& lugar_nacimiento) {
    auto it = mapa_lugares_nacimiento.find(lugar_nacimiento);
    if (it != mapa_lugares_nacimiento.end()) {
        return it->second;
    } else {
        uint32_t indice = lugares_nacimiento.size();
        lugares_nacimiento.push_back(lugar_nacimiento);
        mapa_lugares_nacimiento[lugar_nacimiento] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Departamento
uint8_t Tablas::obtenerIndiceDepartamento(const string& departamento) {
    auto it = mapa_departamentos.find(departamento);
    if (it != mapa_departamentos.end()) {
        return it->second;
    } else {
        uint8_t indice = departamentos.size();
        departamentos.push_back(departamento);
        mapa_departamentos[departamento] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Provincia
uint16_t Tablas::obtenerIndiceProvincia(const string& provincia) {
    auto it = mapa_provincias.find(provincia);
    if (it != mapa_provincias.end()) {
        return it->second;
    } else {
        uint16_t indice = provincias.size();
        provincias.push_back(provincia);
        mapa_provincias[provincia] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Ciudad
uint16_t Tablas::obtenerIndiceCiudad(const string& ciudad) {
    auto it = mapa_ciudades.find(ciudad);
    if (it != mapa_ciudades.end()) {
        return it->second;
    } else {
        uint16_t indice = ciudades.size();
        ciudades.push_back(ciudad);
        mapa_ciudades[ciudad] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Distrito
uint16_t Tablas::obtenerIndiceDistrito(const string& distrito) {
    auto it = mapa_distritos.find(distrito);
    if (it != mapa_distritos.end()) {
        return it->second;
    } else {
        uint16_t indice = distritos.size();
        distritos.push_back(distrito);
        mapa_distritos[distrito] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Ubicación
uint32_t Tablas::obtenerIndiceUbicacion(const string& ubicacion) {
    auto it = mapa_ubicaciones.find(ubicacion);
    if (it != mapa_ubicaciones.end()) {
        return it->second;
    } else {
        uint32_t indice = ubicaciones.size();
        ubicaciones.push_back(ubicacion);
        mapa_ubicaciones[ubicacion] = indice;
        return indice;
    }
}

// Función para obtener o asignar índice a Email
uint32_t Tablas::obtenerIndiceEmail(const string& email) {
    auto it = mapa_emails.find(email);
    if (it != mapa_emails.end()) {
        return it->second;
    } else {
        uint32_t indice = emails.size();
        emails.push_back(email);
        mapa_emails[email] = indice;
        return indice;
    }
}
