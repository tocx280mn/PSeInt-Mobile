## Código Fuente

Para generar el paquete de código fuente para una release sola hay que ejecutar `./pack.sh src`, y el resultado estará en el directorio `dist`.

### Acciones previas

Si es para una release, antes de generar el paquete de código fuente y los diferentes binarios para distribuir, se debe actualizar la documentación (en particular revisar el changelog en `bin/help/cambios.php`), la splash screen (cambiar la versión en `imgs/splash.svg` y regenerarlos pngs con `imgs/splash.sh`), y el achivo `bin/version` (que es donde los scripts toman el número de versión).


## Microsoft Windows

El proceso se hace un GNU/Linux ejecutando el compilador para windows (mingw32 o mingw64) mediante wine (actualmente uso Fedora40 con el wine de sus repositorios)

### Preparación del compilador en wine

Utilizo los compiladores que empaqueto para ZinjaI, así que la forma más simple es descargarlos del sitio de ZinjaI. Se requiere el compilador (al instalar ZinjaI, estará en una carpeta `mingwXX-gccYY` donde `XX` será `32` o `64` según los bits de la versión, e `YY` será la versión de GCC en que se basa). Este compilador es en realidad una de las releases binarias para Windows que se pueden descargar del proyecto mingw64. En la instalación  de wine, el compilador debe ir al raiz en una carpeta `MinGW32` o `MinGW64,` y la ruta a `bin` dentro de esa carpeta debe agregarse a la variable de entorno PATH (de wine), para evitar tener que modificar los configs de los makefiles.

