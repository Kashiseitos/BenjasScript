# BenjasScript

BenjasScript es un interprete experimental para controlar un robot Unitree Go2 mediante un lenguaje sencillo de instrucciones. El programa transforma un archivo de codigo fuente en bytecode y despues ejecuta ese bytecode en una maquina virtual conectada a la red del robot.

## Flujo de ejecucion

El ejecutable realiza estas etapas:

1. Lee `SourceCode.txt` desde el directorio de trabajo actual.
2. El scanner reconoce los tokens mediante un automata finito determinista.
3. El parser descendente recursivo valida la sintaxis y genera bytecode.
4. Escribe el bytecode binario en `../../Object.bin` y una version legible en `../../Object.txt`, usando rutas relativas al directorio de trabajo.
5. Espera que se pulse `ENTER`.
6. La maquina virtual lee `../../Object.bin`, inicializa la comunicacion Unitree y ejecuta las instrucciones.

El archivo `Object.txt` sirve para inspeccionar los opcodes generados. `Object.bin` es el archivo que consume la maquina virtual; `Object.txt` no se utiliza durante la ejecucion.

## Estructura del proyecto

```text
.
├── CMakeLists.txt              Configuracion de compilacion
├── Headers/BenjasSript.h       Tokens, opcodes y declaraciones compartidas
├── Sources/main.c              Punto de entrada del ejecutable
├── Sources/scanner.c           Analizador lexico basado en DFA
├── Sources/parserRDCP.c        Parser y generador de bytecode
├── Sources/VM.cpp              Maquina virtual e integracion con Unitree Go2
├── SourceCode.txt              Programa BenjasScript de ejemplo
├── Object.txt                  Bytecode en formato de texto
├── Object.bin                  Bytecode binario
└── external/unitree_sdk2/      SDK de Unitree y dependencias DDS
```

## Requisitos

- CMake 3.10 o posterior.
- Compilador C compatible con C99.
- Compilador C++ compatible con C++17.
- SDK `unitree_sdk2` y sus bibliotecas incluidas en `external/unitree_sdk2`.
- Entorno Linux para la integracion actual: el proyecto enlaza `libunitree_sdk2.a`, `libddscxx.so`, `libddsc.so` y `pthread`.
- Un Unitree Go2 accesible por la interfaz de red indicada al ejecutar el programa.

La configuracion de CMake selecciona automaticamente las bibliotecas `aarch64` o `x86_64` segun la arquitectura del procesador.

## Compilar

Desde la raiz del repositorio:

```bash
cmake -S . -B build
cmake --build build
```

El ejecutable se genera como `build/BenjasScript`. Si ya existe un directorio `build`, se puede volver a compilar con:

```bash
cmake --build build
```

## Ejecutar

El programa exige el nombre de una interfaz de red como primer argumento. Debido a las rutas relativas usadas actualmente, ejecutalo desde `build`:

```bash
cd build
./BenjasScript eth0
```

Sustituye `eth0` por la interfaz conectada al robot. Para una prueba de comunicacion local, el codigo tambien contempla `lo`, aunque las lecturas y acciones del robot requieren el entorno Unitree correspondiente.

Antes de ejecutar:

- Revisa que `SourceCode.txt` este en el directorio de trabajo. CMake copia una version en `build/` durante la configuracion.
- Verifica que el SDK y las bibliotecas DDS correspondan a la arquitectura del sistema.
- Comprueba que el robot este accesible y que la interfaz de red sea la correcta.
- Ten presente que el programa espera una confirmacion con `ENTER` despues del analisis sintactico.

## Lenguaje BenjasScript

Un programa comienza con `main` y su cuerpo se delimita con llaves. Las instrucciones terminan en punto y coma.

### Instrucciones de movimiento

```text
Stand;
Sit;
Walk expresion;
Climb expresion;
Rotate expresion;
```

`Walk`, `Climb` y `Rotate` consumen el resultado numerico de una expresion. `Climb` es experimental en la maquina virtual y `Walk`/`Rotate` actualmente muestran la accion y esperan un tiempo calculado; algunas llamadas de movimiento del SDK permanecen desactivadas en `VM.cpp`.

### Variables y expresiones

Las variables se crean al usarse por primera vez y tienen un limite de 100 simbolos. Se admiten numeros enteros y decimales, asignaciones, parentesis y estos operadores:

```text
resultado = (10 + 2.5) * 3;
```

```text
+  suma
-  resta
*   multiplicacion
/   division
%   residuo
```

### Sensores

```text
distancia = GetDist();
bateria = GetBattery();
```

`GetDist()` devuelve la distancia del sector frontal y `GetBattery()` devuelve el porcentaje de bateria.

### Condiciones y control de flujo

Las comparaciones disponibles son `>`, `>=`, `<`, `<=`, `==` y `!=`. Las condiciones se pueden combinar con `&&` y `||`.

```text
if (bateria > 20) {
	Walk 1;
} else {
	Sit;
}

while (distancia > 1) {
	Walk 0.5;
	distancia = GetDist();
}
```

## Ejemplo incluido

El `SourceCode.txt` incluido hace que el robot se ponga de pie, inicializa `repetir` en `3` y repite tres veces una caminata usando la lectura de `GetDist()`:

```text
main
{
	Stand;
	repetir = 3;
	while((repetir > 0))
	{
		Walk GetDist();
		repetir = repetir - 1;
	}
}
```

## Limitaciones conocidas

- El archivo fuente y los artefactos de bytecode tienen rutas fijadas en el codigo; cambiar el directorio de ejecucion puede provocar que no se encuentren.
- El argumento de red se valida solo por cantidad, no por existencia de la interfaz.
- No hay una suite de pruebas automatizadas configurada en el repositorio.
- La tabla de simbolos, la pila de la VM y algunos buffers tienen tamanos fijos.
- Los errores lexicos se imprimen, pero el flujo de compilacion no los propaga como un codigo de salida especifico.
- El parser genera bytecode incluso cuando el manejo de errores requiere mejoras; revisa la salida antes de conectar un robot real.

## Licencia y dependencias

El repositorio incluye `external/unitree_sdk2`, que contiene su propio archivo de licencia. Consulta la documentacion y las condiciones de licencia del SDK antes de redistribuir el proyecto.

