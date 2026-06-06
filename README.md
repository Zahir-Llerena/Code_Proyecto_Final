# Alien Invasion: Motor de Juego en C++ 🚀

Proyecto final desarrollado para el curso **Informática II** de la Facultad de Ingeniería en Telecomunicaciones de la **Universidad de Antioquia**.

## 📝 Descripción
Este proyecto consiste en la implementación de un motor de juego bidimensional con vista cenital, basado en el paradigma de **Programación Orientada a Objetos (POO)** en C++. El sistema integra un motor de gestión de entidades múltiples (naves, enemigos, proyectiles) y un sistema de control en tiempo real, garantizando la eficiencia en el procesamiento de eventos y lógica de colisiones.

## 👥 Integrante
* **Arlington Zahir Llerena** - `zahir.llerena@udea.edu.co`

## 🏗️ Arquitectura del Sistema
# Alien Invasion: Motor de Juego en C++

## 1. Descripción del Proyecto
Este proyecto consiste en el diseño y construcción de un motor de juego bidimensional (con vista cenital) desarrollado en **C++**. El sistema implementa el paradigma de **Programación Orientada a Objetos (POO)** e integra un sistema de control en tiempo real para el usuario, junto con una arquitectura de gestión de entidades múltiples (naves, obstáculos espaciales, proyectiles).

El objetivo principal es garantizar la eficiencia en el procesamiento de eventos, la actualización de estados cinemáticos y la resolución de colisiones mediante lógica booleana, como parte del proyecto final del curso de **Informática II** de la **Universidad de Antioquia**.

## 2. Arquitectura y Módulos
El proyecto está estructurado de forma modular para garantizar la escalabilidad y el mantenimiento del código:

| Módulo / Clase | Responsabilidad Técnica |
| :--- | :--- |
| **AlienInvasion** | Clase controladora principal y gestor del ciclo de vida del juego. |
| **Settings** | Repositorio centralizado de constantes (velocidades, dimensiones). |
| **Ship** | Gestión del jugador, movimiento y disparo. |
| **Alien** | Lógica de enemigos, actualización de posición y detección de bordes. |
| **Bullet** | Comportamiento de proyectiles y limpieza de memoria. |
| **GameStats** | Gestión de estados: puntuación, nivel y vidas. |
| **Scoreboard** | Interfaz para la representación visual de estadísticas. |

## 3. Estructura del Repositorio
* `main.cpp`: Punto de entrada del programa.
* `Code_D2/`: Directorio que contiene la implementación de los módulos (`.cpp` y `.h`).
* `Practica_3_Lab.xcodeproj/`: Configuración del proyecto para entorno Xcode.
* `.gitignore`: Configuración para excluir archivos temporales de compilación y binarios.

## 4. Instrucciones de Ejecución

### Requisitos
* Compilador compatible con **C++11** o superior.
* Sistema: macOS (Xcode) o entorno compatible con C++.

### Compilación desde terminal
Para compilar y ejecutar el proyecto manualmente:

```bash
g++ -o alien_invasion Code_D2/main.cpp
./alien_invasion
