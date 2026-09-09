# Recursos de PSeInt

El motor C++ de ejecución, los iconos de la barra de herramientas y los comandos, así como los perfiles institucionales incluidos en esta app, proceden de PSeInt de Pablo Novara, cuyas fuentes se encuentran en la carpeta `pseint/` de este proyecto. El motor incluido corresponde a la versión 20250225.

El icono del lanzador y de la bienvenida es una copia sin modificaciones de `pseint/bin/imgs/icon128.png`. Los recursos XML de Android lo encuadran para las formas de icono clásicas y adaptativas.

La adaptación JNI y las correcciones aplicadas a las copias de compilación se describen en `docs/motor-nativo.md`. La carpeta de fuentes originales se conserva y las copias se generan de forma reproducible mediante `PreparePseintCore` en `app/build.gradle.kts`.

Las formas y la paleta de los diagramas se basan en los archivos `psdraw3/DClasico.cpp`, `psdraw3/DNassiSchne.cpp` y `psdraw3/Global.cpp` de esas fuentes. Los colores del editor se contrastaron con `wxPSeInt/mxSource.cpp`.

Las reglas de autocompletado y las ayudas de argumentos se adaptaron de `SetAutocompletion` y `SetCalltips` en `pseint/wxPSeInt/mxSource.cpp`, con las variantes de `pseint/pseint/Keywords.cpp`, las funciones de `FuncsPredefs.cpp` y las condiciones de `SynCheck.cpp`; su presentación móvil está implementada en Kotlin y Compose.

El archivo original `pseint/license.txt` identifica la licencia del proyecto como GNU GPL versión 2 y distingue las bibliotecas de terceros. El texto completo se conserva en `pseint/dist/license.txt` y se incluye con la aplicación en `app/src/main/assets/pseint-gpl-v2.txt`.

Esta adaptación móvil es un proyecto independiente; los créditos al autor original no implican que sea una versión oficial.
