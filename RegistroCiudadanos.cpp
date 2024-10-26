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

RegistroCiudadanos::RegistroCiudadanos()
    : nombre_archivo_binario("ciudadanos.bin"), cuckooHash(33000000, 2) {
    if (!cargarDesdeArchivo()) {
        cout << "Generando 33 millón de ciudadanos aleatorios...\n";
        generarCiudadanosAleatorios(33000000);
        guardarEnArchivo();
    } else {
        cout << "Datos cargados exitosamente desde " << nombre_archivo_binario << ".\n";
    }
}

RegistroCiudadanos::~RegistroCiudadanos() {
    guardarEnArchivo();
}

void RegistroCiudadanos::generarCiudadanosAleatorios(int cantidad) {
    if (cantidad <= 0) {
        cerr << "Cantidad inválida para generar ciudadanos.\n";
        return;
    }

    // Abrir el archivo binario en modo escritura (trunc para sobrescribir)
    ofstream outfile(nombre_archivo_binario, ios::binary | ios::trunc);
    if (!outfile.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo_binario << " para escribir.\n";
        return;
    }

    // Configuración de generadores aleatorios
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dis_dni(DNI_MIN, DNI_MAX);

    // Listas ampliadas de nombres y apellidos para generar nombres completos
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

    for (int i = 0; i < cantidad; ++i) {
        CiudadanoOptimizado ciudadano;
        uint32_t dni;
        uint32_t offset;

        // Generar DNI único
        do {
            dni = dis_dni(gen);
        } while (cuckooHash.buscar(dni, offset)); // Usar variable separada para 'offset'

        ciudadano.dni = dni;
        dnis.push_back(dni); // Guardamos el DNI

        // Generar Nombre y Apellido
        string nombre = nombres[dis_nombres(gen)];
        string apellido = apellidos[dis_apellidos_dist(gen)];
        string nombre_completo = nombre + " " + apellido;
        ciudadano.nombres_apellidos = tablas.obtenerIndiceNombreApellido(nombre_completo);

        // Generar Lugar de Nacimiento
        string lugar_nacimiento = ciudades_lista[dis_ciudades_dist(gen)];
        ciudadano.lugar_nacimiento = tablas.obtenerIndiceLugarNacimiento(lugar_nacimiento);

        // Generar Nacionalidad
        ciudadano.nacionalidad = (dis_nacionalidad(gen) == 0) ? Nacionalidad::Peruano : Nacionalidad::Extranjero;

        // Generar Dirección
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

        // Generar Teléfono
        ciudadano.telefono = dis_telefono(gen);

        // Generar Email
        string email = generarEmailAleatorio(nombre + apellido, dominios[dis_dominios_dist(gen)]);
        ciudadano.email = tablas.obtenerIndiceEmail(email);

        // Generar Estado Civil
        ciudadano.estado_civil = static_cast<EstadoCivil>(dis_estado_civil(gen));

        // Serializar y comprimir el ciudadano
        string ciudadanoData(reinterpret_cast<char*>(&ciudadano), sizeof(CiudadanoOptimizado));
        vector<char> compressedData = Compressor::compress(ciudadanoData);

        // Obtener el offset actual del archivo
        uint32_t current_offset = outfile.tellp();

        // Escribir los datos comprimidos al archivo
        uint32_t compressedSize = compressedData.size();
        outfile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(uint32_t)); // Escribir el tamaño
        outfile.write(compressedData.data(), compressedSize);

        // Insertar en la tabla hash de Cuckoo
        cuckooHash.insertar(dni, current_offset);

        // Mostrar progreso cada 100,000 registros
        if ((i + 1) % 1000000 == 0) {
            cout << (i + 1) << " ciudadanos generados.\n";
        }
    }

    outfile.close();
    cout << "Generación de ciudadanos aleatorios completada.\n";
}

