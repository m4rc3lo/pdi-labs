#Nome do Projeto
project(Vision)


# Esta linha é importante para garantir que não se tente usar uma versão antiga
# do CMake. Freqüentemente são introduzidas novas features, então é importante.
cmake_minimum_required(VERSION 3.0.0)


set(CMAKE_CXX_COMPILER "g++")
#SET(CMAKE_CXX_COMPILER "arm-linux-gnueabi-g++")

# Seta o compilador para trabalhar com as diretivas do C++ 11.
set(GCC_COVERAGE_COMPILE_FLAGS "-std=c++11")
set( CMAKE_CXX_FLAGS  "${GCC_COVERAGE_COMPILE_FLAGS}" )

set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -Wall -pedantic -Wextra -fpermissive" )

set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2" )

set(MAIN_FILE "app/run.cpp" CACHE STRING "Main file ...")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
#find_package(Qt5Widgets)
find_package(Qt5Core ${QT_VERSION_REQ} REQUIRED)
find_package(Qt5Widgets ${QT_VERSION_REQ} REQUIRED)

set (CMAKE_PREFIX_PATH $ENV{HOME}/Qt5.5.0/5.5/gcc_64)

set (VisoinLib_src ${PROJECT_SOURCE_DIR}/src/gui/PointList.cpp)
set (VisoinLib_hdr ${PROJECT_SOURCE_DIR}/include/gui/PointList.h)
set (VisoinLib_ui  ${PROJECT_SOURCE_DIR}/PointList.ui)
set (VisoinLib_src ${PROJECT_SOURCE_DIR}/test.cpp)

qt5_wrap_cpp(VisionLib_hdr_moc ${VisiontLib_hdr})
qt5_wrap_ui (VisionLib_ui_moc  ${VisionLib_ui})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

#add_library (VisionLib SHARED ${VisionLib_src} ${VisionLib_hdr_moc} ${VisionLib_ui_moc})
#target_link_libraries (VisionLib Qt5::Widgets)


#INCLUDE(${Qt5Widgets_INCLUDE_DIRS})
#INCLUDE( ${Qt5Widgets_LIBRARIES})
 
#QT5_WRAP_UI(UISrcs src/gui/PointList.ui)
#QT5_WRAP_CPP(MOCSrcs include/gui/PointList.h)
# Tell CMake to create the helloworld executable
#//add_executable(helloworld WIN32 main.cpp)

# Use the Widgets module from Qt 5.
#//target_link_libraries(helloworld Qt5::Widgets)


#FIND_PACKAGE(Qt5 REQUIRED)

#find_package(Qt5Core ${QT_VERSION_REQ} REQUIRED)
#find_package(Qt5Quick ${QT_VERSION_REQ} REQUIRED)
#find_package(Qt5Widgets ${QT_VERSION_REQ} REQUIRED)

#INCLUDE(${QT_USE_FILE})
#INCLUDE(${Qt5Widgets_INCLUDE_DIRS})
#INCLUDE( ${Qt5Widgets_LIBRARIES})

 
#QT5_WRAP_UI(UISrcs MyWidget.ui)
#QT5_WRAP_CPP(MOCSrcs MyWidget.h)
 





# Define modo debug.
#set_directory_properties(PROPERTIES COMPILE_DEFINITIONS_DEBUG "_DEBUG")

# Cria uma variável com um valor inicial e também a torna acessível para o
# usuário através da interface gráfica ou curses.
#set(FW_PATH "../fw" CACHE STRING "Path to the FW")

# Procura pelo OpenCV instalado na máquina. Se não existir, vai dar erro.
find_package(OpenCV REQUIRED)

# Aqui verificamos se o OpenCV foi encontrado. Note que estamos testando a
# variável, e não o valor dela.
if (OpenCV_VERSION)

  # Se o OpenCV foi encontrado, informa ao usuário qual a versão presente no
  # sistema.
  message(STATUS "OpenCV version: " ${OpenCV_VERSION_MAJOR} "." ${OpenCV_VERSION_MINOR} "." ${OpenCV_VERSION_PATCH})

  # Se a versão do OpenCV for menor que 3.0, emitimos um fatal error.
  if(${OpenCV_VERSION_MAJOR} LESS 3 OR (${OpenCV_VERSION_MAJOR} EQUAL 2 AND ${OpenCV_VERSION_MINOR} LESS 3))
    message(FATAL_ERROR "You need OpenCV version 3.0 or later!")
  endif()

endif()

# Adicionamos o diretório de include deste projeto.
include_directories(include/advanced)
include_directories(include/basic)
include_directories(include/gui)
# O diretório de include do framework também é adicionado,
#include_directories(${FW_PATH}/include)
# assim como o diretório onde espera-se encontrar a biblioteca.
#link_directories(${FW_PATH}/build)

# Este comando busca todos os arquivos *.cpp dentro do diretório src,
# recursivamente, e armazena a lista na variável lib_sources.
file(GLOB_RECURSE lib_sources src/*.cpp)

# Aqui dizemos ao CMake que queremos construir uma biblioteca chamada ex (que
# será libex.so no Linux e ex.dll no Windows) composta pelos arquivos contidos
# na variável lib_sources.
add_library(code STATIC ${lib_sources})

# Esta biblioteca que estamos criando depende das bibliotecas do OpenCV e do
# framework.
#target_link_libraries(code ${OpenCV_LIBS} fw)
target_link_libraries(code ${OpenCV_LIBS} )
target_link_libraries(code Qt5::Widgets)
target_link_libraries(code Qt5::Core)

# Informamos ao CMake para criar um executável chamado test (test.exe no
# Windows) a partir do arquivo app/test.cpp.
#add_executable(run app/run.cpp)
add_executable(run ${MAIN_FILE} ${VisiontLib_hdr} ${VisionLib_ui})


# Este executável depende da biblioteca ex, também construída por este arquivo.
target_link_libraries(run code ${QT_LIBRARIES})
