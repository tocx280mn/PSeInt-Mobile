# Verificación del motor y los perfiles

Resultado del 6 de septiembre de 2026 para el APK de desarrollo generado en este workspace. Motor original incluido: PSeInt 20250225. Este informe conserva la verificación anterior a los cambios de configuración inicial e icono; el APK de la ruta indicada fue reemplazado por la [compilación actualizada](verificacion-inicio.md).

| Comprobación | Resultado |
| --- | --- |
| Pruebas JVM y Robolectric con JNI real | 743 aprobadas; 0 fallos, 0 errores y 0 omitidas |
| Programas de `pseint/test/interp` | 195 comprobados |
| Perfiles originales | 466 archivos, comparando sus 24 opciones con el cargador de escritorio |
| Android Lint | 0 errores; 81 advertencias pendientes |
| Compilación de desarrollo | Correcta para arm64-v8a, armeabi-v7a y x86_64 |
| Firma del APK | Verificada con `apksigner`, esquema v2 |
| Alineación del APK | `zipalign -c -P 16 4`: correcta |
| Bibliotecas de 64 bits | Segmentos ELF LOAD alineados a 16 KiB, incluido el motor |
| Dispositivo físico | No comprobado; `adb devices` no muestra dispositivos conectados |

Las pruebas de compatibilidad incluyen 192 comparaciones exactas contra una compilación independiente del escritorio, dos comparaciones con los `.out` originales para evitar el fallo de memoria del redimensionamiento original y un caso que exige el diagnóstico adicional por redimensionar cuando el perfil lo prohíbe. Las diferencias se describen en [la adaptación del motor](motor-nativo.md).

También se probaron funciones recursivas, recuperación tras superar el límite de anidamiento, arreglos en base cero, protección del contador de Para, variables sin inicializar, entrada interactiva, cancelación, avance manual, importación/exportación, persistencia del perfil y cambios visuales del editor y los diagramas. Las capturas de la interfaz se generaron con Robolectric y se revisaron visualmente.

## Reproducción

Desde la raíz del proyecto, con el compilador C++17 de 64 bits y las dependencias indicadas en el README:

```powershell
$env:JAVA_HOME = 'C:/Program Files/Android/Android Studio/jbr'
./tools/build-native-tests.ps1
./gradlew.bat :app:testDebugUnitTest :app:lintDebug :app:assembleDebug '-Proborazzi.test.record=true' --console=plain
```

Los informes completos están en `app/build/reports/tests/testDebugUnitTest/` y `app/build/reports/lint-results-debug.html`; las salidas de compatibilidad, en `app/build/compatibility/`. La compilación final quedó registrada en `app/build/final-verification.log`.

## APK comprobado

- Ruta: `app/build/outputs/apk/debug/app-debug.apk`.
- Tamaño: 27 092 826 bytes.
- SHA-256: `E74FBA7609242351317742287BA997A0A176FEC51203099B224B2D2A2975A6A4`.

Estos resultados corresponden a los casos probados y a este APK. No demuestran ausencia de errores para todo pseudocódigo posible ni sustituyen la prueba en un teléfono.
