cmake_minimum_required(VERSION 3.29)
project(Trabajo_final_eda)

set(CMAKE_CXX_STANDARD 14)

include_directories("/opt/homebrew/include")

link_directories("/opt/homebrew/lib")

add_executable(Trabajo_final_eda
        main.cpp
        CuckooHashTab.cpp
        Compressor.cpp
        RegistroCiudadanos.cpp
        Tablas.cpp
)

target_link_libraries(Trabajo_final_eda
        /opt/homebrew/lib/libzstd.dylib
)
