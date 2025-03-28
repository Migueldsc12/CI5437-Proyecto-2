# Requisitos

Esta aplicación utiliza SFML como la biblioteca principal para gráficos.
Se recomienda ejecutar los siguientes comandos:

**1. Actualizar Controladores Gráficos y la Dependencia MESA** 

    
    sudo apt update && sudo apt upgrade && sudo apt install mesa-utils
    

**2. Instalar Dependencias de SFML**

    sudo apt install libsfml-dev


**3. Limpiar Archivos Residuales**

    sudo apt autoremove
    

# Instrucciones para Compilar y Ejecutar

1. Compilación:
   - Abra una terminal en la carpeta donde se encuentra el archivo `makefile`.
   - Ejecute el siguiente comando para compilar el programa:
     
     ```
     make
     ```
   - Esto generará un ejecutable llamado `agenteConnect6`.

2. Ejecución:
   - Ejecute el programa con el siguiente comando:
     
     ```
     ./agenteConnect6 -fichas=<blancas|negras> -tpj=[segundos]
     ```
    
    Donde el valor para `fichas` representa el color de las fichas con las que
    jugará el agente y `tpj` es el tiempo máximo, en segundos, que el agente 
    puede usar para computar su jugada.
    - Se abrirá una ventana de juego con el tablero. Haga click sobre las 
    posiciones donde desee jugar. Una vez finalizado el juego, éste se cerrará 
    automáticamente. 


# Autores

- Laura Parilli, 17-10778
- Miguel Salomon, 19-10274
    
