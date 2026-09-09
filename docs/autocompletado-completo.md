# Cobertura del autocompletado

El catálogo móvil incluye todas las entradas del autocompletado de las fuentes de PSeInt 20250225 incluidas en este proyecto. También reconoce las variantes de palabras clave del motor, sus funciones predefinidas y sus condiciones coloquiales.

| Fuente comprobada directamente por las pruebas | Entradas |
| --- | --- |
| `wxPSeInt/mxSource.cpp`, `SetAutocompletion` | 105 combinaciones de sugerencia y contexto |
| `pseint/Keywords.cpp`, `initKeywords` | 104 palabras clave y variantes |
| `pseint/FuncsPredefs.cpp`, `LoadPredefs` | 29 nombres de funciones, incluidas sus variantes |
| `pseint/SynCheck.cpp`, `GetColoquialConditions` | 30 formas de condiciones |
| `wxPSeInt/mxSource.cpp`, `SetCalltips` | 50 entradas de ayudas de instrucciones y funciones |

Los grupos se solapan: estos números no representan comandos distintos que deban sumarse. Las pruebas extraen las entradas de los archivos C++ originales, comprueban su presencia y prueban prefijos en los contextos correspondientes. Los listados comprobados quedan en `app/build/compatibility/completion/`.

## Comportamiento

- `FechaActual` y `HoraActual` insertan sus paréntesis vacíos y muestran ayuda. Si los paréntesis ya existen, se conservan sin duplicarlos.
- Se completan las condiciones `Es Par`, `Es Divisible Por`, `Es Mayor O Igual A`, etc., y las declaraciones alternativas `n Es Real` y `a, b Son Enteros` cuando el perfil las permite.
- Se reconocen alias como `Por Copia`, `HastaQue`, `Sin Bajar` y variantes con acentos. Los alias equivalentes se presentan mediante una sugerencia canónica; por ejemplo, `Como Numér…` ofrece `Como Real` y `Por Cop…` ofrece `Por Valor`.
- Se conservan las cuatro opciones principales de tipos, sin llenar la lista de sinónimos equivalentes.
- La lista permite recorrer todos los resultados; se eliminó el recorte anterior de 12 sugerencias. También se pueden completar instrucciones cortas como `Si` y `RC`.
- Los nombres de variables mantienen sus acentos; la equivalencia de acentos se aplica al vocabulario del lenguaje.
- Las funciones, subprogramas, formas coloquiales, sintaxis flexible y variantes de ciclos se filtran por el perfil activo. Las palabras deshabilitadas no reaparecen como supuestos nombres de variables.
- El resaltado reconoce las nuevas funciones. Las ayudas contextuales incluyen los pasos intermedios de Para, Hasta Que y las funciones predefinidas.

Los interruptores **Utilizar autocompletado** y **Utilizar ayudas emergentes**, en **Ajustes > Opciones de Editor**, siguen activados por defecto y conservan su configuración independiente.

## Pruebas

`CompletionCatalogCoverageTest` compara el catálogo con las fuentes originales. `EditorCompletionTest` verifica inserción, alias, perfiles, paréntesis, ayudas y programas completados que se comprueban con el motor nativo. `EditorAssistanceScreenshotTest` verifica selección por toque/teclado, desplazamiento de listas, foco, persistencia y las capturas de interfaz.

La verificación completa del 8 de septiembre de 2026 terminó con **776 pruebas aprobadas**, sin fallos, y `lintDebug` sin errores (78 advertencias de dependencias, versiones y metadatos). El APK de depuración generado es `app/build/outputs/apk/debug/app-debug.apk` (27,092,157 bytes; SHA-256 `1CFF0130B304A6D803B8EA59DFCCCB16484CCD2E0552F101AF6BF8859E5D8104`). Contiene el motor nativo para `arm64-v8a`, `armeabi-v7a` y `x86_64`.

La cobertura corresponde al catálogo incluido en el repositorio. La presentación y la interacción están adaptadas a Android; no se pretende reproducir todos los detalles internos del editor de escritorio.
