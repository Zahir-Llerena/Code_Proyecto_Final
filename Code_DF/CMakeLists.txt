cmake_minimum_required(VERSION 3.16)

project(Code_DF LANGUAGES CXX)

# ---- 1. ESTÁNDARES DE C++ ----
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- 2. AUTOMATIZACIÓN DE QT ----
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---- 3. ENLACE ESTRICTAMENTE GRÁFICO ----
# Se omiten módulos multimedia y de red para garantizar compilación instantánea
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# ---- 4. CONSTRUCCIÓN DEL BINARIO NATIVO M4 ----
add_executable(Code_DF MACOSX_BUNDLE main.cpp)
target_link_libraries(Code_DF PRIVATE Qt6::Widgets)

# ---- 5. DESPLIEGUE ----
include(GNUInstallDirs)
install(TARGETS Code_DF
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)