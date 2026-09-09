# Motor de PSeInt en Android

El APK compila las fuentes de `pseint/pseint` (versión 20250225) para arm64-v8a, armeabi-v7a y x86_64. `PSeIntEvaluator` es el puente Kotlin/JNI; no contiene una segunda implementación de las expresiones ni del intérprete. La biblioteca se usa para validar y ejecutar.

## Transporte y estado

`mobile_runtime.cpp` adapta salida, entrada, espera y depuración a callbacks de Compose. Un bloqueo protege el estado global del motor. Tanto una ejecución como una validación que espera el bloqueo pueden cancelarse. La salida aplica contrapresión antes de llenar la memoria; la consola conserva un historial limitado.

Los errores de ejecución se convierten en diagnósticos y excepciones C++ para regresar a Kotlin, evitando el `exit()` que utiliza la consola de escritorio. Los callbacks de entrada y depuración propagan la cancelación. Las redirecciones de salida y los punteros globales se restablecen al terminar, incluso ante errores.

La interfaz trabaja con Unicode. El puente usa Windows-1252, igual que las fuentes suministradas, sin conversiones con pérdida. La generación de fuentes convierte los bytes de los literales C++ en escapes octales para conservarlos con Clang en todas las arquitecturas.

## Perfiles

`ProfileRules.nativeFlags()` sigue el orden de las 24 opciones de `LangSettings.h`. La importación reproduce los valores predeterminados, las claves originales y la migración por versión de `LangSettings::Fix`. El exportador escribe las claves del escritorio y agrega `editor_font_size`, que el escritorio puede ignorar.

Las preferencias Algoritmo/Proceso y Función/SubProceso afectan las plantillas. Ambas variantes continúan siendo válidas. El perfil decide el cierre de Repetir, los puntos y coma de las plantillas, la disponibilidad de funciones, Para Cada y Redimensión, el resaltado, las formas de entrada/salida y el diagrama inicial. El documento existente se conserva al cambiar de perfil. Las antiguas propiedades móviles `strictTypes` y `allowImplicitVariables` se mantienen por compatibilidad de importación; se normalizan con `forceDefineVariables` y no aparecen como opciones independientes sin equivalente en el motor.

## Correcciones sobre la copia móvil del motor

Las fuentes originales no se sobrescriben. La tarea de generación comprueba los puntos de inserción y falla si cambian inesperadamente.

- La reducción de arreglos original continúa la recursión después de la última dimensión y lee fuera del arreglo de dimensiones. La copia móvil termina la recursión, usa `delete[]` y respeta la base de los índices. Como en el escritorio, redimensionar un argumento de un subproceso se rechaza con el error 223.
- La comprobación original de índices fraccionarios compara dos conversiones a entero. La adaptación compara el valor real con el índice entero y devuelve el error 301 cuando corresponde.
- El código original no consulta las opciones para prohibir Para Cada, Repetir Mientras Que y Redimensión al reconocer estas instrucciones. La copia móvil las aplica y devuelve los códigos 1001, 1002 y 1003, respectivamente. Son correcciones deliberadas para que las opciones tengan efecto.
- La ejecución móvil limita a 128 los bloques o llamadas anidados para devolver un error 1004 ante una recursión descontrolada, evitando agotar la pila nativa del teléfono.

## Referencias de prueba

`tools/build-native-tests.ps1` construye una biblioteca JNI para la JVM y `pseint-desktop`, una compilación independiente del intérprete original. Su pequeño controlador únicamente agrega una consulta para imprimir las opciones de un perfil; la ejecución normal delega al `main` original.

- `DesktopCompatibilityTest`: 192 casos comparan exactamente salida y códigos de diagnóstico con el ejecutable independiente. `redimension-01` y `redimension-03` se comprueban contra los archivos `.out` originales, porque la implementación original tiene comportamiento indefinido al reducir arreglos. `redimension-05` exige la salida original más el diagnóstico 1003 de la instrucción que su perfil prohíbe y que el escritorio no comprobaba. Se prueban los 195 casos; estas tres diferencias se verifican explícitamente.
- `DesktopProfilesTest`: compara las 24 opciones de cada archivo de perfil con `LangSettings::Load` del escritorio.
- `PSeIntEvaluatorTest` y `ProfileRulesTest`: regresiones, restricciones, entrada, funciones, referencias, cancelación e importación/exportación.
- `WorkspaceScreenshotTest`: prueba la consola, el avance manual, la recuperación y la aplicación visual del perfil con renderizado nativo de Robolectric.

Los resultados certifican esos casos y esas fuentes. No implican que todo programa posible esté libre de errores, ni que se haya probado el APK en un dispositivo físico. `testMode` es interno a las pruebas y usa las convenciones `--fortest` del escritorio para esperas y limpieza de pantalla; las ejecuciones de la app usan siempre el modo interactivo normal.
