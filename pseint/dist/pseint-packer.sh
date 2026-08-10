#!/bin/bash

# Este script sirve para armar el paquete binario distribuible para GNU/Linux desde 
# una maquina virtual. En la maquina real debe estar el paquete de fuentes (se genera 
# con "./pack.sh src") y a este script se le pasa como argumento la version 
# de ese paquete.

# Es importante correr este script con bash y no con otro shell no compatible

function get_arch {
  if uname -a | grep x86_64 >/dev/null; then echo l64; else echo l32; fi
}

if ! test -e ./pseint-packer.cfg; then
	touch ./pseint-packer.cfg
	echo '# usuario, ip, y ruta hacia el directorio donde se encuentran los fuentes' >> ./pseint-packer.cfg
	echo '# de pseint, y donde se va a copiar luego el resultado' >> ./pseint-packer.cfg
	echo 'SSHSRC="usuario@10.0.2.4:/home/usuario/pseint/dist"' >> ./pseint-packer.cfg
	echo '' >> ./pseint-packer.cfg
	echo '# tipo de arquitectura para la que se compila, que se usara para el nombre' >> ./pseint-packer.cfg
	echo '# del paquete generado (l32 o l64)' >> ./pseint-packer.cfg
	echo 'ARCH="'`get_arch`'"' >> ./pseint-packer.cfg
	echo '' >> ./pseint-packer.cfg
	echo '# directorio con los .so de a incluir en el paquete... debería haber allí dos' >> ./pseint-packer.cfg
	echo '# subdirectorios, png y wx, cada uno con sus .so y un txt con la licencia' >> ./pseint-packer.cfg
	echo 'LIBSDIR="libs-pseint"' >> ./pseint-packer.cfg
	echo '' >> ./pseint-packer.cfg
	echo '# puede modificar aquí otras variables si fuera necesario' >> ./pseint-packer.cfg
	echo '#PATH=/home/usuario/wx/bin:$PATH' >> ./pseint-packer.cfg
	echo '' >> ./pseint-packer.cfg
	echo "Creado un archivo \"pseint-packer.cfg\" de ejemplo. Verifique su contenido y'vuelva a ejecutar este script."
	exit 1
fi
source ./pseint-packer.cfg
if [ "$1" == "" ]; then
	echo "Debe utilizar la version de pseint (por ej: 20200428) como argumento de este script."
	exit 2
fi

if ! [ "$2" = "fast" ]; then
	TAR_OPTS=""
	rm -rf pseint
else
	TAR_OPTS="--keep-new-files"
fi
scp $SSHSRC/pseint-src-$1.tgz . || exit 3
tar $TAR_OPTS -xzvf pseint-src-$1.tgz

if ! [ "$LIBSDIR" = "" ]; then 	
	mkdir -p pseint/bin/lib/
	cp -rf "$LIBSDIR"/* pseint/bin/lib/
fi

cd pseint
./pack.sh $ARCH
scp dist/pseint-$ARCH-$1.tgz $SSHSRC/pseint-$ARCH-$1.tgz
