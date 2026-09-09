# Archivos de PSeInt y asistencia del editor

Este documento conserva la primera actualización de archivos y asistencia. La ampliación posterior, con el catálogo completo y el APK más reciente, está en [Cobertura del autocompletado](autocompletado-completo.md).

## Archivos `.psc`

La creación solicita `application/octet-stream` y propone un nombre terminado en `.psc`. Antes de escribir, se consulta el nombre real que devuelve el proveedor. Si un documento recién creado tiene otra extensión, se intenta corregirla y se utiliza la URI devuelta por el renombrado. Un fallo conserva el borrador sin marcarlo como guardado. Guardar sobre un documento existente también verifica su nombre antes de escribir.

La causa del sufijo adicional se contrastó con `FileUtils.splitFileName`: Android conserva una extensión desconocida si el MIME solicitado es genérico; con `text/plain` puede anexar `.txt`. [Implementación de Android](https://android.googlesource.com/platform/frameworks/base/+/oreo-release/core/java/android/os/FileUtils.java).

La apertura valida `OpenableColumns.DISPLAY_NAME` antes de leer el contenido. Acepta `.psc` sin distinguir mayúsculas; rechaza `.txt`, `.psc.txt` y nombres desconocidos. El selector del sistema puede mostrar otros archivos: Android filtra por MIME y los proveedores clasifican los `.psc` de diferentes maneras. La validación de la app impide cargarlos como pseudocódigo. La importación de perfiles `.prf` conserva su flujo independiente.

Los archivos `.psc.txt` de versiones anteriores pueden renombrarse a `.psc` en el administrador de archivos. Si el documento ya está recuperado como borrador, Guardar propone un nuevo archivo con la extensión corregida.

La app usa el selector y las URI del [Storage Access Framework de Android](https://developer.android.com/training/data-storage/shared/documents-files), sin solicitar acceso general al almacenamiento.

## Autocompletado y ayudas

Las sugerencias se basan en `SetAutocompletion` y las ayudas en `SetCalltips` de `pseint/wxPSeInt/mxSource.cpp`. La interfaz móvil las presenta junto al cursor, manteniendo el foco del campo de texto.

- `Esc` sugiere `Escribir`; `Definir variable Com` ofrece los cuatro tipos.
- Las frases dependen del contexto: `Entonces` en una condición, `Con Paso` en un Para, `Sin Saltar` en una salida, etc.
- Las opciones del perfil controlan sugerencias de funciones de cadenas, subprogramas, redimensionamiento, sintaxis flexible y variantes de ciclos.
- Se proponen nombres presentes en el documento y funciones numéricas. No se sugieren palabras dentro de comentarios o cadenas ni cuando hay texto seleccionado.
- Un toque acepta una sugerencia. Con teclado físico, flechas recorren la lista, Tab/Enter aceptan y Escape la cierra. Tab conserva la sangría cuando no hay sugerencias; Mayús+Tab quita sangría.
- La aceptación reemplaza la palabra en el cursor, conserva el resto del documento y entra en el historial de deshacer.
- Las ayudas describen los argumentos, por ejemplo las expresiones de `Escribir` o los parámetros de `Subcadena`.

En **Ajustes > Opciones de Editor** están **Utilizar autocompletado** y **Utilizar ayudas emergentes**. Ambos se activan por defecto y se guardan de forma independiente. También se accede desde **Configurar > Apariencia y editor**.

## Verificación del 7 de septiembre de 2026

Pasaron **43 pruebas**, sin fallos, errores ni omisiones:

| Grupo | Pruebas |
| --- | --- |
| Reglas y aceptación del autocompletado | 7 |
| Archivos: extensión, proveedor, renombrado, lectura y escritura | 6 |
| Interfaz de sugerencias, teclado y preferencias | 3 |
| Operaciones del editor | 9 |
| Regresiones del documento y los ejemplos | 7 |
| Interfaz del espacio de trabajo | 8 |
| Configuración inicial | 3 |

Las pruebas del proveedor verifican que el rechazo ocurre antes de leer, que un renombrado utiliza la nueva URI, que un fallo no sobrescribe el contenido y que guardar un texto más corto trunca el anterior. Las pruebas de interfaz comprueban toque, foco, deshacer/rehacer, Tab, flechas, Escape, persistencia de los interruptores y pantalla horizontal.

Registro: `app/build/editor-verification.log`. Resultados conservados: `app/build/editor-related-test-results/`. Las tres pruebas de asistencia se ejecutan también al generar las capturas finales de todas las ventanas, con `captureScreenRoboImage`; el registro de ese paso y del empaquetado es `app/build/editor-package.log`.

Las capturas están en `app/build/outputs/screenshots/`: `autocompletado-escribir.png`, `autocompletado-tipos.png`, `ayuda-escribir.png`, `autocompletado-oscuro.png` y `ajustes-autocompletado.png`. La interfaz se verificó con Robolectric; falta la comprobación en un teléfono y sus proveedores de archivos concretos.

## APK verificado

La compilación de desarrollo y Android Lint finalizaron correctamente: **0 errores y 78 advertencias**. La firma v2 se verificó con `apksigner` y la alineación con `zipalign -c -P 16 4`.

- Ruta: `app/build/outputs/apk/debug/app-debug.apk`.
- Tamaño: 27 092 157 bytes.
- SHA-256: `94A47E260CCF6649B58F3D3CC8B3836278FECF53A4AE9911F4AC4823280A165F`.
