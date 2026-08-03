#Version Minima de Cmake
cmake_minimum_required(VERSION 3.10)

#Nombre del Proyecto
project(BenjasScript C CXX)

#Estandar de C++
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)


# Incluimos los directorios de Headers locales y del SDK     ----.
include_directories(
    Headers
    external/unitree_sdk2/include
)

# Definimos los archivos fuente explícitamente (Más seguro que GLOB para evitar archivos fantasma)
set(SOURCES
    Sources/main.c
    Sources/parserRDCP.c
    Sources/scanner.c
    Sources/VM.cpp        
    )

# Crear el ejecutable
add_executable(BenjasScript ${SOURCES})


#Vincular librerias del SDK
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/build/lib)           -----.

# Copia el archivo de prueba a la carpeta del ejecutable
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/SourceCode.txt"
    "${CMAKE_CURRENT_BINARY_DIR}/SourceCode.txt"
    COPYONLY
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(UNITREE_ARCH "aarch64")
    message(STATUS "Arquitectura detectada: ARM (aarch64)")
else()
    set(UNITREE_ARCH "x86_64")
    message(STATUS "Arquitectura detectada: Intel/AMD (x86_64)")
endif()

#linkear librerias
target_link_libraries(BenjasScript PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/lib/${UNITREE_ARCH}/libunitree_sdk2.a"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/thirdparty/lib/${UNITREE_ARCH}/libddscxx.so"
    "${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/thirdparty/lib/${UNITREE_ARCH}/libddsc.so"
    "-Wl,-rpath=${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/thirdparty/lib/${UNITREE_ARCH}"
    pthread
    m
)

#linkear directorios
target_include_directories(BenjasScript PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/thirdparty/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/unitree_sdk2/thirdparty/include/ddscxx
)