Inicio Rapido
---------------

Si solo quiere compilar y utilizar el programa instale las dependencias (ver requisitos) 
y utilice el comando `ming32-make ARCH=w32` si esta en *Microsoft Windows*, o el comando
`make ARCH=lnx` si está en *GNU/Linux* o *macOS*. Los binarios se ubicarán en el directorio bin. 
Desde allí puede ejecutar `./pseint` para comenzar a utilizar el programa.


Requisitos para compilar PSeInt
---------------------------------

Para compilar pseint se requiere la biblioteca wxWidgets 3.1.x o superior, compilada en modo unicode,
con los componentes de OpenGL y Scintilla habilitados. En  la mayoría de los sistemas GNU/Linux se puede 
conseguir facilmente a través del repositorio de la distribución. Si utiliza Windows puede descargar 
el IDE ZinjaI (http://zinjai.sf.net) que incluye el compilador mingw64 utilizado para PSeInt, y para el
cual puede descargar en el mismo sitio un complemento con los binarios adecuados de wxWidgets.


Acerca de los fuentes de PSeInt
---------------------------------

Contenido de los directorios:

* `pseint`: fuentes del interprete (verifica sintaxis e interpreta un algoritmo)
* `psdraw3`: fuentes del editor de diagramas de flujo (permite editar el diagrama)
* `psdrawE`: fuentes del exportador de diagramas de flujo (genera imagenes png/jpg/bmp)
* `psexport`: fuentes del exportador (convierte de pseudocodigo a codigo C++)
* `wxPSeInt`: fuentes de la nueva interfaz (editor de pseudocódigo, e interfaz principal)
* `psterm`: fuentes de la terminal que se utiliza para ejecutar desde wxPSeInt
* `pseval`: fuentes de la interfaz que genera y evalúa los ejercicios autocontenidos
* `updatem`: fuentes del programa que busca actualizacionesç
* `dtl`: biblioteca auxiliar para comparar texto simil diff
* `hoewrap`: biblioteca auxiliar (hoedown) para convertir markdown a html
* `test`: pseudocódigos con casos de prueba y scripts para correrlos de forma automática
* `bin`: demás archivos necesarios para ejecutar wxPSeInt (imagenes, documentacion, ayuda, etc).
* `dist`: archivos adicionales para generar paquetes e instaladores
* `configs`: configuraciones de los distintos toolchains para compilar con los Makefiles

Puede encontrar más información de la función de cada módulo y de cómo se comunican entre
ellos en http://cucarachasracing.blogspot.com.ar/2012/12/destripando-pseint.html

Hay un Makefile general para compilar todo desde el directorio principal, y también un 
Makefile en cada directorio para cada proyecto o componente. Para compilar con cualquiera
de ellos hay que definir la variable ARCH:

- Para compilar directo (en un sistema para ese mismo sistema), usar
  - `make ARCH=lnx` para compilar todo en GNU/Linux o macOS
  - `mingw32-make ARCH=w32` para compilar todo en Microsoft Windows
- Puede ver en `release-process.md` más detalles sobre cómo se compilan todas las versiones desde GNU/Linux.

Los Makefiles toman las configuración del toolchain a utilizar a partir de los archivos
`configs/config.xxx` donde `xxx` es el valor de `ARCH`. Si necesita ajustar los comandos de
compilación para su sistema/compilador, estos son los archivos a modificar. Son archivos
que serán incluidos en los Makefiles (directiva "include"), por lo que la sintaxis de los
mismos es la de un Makefile.
        
Además, hay un archivo pack.sh que sirve para generar los paquetes e instaladores
de todo el software desde GNU/Linux. Para generar el instalador para Windows requiere 
wine+mingw64+wxwidgets+inno o nsis. Para las versiones para macOS se utilizó osxcross
para generar versiones de clang adecuadas.

Los fuentes son exactamente los mismos tanto para GNU/Linux como para MS Windows como para macOS.
Todo está programado con C++17. El framework utilizado para el desarrollo de la GUI es wxWidgets

Para más detalles de cómo se construyen los paquetes oficiales que se distribuyen en el sitio,
consulte el archivo release-process.md"

En cada directorio de fuentes hay un archivo .zpr. Este es el archivo de proyecto para ZinjaI.
Dado que el 99% del desarrollo ocurre en GNU/Linux. Este proyecto seguramente tendrá su configuración
para GNU/Linux actualizada, pero puede tener desactualizadas las configuraciones para otros sistemas.
Puede utilizar los archivos config para encontrar configuraciones actualizadas, ya que son los que
efectivamente se utilizan para generar los paquetes e instaladores que se publican en cada versión.

Los recurso gráficos son todos propios del proyecto (algunos compartidos con ZinjaI, proyecto
del mismo autor), a excepción de la tipografía Inconsolata, de Raph Levien.

Enlaces externos:

* wxWidgets: http://www.wxwidgets.org
* ZinjaI: http://zinjai.sourceforge.net
* Hoedown: https://github.com/hoedown/hoedown.git
* Diff Template Library (dtl): https://github.com/cubicdaiya/dtl.git
* wine: http://winehq.org
* NSIS: https://nsis.sourceforge.io/Main_Page
* Inno Setup: https://jrsoftware.org/isinfo.php
* osxcross: https://github.com/tpoechtrager/osxcross
* Inconsolata: https://levien.com/type/myfonts/inconsolata.html

Actualizaciones: http://pseint.sourceforge.net

Para enviar comentarios, consultas, reportes de errores, etc, acceda a los foros en:
http://pseint.sourceforge.net/index.php?page=contacto.php

Todo se distribuye bajo licencia GPL (ver pack/help/gpl.html).

Por Pablo Novara (zaskar_84@yahoo.com.ar)