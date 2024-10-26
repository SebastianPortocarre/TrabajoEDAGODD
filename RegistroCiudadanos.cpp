#include "RegistroCiudadanos.h"
#include "Compressor.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <string>
#include <cstring>
#include <cctype>

using namespace std;
namespace fs = std::filesystem;

RegistroCiudadanos::RegistroCiudadanos()
    : nombre_archivo_data("ciudadanos_data.bin"),
      nombre_archivo_index("ciudadanos_index.bin"),
      cuckooHash(33000000, 2) {
    if (!tablas.cargarTablas("tablas")) {
        std::cout << "No se pudieron cargar las tablas. Se crearán nuevas.\n";
    }

    if (!cargarDesdeArchivo()) {
        cout << "Generando 1 millón de ciudadanos aleatorios...\n";
        generarCiudadanosAleatorios(1000000);
        guardarEnArchivo();
        tablas.guardarTablas("tablas");
    } else {
        cout << "Datos cargados exitosamente desde los archivos binarios.\n";
    }
}

RegistroCiudadanos::~RegistroCiudadanos() {
    guardarEnArchivo();
    tablas.guardarTablas("tablas");
}

void RegistroCiudadanos::generarCiudadanosAleatorios(int cantidad) {
    if (cantidad <= 0) {
        cerr << "Cantidad inválida para generar ciudadanos.\n";
        return;
    }

    ofstream outfile_data(nombre_archivo_data, ios::binary | ios::trunc);
    if (!outfile_data.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo_data << " para escribir.\n";
        return;
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dis_dni(DNI_MIN, DNI_MAX);

    vector<string> nombres = {
        "Juan", "María", "Carlos", "Ana", "Luis", "Carmen",
        "Pedro", "Lucía", "Jorge", "Sofía", "Miguel", "Elena",
        "Andrés", "Isabel", "Fernando", "Laura", "Ricardo", "Patricia",
        "Daniel", "Claudia", "Gabriel", "Fernanda", "Santiago", "Valeria",
        "Diego", "Paula", "Roberto", "Verónica", "Sebastián", "Natalia"
    };
    vector<string> apellidos = {
        "Pérez", "González", "Rodríguez", "López", "García", "Martínez",
        "Sánchez", "Ramírez", "Torres", "Flores", "Díaz", "Morales",
        "Vásquez", "Jiménez", "Rojas", "Álvarez", "Castillo", "Vega",
        "Ortiz", "Silva", "Mendoza", "Cortez", "Ruiz", "Reyes",
        "Fernández", "Herrera", "Molina", "Navarro", "Ramos", "Romero"
    };
    vector<string> departamentos_lista = { "Lima", "Arequipa", "Cusco", "La Libertad", "Piura", "Junín", "Ancash", "Puno", "Apurímac", "Tacna" };
    vector<string> provincias_lista = { "Lima", "Arequipa", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    vector<string> ciudades_lista = { "Lima", "Arequipa", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    vector<string> distritos_lista = { "Miraflores", "San Isidro", "Cusco", "Trujillo", "Piura", "Huancayo", "Huaraz", "Juliaca", "Abancay", "Tacna" };
    vector<string> ubicaciones_lista = { "Av. Pardo 123", "Calle 50 #456", "Jr. Las Flores 789", "Av. Arequipa 321", "Calle Lima 654", "Av. Cusco 987", "Calle Trujillo 111", "Jr. Piura 222", "Av. Junín 333", "Calle Tacna 444" };
    vector<string> dominios = { "example.com", "correo.com", "mail.com", "peru.com", "hotmail.com", "gmail.com", "yahoo.com", "live.com", "outlook.com", "universidad.edu.pe" };

    // Generador para seleccionar elementos aleatorios de las listas
    uniform_int_distribution<> dis_nombres(0, nombres.size() - 1);
    uniform_int_distribution<> dis_apellidos_dist(0, apellidos.size() - 1);
    uniform_int_distribution<> dis_departamentos(0, departamentos_lista.size() - 1);
    uniform_int_distribution<> dis_provincias(0, provincias_lista.size() - 1);
    uniform_int_distribution<> dis_ciudades_dist(0, ciudades_lista.size() - 1);
    uniform_int_distribution<> dis_distritos(0, distritos_lista.size() - 1);
    uniform_int_distribution<> dis_ubicaciones(0, ubicaciones_lista.size() - 1);
    uniform_int_distribution<> dis_dominios_dist(0, dominios.size() - 1);
    uniform_int_distribution<> dis_nacionalidad(0, 1);
    uniform_int_distribution<> dis_estado_civil(0, 3);
    uniform_int_distribution<uint32_t> dis_telefono(900000000, 999999999);

    vector<pair<uint32_t, uint32_t>> index_entries;

    for (int i = 0; i < cantidad; ++i) {
        CiudadanoOptimizado ciudadano;
        uint32_t dni;
        uint32_t offset;

        do {
            dni = dis_dni(gen);
        } while (cuckooHash.buscar(dni, offset)); 

        ciudadano.dni = dni;
        dnis.push_back(dni); 

        string nombre = nombres[dis_nombres(gen)];
        string apellido = apellidos[dis_apellidos_dist(gen)];
        string nombre_completo = nombre + " " + apellido;
        ciudadano.nombres_apellidos = tablas.obtenerIndiceNombreApellido(nombre_completo);

        string lugar_nacimiento = ciudades_lista[dis_ciudades_dist(gen)];
        ciudadano.lugar_nacimiento = tablas.obtenerIndiceLugarNacimiento(lugar_nacimiento);

        ciudadano.nacionalidad = (dis_nacionalidad(gen) == 0) ? Nacionalidad::Peruano : Nacionalidad::Extranjero;

        string departamento = departamentos_lista[dis_departamentos(gen)];
        ciudadano.direccion.departamento = tablas.obtenerIndiceDepartamento(departamento);
        string provincia = provincias_lista[dis_provincias(gen)];
        ciudadano.direccion.provincia = tablas.obtenerIndiceProvincia(provincia);
        string ciudad = ciudades_lista[dis_ciudades_dist(gen)];
        ciudadano.direccion.ciudad = tablas.obtenerIndiceCiudad(ciudad);
        string distrito = distritos_lista[dis_distritos(gen)];
        ciudadano.direccion.distrito = tablas.obtenerIndiceDistrito(distrito);
        string ubicacion = ubicaciones_lista[dis_ubicaciones(gen)];
        ciudadano.direccion.ubicacion = tablas.obtenerIndiceUbicacion(ubicacion);

        ciudadano.telefono = dis_telefono(gen);

        string email = generarEmailAleatorio(nombre + apellido, dominios[dis_dominios_dist(gen)]);
        ciudadano.email = tablas.obtenerIndiceEmail(email);

        ciudadano.estado_civil = static_cast<EstadoCivil>(dis_estado_civil(gen));

        // Serializar y comprimir el ciudadano
        string ciudadanoData(reinterpret_cast<char*>(&ciudadano), sizeof(CiudadanoOptimizado));
        vector<char> compressedData = Compressor::compress(ciudadanoData);

        // Obtener el offset actual del archivo de datos
        uint32_t current_offset = outfile_data.tellp();

        // Escribir los datos comprimidos al archivo de datos
        uint32_t compressedSize = compressedData.size();
        outfile_data.write(reinterpret_cast<const char*>(&compressedSize), sizeof(uint32_t)); // Escribir el tamaño
        outfile_data.write(compressedData.data(), compressedSize);

        cuckooHash.insertar(dni, current_offset);

        // Guardar el índice temporalmente
        index_entries.emplace_back(dni, current_offset);

        if ((i + 1) % 1000000 == 0) {
            cout << (i + 1) << " ciudadanos generados.\n";
        }
    }

    outfile_data.close();
    cout << "Generación de ciudadanos aleatorios completada.\n";

    // Guardar el índice en el archivo de índice
    ofstream outfile_index(nombre_archivo_index, ios::binary | ios::trunc);
    if (!outfile_index.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo_index << " para escribir el índice.\n";
        return;
    }

    uint32_t num_registros = index_entries.size();
    outfile_index.write(reinterpret_cast<const char*>(&num_registros), sizeof(num_registros));

    for (const auto& [dni, offset] : index_entries) {
        outfile_index.write(reinterpret_cast<const char*>(&dni), sizeof(dni));
        outfile_index.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
    }

    outfile_index.close();
    cout << "Índice guardado correctamente en " << nombre_archivo_index << ".\n";
}

CiudadanoOptimizado* RegistroCiudadanos::buscarCiudadano(uint32_t dni) {
    uint32_t offset;
    if (cuckooHash.buscar(dni, offset)) {
        ifstream infile_data(nombre_archivo_data, ios::binary);
        if (!infile_data.is_open()) {
            cerr << "No se pudo abrir el archivo " << nombre_archivo_data << " para leer.\n";
            return nullptr;
        }

        // Mover el cursor al offset
        infile_data.seekg(offset, ios::beg);

        // Leer el tamaño de los datos comprimidos
        uint32_t compressedSize;
        infile_data.read(reinterpret_cast<char*>(&compressedSize), sizeof(compressedSize));

        // Verificar que la lectura haya sido exitosa
        if (infile_data.fail()) {
            cerr << "Error al leer el tamaño comprimido para el DNI " << dni << ".\n";
            infile_data.close();
            return nullptr;
        }

        // Leer los datos comprimidos
        vector<char> compressedData(compressedSize);
        infile_data.read(compressedData.data(), compressedSize);

        if (infile_data.gcount() != static_cast<std::streamsize>(compressedSize)) {
            cerr << "Error al leer los datos comprimidos para el DNI " << dni << ".\n";
            infile_data.close();
            return nullptr;
        }

        // Descomprimir y deserializar
        string decompressedData = Compressor::decompress(compressedData);
        if (decompressedData.size() < sizeof(CiudadanoOptimizado)) {
            cerr << "Error: Datos descomprimidos incompletos para el DNI " << dni << ".\n";
            infile_data.close();
            return nullptr;
        }
        CiudadanoOptimizado* ciudadano = new CiudadanoOptimizado();
        memcpy(ciudadano, decompressedData.data(), sizeof(CiudadanoOptimizado));

        infile_data.close();
        return ciudadano;
    } else {
        cout << "Ciudadano no encontrado.\n";
        return nullptr;
    }
}

// Función para insertar un ciudadano manualmente
void RegistroCiudadanos::insertarCiudadanoManual() {
    CiudadanoOptimizado ciudadano;
    uint32_t dni;
    uint32_t offset;
    cout << "Ingrese el DNI: ";
    cin >> dni;

    if (cuckooHash.buscar(dni, offset)) {
        cerr << "Error: El DNI ya existe.\n";
        return;
    }

    ciudadano.dni = dni;
    dnis.push_back(dni);

    string nombre, apellido;
    cout << "Ingrese el Nombre: ";
    cin >> nombre;
    cout << "Ingrese el Apellido: ";
    cin >> apellido;
    string nombre_completo = nombre + " " + apellido;
    ciudadano.nombres_apellidos = tablas.obtenerIndiceNombreApellido(nombre_completo);

    cout << "Ingrese el Lugar de Nacimiento: ";
    string lugar_nacimiento;
    cin >> lugar_nacimiento;
    ciudadano.lugar_nacimiento = tablas.obtenerIndiceLugarNacimiento(lugar_nacimiento);

    cout << "Ingrese la Nacionalidad (0: Peruano, 1: Extranjero): ";
    int nacionalidad;
    cin >> nacionalidad;
    ciudadano.nacionalidad = (nacionalidad == 0) ? Nacionalidad::Peruano : Nacionalidad::Extranjero;

    cout << "Ingrese el Departamento: ";
    string departamento;
    cin >> departamento;
    ciudadano.direccion.departamento = tablas.obtenerIndiceDepartamento(departamento);

    cout << "Ingrese la Provincia: ";
    string provincia;
    cin >> provincia;
    ciudadano.direccion.provincia = tablas.obtenerIndiceProvincia(provincia);

    cout << "Ingrese la Ciudad: ";
    string ciudad;
    cin >> ciudad;
    ciudadano.direccion.ciudad = tablas.obtenerIndiceCiudad(ciudad);

    cout << "Ingrese el Distrito: ";
    string distrito;
    cin >> distrito;
    ciudadano.direccion.distrito = tablas.obtenerIndiceDistrito(distrito);

    cout << "Ingrese la Ubicación: ";
    string ubicacion;
    cin.ignore(); 
    getline(cin, ubicacion);
    ciudadano.direccion.ubicacion = tablas.obtenerIndiceUbicacion(ubicacion);

    cout << "Ingrese el Teléfono: ";
    cin >> ciudadano.telefono;

    cout << "Ingrese el Email: ";
    string email;
    cin >> email;
    ciudadano.email = tablas.obtenerIndiceEmail(email);

    cout << "Ingrese el Estado Civil (0: Soltero, 1: Casado, 2: Divorciado, 3: Viudo): ";
    int estado_civil;
    cin >> estado_civil;
    ciudadano.estado_civil = static_cast<EstadoCivil>(estado_civil);

    string ciudadanoData(reinterpret_cast<char*>(&ciudadano), sizeof(CiudadanoOptimizado));
    vector<char> compressedData = Compressor::compress(ciudadanoData);

    // Abrir el archivo de datos en modo append
    ofstream outfile_data(nombre_archivo_data, ios::binary | ios::app);
    if (!outfile_data.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo_data << " para escribir.\n";
        return;
    }

    // Obtener el offset actual del archivo de datos
    uint32_t current_offset = outfile_data.tellp();

    // Escribir los datos comprimidos al archivo de datos
    uint32_t compressedSize = compressedData.size();
    outfile_data.write(reinterpret_cast<const char*>(&compressedSize), sizeof(uint32_t)); // Escribir el tamaño
    outfile_data.write(compressedData.data(), compressedSize);

    cuckooHash.insertar(dni, current_offset);

    outfile_data.close();

    if (!guardarEnArchivo()) {
        cerr << "Error al guardar el índice después de insertar el ciudadano.\n";
        return;
    }

    cout << "Ciudadano insertado correctamente.\n";
}

bool RegistroCiudadanos::eliminarCiudadano(uint32_t dni) {
    uint32_t offset;
    if (cuckooHash.buscar(dni, offset)) {
        cuckooHash.eliminar(dni);
        dnis.erase(std::remove(dnis.begin(), dnis.end(), dni), dnis.end()); 

        // Guardar el índice actualizado en el archivo de índice
        // Reescribir el archivo de índice con los DNIs restantes
        ofstream outfile_index(nombre_archivo_index, ios::binary | ios::trunc);
        if (!outfile_index.is_open()) {
            cerr << "No se pudo abrir el archivo " << nombre_archivo_index << " para escribir el índice.\n";
            return false;
        }

        uint32_t num_registros = dnis.size();
        outfile_index.write(reinterpret_cast<const char*>(&num_registros), sizeof(num_registros));

        for (uint32_t dni_restante : dnis) {
            uint32_t offset_restante;
            if (cuckooHash.buscar(dni_restante, offset_restante)) {
                outfile_index.write(reinterpret_cast<const char*>(&dni_restante), sizeof(dni_restante));
                outfile_index.write(reinterpret_cast<const char*>(&offset_restante), sizeof(offset_restante));
            }
        }

        outfile_index.close();

        cout << "Ciudadano con DNI " << dni << " eliminado correctamente.\n";
        return true;
    } else {
        cout << "Ciudadano con DNI " << dni << " no encontrado.\n";
        return false;
    }
}

bool RegistroCiudadanos::cargarDesdeArchivo() {
    // Asegúrate de abrir el archivo de índice correcto
    ifstream infile_index(nombre_archivo_index, ios::binary);
    if (!infile_index.is_open()) {
        cerr << "Archivo de índice no encontrado. Se generarán nuevos datos.\n";
        return false;
    }

    // Leer el número de registros
    uint32_t num_registros;
    infile_index.read(reinterpret_cast<char*>(&num_registros), sizeof(num_registros));

    // Leer cada DNI y su offset
    for (uint32_t i = 0; i < num_registros; ++i) {
        uint32_t dni;
        uint32_t offset;
        infile_index.read(reinterpret_cast<char*>(&dni), sizeof(dni));
        infile_index.read(reinterpret_cast<char*>(&offset), sizeof(offset));

        if (infile_index.fail()) {
            cerr << "Error al leer el índice para el registro " << i + 1 << ".\n";
            infile_index.close();
            return false;
        }

        dnis.push_back(dni);
        cuckooHash.insertar(dni, offset);
    }

    infile_index.close();

    cout << "Cargados " << num_registros << " registros desde los archivos binarios.\n";
    return true;
}

// Función para guardar los datos en los archivos binarios
bool RegistroCiudadanos::guardarEnArchivo() {
    // Reescribir el archivo de índice
    ofstream outfile_index(nombre_archivo_index, ios::binary | ios::trunc);
    if (!outfile_index.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo_index << " para escribir el índice.\n";
        return false;
    }

    uint32_t num_registros = dnis.size();
    outfile_index.write(reinterpret_cast<const char*>(&num_registros), sizeof(num_registros));

    for (uint32_t dni : dnis) {
        uint32_t offset;
        if (cuckooHash.buscar(dni, offset)) {
            outfile_index.write(reinterpret_cast<const char*>(&dni), sizeof(dni));
            outfile_index.write(reinterpret_cast<const char*>(&offset), sizeof(offset));
        }
    }

    outfile_index.close();

    cout << "Índice guardado correctamente en " << nombre_archivo_index << ".\n";
    return true;
}


// Función para imprimir tres DNIs aleatorios
void RegistroCiudadanos::imprimirTresDniAleatorios() {
    if (dnis.empty()) {
        cerr << "No hay ciudadanos registrados.\n";
        return;
    }
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<size_t> dis(0, dnis.size() - 1);
    cout << "Tres DNIs aleatorios:\n";
    for (int i = 0; i < 3; ++i) {
        size_t idx = dis(gen);
        cout << (i + 1) << ". " << dnis[idx] << "\n";
    }
}

// Función para exportar los datos a un archivo CSV
void RegistroCiudadanos::exportarACSV(const string& nombre_archivo) {
    cout << "Iniciando exportación a CSV: " << nombre_archivo << "\n";
    ofstream archivo_csv(nombre_archivo);
    if (!archivo_csv.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo << " para escribir.\n";
        return;
    }
    // Escribir la cabecera del CSV
    archivo_csv << "DNI,NombreApellido,LugarNacimiento,Nacionalidad,Departamento,Provincia,Ciudad,Distrito,Ubicacion,Telefono,Email,EstadoCivil\n";

    for (uint32_t dni : dnis) {
        CiudadanoOptimizado* ciudadano = buscarCiudadano(dni);
        if (ciudadano) {
            // Verificar índices y escribir los datos
            if (ciudadano->nombres_apellidos >= tablas.nombres_apellidos.size() ||
                ciudadano->lugar_nacimiento >= tablas.lugares_nacimiento.size() ||
                ciudadano->direccion.departamento >= tablas.departamentos.size() ||
                ciudadano->direccion.provincia >= tablas.provincias.size() ||
                ciudadano->direccion.ciudad >= tablas.ciudades.size() ||
                ciudadano->direccion.distrito >= tablas.distritos.size() ||
                ciudadano->direccion.ubicacion >= tablas.ubicaciones.size() ||
                ciudadano->email >= tablas.emails.size()) {
                cerr << "Error: Índices fuera de rango para el ciudadano con DNI " << ciudadano->dni << ".\n";
                delete ciudadano;
                continue; // Saltar este registro
            }
            archivo_csv << ciudadano->dni << ","
                       << "\"" << tablas.nombres_apellidos[ciudadano->nombres_apellidos] << "\","
                       << "\"" << tablas.lugares_nacimiento[ciudadano->lugar_nacimiento] << "\","
                       << "\"" << ((ciudadano->nacionalidad == Nacionalidad::Peruano) ? "Peruano" : "Extranjero") << "\","
                       << "\"" << tablas.departamentos[ciudadano->direccion.departamento] << "\","
                       << "\"" << tablas.provincias[ciudadano->direccion.provincia] << "\","
                       << "\"" << tablas.ciudades[ciudadano->direccion.ciudad] << "\","
                       << "\"" << tablas.distritos[ciudadano->direccion.distrito] << "\","
                       << "\"" << tablas.ubicaciones[ciudadano->direccion.ubicacion] << "\","
                       << ciudadano->telefono << ","
                       << "\"" << tablas.emails[ciudadano->email] << "\","
                       << "\"" << ((ciudadano->estado_civil == EstadoCivil::Soltero) ? "Soltero" :
                                   (ciudadano->estado_civil == EstadoCivil::Casado) ? "Casado" :
                                   (ciudadano->estado_civil == EstadoCivil::Divorciado) ? "Divorciado" :
                                   "Viudo") << "\"\n";

            delete ciudadano;
        }
    }

    archivo_csv.close();
    cout << "Datos exportados correctamente a " << nombre_archivo << ".\n";
}
string RegistroCiudadanos::generarEmailAleatorio(const string& nombre, const string& dominio) {
    string email = nombre;
    // Reemplazar espacios con puntos y convertir a minúsculas
    replace(email.begin(), email.end(), ' ', '.');
    transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return tolower(c); });
    email += "@" + dominio;
    return email;
}