// Función para buscar un ciudadano por DNI
CiudadanoOptimizado* RegistroCiudadanos::buscarCiudadano(uint32_t dni) {
    uint32_t offset;
    if (cuckooHash.buscar(dni, offset)) {
        ifstream infile(nombre_archivo_binario, ios::binary);
        if (!infile.is_open()) {
            cerr << "No se pudo abrir el archivo " << nombre_archivo_binario << " para leer.\n";
            return nullptr;
        }

        // Mover el cursor al offset
        infile.seekg(offset, ios::beg);

        // Leer el tamaño de los datos comprimidos
        uint32_t compressedSize;
        infile.read(reinterpret_cast<char*>(&compressedSize), sizeof(uint32_t));

        // Leer los datos comprimidos
        vector<char> compressedData(compressedSize);
        infile.read(compressedData.data(), compressedSize);

        // Descomprimir y deserializar
        string decompressedData = Compressor::decompress(compressedData);
        if (decompressedData.size() < sizeof(CiudadanoOptimizado)) {
            cerr << "Error: Datos descomprimidos incompletos para el DNI " << dni << ".\n";
            infile.close();
            return nullptr;
        }
        CiudadanoOptimizado* ciudadano = new CiudadanoOptimizado();
        memcpy(ciudadano, decompressedData.data(), sizeof(CiudadanoOptimizado));

        infile.close();
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
    dnis.push_back(dni); // Guardamos el DNI

    // Ingresar los demás datos del ciudadano
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

    // Ingreso de Dirección
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
    cin.ignore(); // Limpiar el buffer antes de getline
    getline(cin, ubicacion);
    ciudadano.direccion.ubicacion = tablas.obtenerIndiceUbicacion(ubicacion);

    // Ingreso de Teléfono
    cout << "Ingrese el Teléfono: ";
    cin >> ciudadano.telefono;

    // Ingreso de Email
    cout << "Ingrese el Email: ";
    string email;
    cin >> email;
    ciudadano.email = tablas.obtenerIndiceEmail(email);

    // Ingreso de Estado Civil
    cout << "Ingrese el Estado Civil (0: Soltero, 1: Casado, 2: Divorciado, 3: Viudo): ";
    int estado_civil;
    cin >> estado_civil;
    ciudadano.estado_civil = static_cast<EstadoCivil>(estado_civil);

    // Serializar y comprimir el ciudadano
    string ciudadanoData(reinterpret_cast<char*>(&ciudadano), sizeof(CiudadanoOptimizado));
    vector<char> compressedData = Compressor::compress(ciudadanoData);

    // Abrir el archivo binario en modo append
    ofstream outfile(nombre_archivo_binario, ios::binary | ios::app);
    if (!outfile.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombre_archivo_binario << " para escribir.\n";
        return;
    }

    // Obtener el offset actual del archivo
    uint32_t current_offset = outfile.tellp();

    // Escribir los datos comprimidos al archivo
    uint32_t compressedSize = compressedData.size();
    outfile.write(reinterpret_cast<const char*>(&compressedSize), sizeof(uint32_t)); // Escribir el tamaño
    outfile.write(compressedData.data(), compressedSize);

    // Insertar en la tabla hash de Cuckoo
    cuckooHash.insertar(dni, current_offset);

    outfile.close();
    cout << "Ciudadano insertado correctamente.\n";
}

// Función para eliminar un ciudadano por DNI
bool RegistroCiudadanos::eliminarCiudadano(uint32_t dni) {
    uint32_t offset;
    if (cuckooHash.buscar(dni, offset)) {
        cuckooHash.eliminar(dni);
        cout << "Ciudadano con DNI " << dni << " eliminado correctamente.\n";
        return true;
    } else {
        cout << "Ciudadano con DNI " << dni << " no encontrado.\n";
        return false;
    }
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

// Funciones para cargar y guardar los datos de la tabla hash y los DNIs

bool RegistroCiudadanos::cargarDesdeArchivo() {
    // Implementación opcional: Si deseas cargar los DNIs y la tabla hash desde un archivo
    // Por ahora, retornamos false para generar nuevos datos
    return false;
}

bool RegistroCiudadanos::guardarEnArchivo() {
    // Implementación opcional: Si deseas guardar los DNIs y la tabla hash en un archivo
    // Por ahora, no hacemos nada y retornamos true
    return true;
}

// Funciones auxiliares para generar datos aleatorios
uint32_t RegistroCiudadanos::generarDniAleatorio() {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dis(DNI_MIN, DNI_MAX);
    return dis(gen);
}

string RegistroCiudadanos::generarNombreAleatorio() {
    static vector<string> nombres = {
        "Juan", "María", "Carlos", "Ana", "Luis", "Carmen",
        "Pedro", "Lucía", "Jorge", "Sofía", "Miguel", "Elena",
        "Andrés", "Isabel", "Fernando", "Laura", "Ricardo", "Patricia",
        "Daniel", "Claudia", "Gabriel", "Fernanda", "Santiago", "Valeria",
        "Diego", "Paula", "Roberto", "Verónica", "Sebastián", "Natalia"
    };
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, nombres.size() - 1);
    return nombres[dis(gen)];
}

string RegistroCiudadanos::generarApellidoAleatorio() {
    static vector<string> apellidos = {
        "Pérez", "González", "Rodríguez", "López", "García", "Martínez",
        "Sánchez", "Ramírez", "Torres", "Flores", "Díaz", "Morales",
        "Vásquez", "Jiménez", "Rojas", "Álvarez", "Castillo", "Vega",
        "Ortiz", "Silva", "Mendoza", "Cortez", "Ruiz", "Reyes",
        "Fernández", "Herrera", "Molina", "Navarro", "Ramos", "Romero"
    };
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, apellidos.size() - 1);
    return apellidos[dis(gen)];
}

uint32_t RegistroCiudadanos::generarTelefonoAleatorio() {
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<uint32_t> dis(900000000, 999999999);
    return dis(gen);
}

string RegistroCiudadanos::generarEmailAleatorio(const string& nombre, const string& dominio) {
    string email = nombre;
    // Reemplazar espacios con puntos y convertir a minúsculas
    replace(email.begin(), email.end(), ' ', '.');
    transform(email.begin(), email.end(), email.begin(), [](unsigned char c){ return tolower(c); });
    email += "@" + dominio;
    return email;
}

string RegistroCiudadanos::generarUbicacionAleatoria() {
    static vector<string> ubicaciones = {
        "Av. Pardo 123", "Calle 50 #456", "Jr. Las Flores 789",
        "Av. Arequipa 321", "Calle Lima 654", "Av. Cusco 987",
        "Calle Trujillo 111", "Jr. Piura 222", "Av. Junín 333", "Calle Tacna 444"
    };
    static random_device rd;
    static mt19937 gen(rd());
    uniform_int_distribution<> dis(0, ubicaciones.size() - 1);
    return ubicaciones[dis(gen)];
}
