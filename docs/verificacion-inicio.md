# Verificación de configuración inicial e icono

Pruebas realizadas el 6 de septiembre de 2026. Esta actualización restaura el asistente de primer inicio y utiliza el icono original incluido en `pseint/bin/imgs/icon128.png`, sin modificar la imagen.

Este informe conserva los resultados de esa actualización. El APK de la ruta indicada fue reemplazado el 7 de septiembre por la [compilación con archivos `.psc` y autocompletado](archivos-y-autocompletado.md).

El asistente permite elegir perfil y apariencia antes de entrar al editor. Guarda su finalización, conserva los ajustes al recrear la interfaz y vuelve a aparecer si quedó pendiente. Puede reabrirse desde Ajustes sin borrar el documento. También se corrigió la medición del diálogo de perfiles para que funcione desde el asistente.

| Comprobación | Resultado |
| --- | --- |
| Pruebas de configuración inicial | 3 aprobadas |
| Pruebas de interfaz del editor | 8 aprobadas |
| Pruebas básicas con Robolectric | 2 aprobadas |
| Total de esta verificación | 13 aprobadas; 0 fallos, errores u omisiones |
| Android Lint | 0 errores; 78 advertencias |
| Compilación de desarrollo | Correcta |
| Firma del APK | Verificada con `apksigner`, esquema v2 |
| Alineación del APK | `zipalign -c -P 16 4`: correcta |
| Recursos del lanzador | Iconos clásico y adaptativo; eliminadas las imágenes predeterminadas |

Las capturas de configuración en vertical y horizontal, y del icono del lanzador, se revisaron visualmente. Las pruebas de interfaz usan Robolectric; no se verificó esta compilación en un teléfono. El informe de las 743 pruebas anteriores del motor se conserva por separado en [verificacion-motor.md](verificacion-motor.md).

## Reproducción

Con el JNI de pruebas preparado según el README:

```powershell
$env:JAVA_HOME = 'C:/Program Files/Android/Android Studio/jbr'
./gradlew.bat :app:testDebugUnitTest --tests 'com.example.OnboardingScreenshotTest' --tests 'com.example.WorkspaceScreenshotTest' --tests 'com.example.ExampleRobolectricTest' '-Proborazzi.test.record=true' --console=plain
./gradlew.bat :app:lintDebug :app:assembleDebug --console=plain
```

Registros: `app/build/onboarding-verification.log` y `app/build/onboarding-package.log`. Capturas: `app/build/outputs/screenshots/configuracion-inicial.png`, `configuracion-horizontal.png` e `icono-lanzador.png`.

## APK actualizado

- Ruta: `app/build/outputs/apk/debug/app-debug.apk`.
- Tamaño: 27 092 157 bytes.
- SHA-256: `1CB1FA2B0ACD47A21B1DD13BCAD3B0E983FC2FC065B70BBD0DE46BE51F2787F2`.