Además del compilador, se requiere la biblioteca wxWidgets. Lo más simple vuelve a ser tomar el complemento para ZinjaI disponible en su página. Esto genera una carpeta wx3 dentro de la de mingw, mantener esa estructura en wine. Esa versión fue compilada usando el `makefile.gcc` que wx trae en la carpeta `buils/msw`, con las opciones `BUILD=release SHARED=1'

Para separar ambas versiones, 32 y 64bits, uso prefixes diferentes para wine. Se configuran con variables de entorno:

* 32bits: `WINEPREFIX=$(pwd)/wine32` y `WINEARCH=win32`
  * el compilador irá en `wine32/drive_c/MinGW32`
* 64bits: `WINEPREFIX=$(pwd)/wine64` y `WINEARCH=win64`
  * el compilador irá en `wine64/drive_c/MinGW64`

En ambos casos además defino `WINEDEBUG=-all` para reducir warnings de wine.

Para definir las variables de entorno en wine, creo un archivo con info para el registro, `user.reg`

```REGEDIT4 
REGEDIT4
[HKEY_CURRENT_USER\Environment] 
"PATH"="c:\\mingw32\\bin;%PATH%" 
```

y lo ejecuto con `regedit /C user.reg` (`regedit` es uno de los ejecutables de wine).

### Software para generar el instalador

Actualmente utilizo Inno Setup 6.2.2. Ejecuto el instalador con wine (una vez con cada prefix) y le digo que lo instale en `C:\inno`. Con eso los scripts de zinjai podrán generar el instalador sin tener que modificar nada más. Alternativamente están comentadas (en `pack.sh`) los comandos para usar nsis en lugar de inno (anteriormente usaba nsis, pero en algún momento por los problemas con antiviru descubrí que con inno generaba menos  y falsos positivos).

### Acciones en cada Release

El script `pack.sh` compila todo usando wine+mingw y genera también el exe con inno y un zip "portable". Se ejecuta con `./pack.sh w32` y `./pack.sh w64` (en cada caso definiendo previamente `WINEPREFIX` y `WINEARCH`) . Los archivos resultantes estarán en el directorio `dist`.

## GNU/Linux

El proceso se hace en una máquina virtual (utilizo VirtualBox).

Uso distribuciones de GNU/Linux populares y viejas, para evitar que los binarios se compilen con dependencias a versiones muy recientes de bibliotecas del sistema y eso les impida correr en máquinas desactualizadas. Actualmente:

* 64bits:  *Mint 19-xfce*
* 32bits: *Mint 19-xfce*

Para instalar las dependencias necesarias:

* compilador c++: `sudo apt-get install g++`
* para  wx: `sudo apt-get install libgtk-3-dev`
* para opengl: `sudo apt-get install libgl1-mesa-dev` y `sudo apt-get install libglu1-mesa-dev` (o instalar `freeglut3-dev` que tiene a ambos por dependencia)

### Compilación de wxWidgets

1. Crear un directorio `wx32` en el home
2. Extraer los fuentes ahí dentro (quedarán en `wx32/wxWidgtes-3.2.1.1`)
3. Crear un directorio `build` también en wx32, y desde ese directorio hacer:
   1. `./wxWidgets-3.2.2.1/configure --disable-debug --with-opengl --with-libpng=builtin --with-libjpeg=builtin --enable-shared --with-gtk=3 --prefix=/home/usuario/wx32/dist --enable-stc`
   2. `make && make install`
4. Crear un directorio en el home (cualquier nombre, voy a usar `libs-pseint`) para poner ahí los binarios que van a requerir los ejecutables de pseint. Actualmente son:
   * `libwx_base-3.2.so.0`
   * `libwx_base-net-3.2.so.0`
   * `libwx_gtk3u_aui-3.2.so.0`
   * `libwx_gtk3u_core-3.2.so.0`
   * `libwx_gtk3u_gl-3.2.so.0`
   * `libwx_gtk3u_html-3.2.so.0`
   * `libwx_gtk3u_stc-3.2.so.0`

### Script auxiliar `pseint-packer.sh`

Para agilizar el proceso de ida y vuelta de archivos entre le host y la máquina virtual, hay un script `pseint-packer.sh` en `dist`. En una terminal de la máquina virtual:

* Para evitar que cada paso pida las claves: `ssh-keygen; ssh-copy-id zaskar@10.0.2.4`
* Copiar el script a la virtual: `scp zaskar@10.0.2.4:/mnt/ex/pseint/dist/pseint-packer.sh .`
* y darle permisos de ejecucion: `chmod a+x pseint-packer.sh`
* Correr el script sin parametros para que genere el archivo de configuración: `./pseint-packer.sh`
* Editar el archivo de configuración `pseint-packer.cfg`
  * `SSHRC`: poner la ruta para *ssh* (`usuario@ip:/path`) a los fuentes de pseint en el host
  * `ARCH`: poner `l32` o `l64` segun corresponda
  * `LIBSDIR`: poner algun nombre de directorio (`pseint-libs`) a crear donde iran luega algunos binarios de wx
  * `PATH`: descomentar y ajustar la ruta al directorio `bin` de la instalación de wx (`/home/usuario/wx32/dist/bin`)

### Acciones en cada release

1. Antes de comenzar, en el host hay que generar el tgz con los fuentes. Para ello, correr `./pack.sh src` en el directorio de fuentes de pseint.
2. En la máquina virtual, ejecutar `./pseint-packer.sh 20230314` (se le pasa el nro de versión como argumento, el que tenga en el nombre el archivo del paso 1)
   * El archivo quedará en el directorio `pseint/dist` del host

## MacOS

En estos casos, genero un compilador cruzado (que compila para macOS desde GNU/Linux) con ayuda de osxcross, y luego hago todo el proceso desde GNU/Linux.

### Compilador

1. Descargar xcode 

   * https://developer.apple.com/download/more 
   * usé la versión 12.5

2. Clonar osxcross 

   * https://github.com/tpoechtrager/osxcross

3. Empaquetar el sdk

   * en osxcross, ejecutar `./tools/gen_sdk_package_pbxz.sh <xcode descargado en 1>`
     * esto tarda mucho y usa mucho espacio en disco
   * supuestamente este paso requiere: clang`, `make`, `libssl-devel`, `lzma-devel y libxml2-devel
   * copiar el resultado (que va a estar en el directorio de osxcross) en el directorio `tarballs`

4. Compilar:

   * Ejecutar `TARGET_DIR=/opt/mac-arm64/clang/ UNATTENDED=1 ./build.sh`

5. Agregar `/opt/mac-arm64/clang/bin` al PATH para usar

   * `export PATH=/opt/mac-arm64/clang/bin:$PATH`

6. Ejecutar `./build_compiler_rt.sh` (solo para x86_64, no es necesario para arm64)

   * Sin esto al compilar wx  vamos a tener un error `Undefined symbols for architecture x86_64: ___isOSVersionAtLeast`

   * Al finalizar hay que copiar archivos a pata al clang del sistema:

     1. `sudo mkdir -p /usr/lib64/clang/11.0.0/lib/darwin`
     2. `sudo cp -v /opt/mac-arm64/osxcross/build/compiler-rt/compiler-rt/build/lib/darwin/*.a /usr/lib64/clang/11.0.0/lib/darwin`
     3. `sudo cp -v /opt/mac-arm64/osxcross/build/compiler-rt/compiler-rt/build/lib/darwin/*.dylib /usr/lib64/clang/11.0.0/lib/darwin`

     * Nota: En los comandos anteriores hay que reemplazar `11.0.0` por la versión del clang nativo del linux sobre el cual se corra esto.


Dado que los nombres de los ejecutables del compilador están precedidos por la arquitectura y la versión del sdk (ej: `arm64-apple-darwin20.4-g++`), puede haber que ajustar los configs de los makefiles y del script `dist/fix-dylibs.sh`.

### wxWidgets

0. en el CMakeLists.txt principal de wx,  buscar donde define los lenguajes (en 2.2.2.1 linea 92) y agregar OBJC y OBJCXX (tal vez solo el primero?): `set(wxLANGUAGES C CXX OBJC OBJCXX)`
   * sino da un error raro sobre objective-C y las PCHs

1. extraer wx, crear una carpeta build-arm al lado de la de los fuentes y compilar con cmake:

   * `arm64-apple-darwin20.4-cmake ../wxWidgets-3.1.4/ -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX=/opt/mac-arm64/wx/dist-arm/`
   * `make -j 4 && make install`

2. Corregir nombres de dylib, porque los generados no coinciden con lo que dice `wx-config`:

   * En `/opt/mac-arm64/wx/dist-arm/lib`, ejecutar:
     * `for a in *.dylib; do mv "$a" "${a%.dylib}-Darwin.dylib"; done`

3. Agregar `/opt/mac-arm64/wx/dist-arm/bin` al `PATH` para usar
   * `export PATH=/opt/mac-arm64/wx/dist-arm/bin:$PATH`

### Acciones en cada release

Argregar en la variable de entorno `PATH` la ruta a los binarios de clang que generó *osxcross* y ejecutar:

* Para compilar para arquitectura intel x86_64: `./pack.sh mi64`

* Para compilar para arquitectura arm64: `./pack.sh ma64`.

En ambos casos el resultado queda en `dist`, es un .tgz con la estructura y los metadatos para que macOS lo reconozca como una app.
