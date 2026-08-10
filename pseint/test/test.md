En esta carpeta se encuentra scripts de bash y datos de ejemplo para testear automáticamente los módulos pseint, psdraw y psexport.

## interp

Este directorio contiene 2 scripts de bash y casos de prueba para testear el módulo principal. 

Por cada caso de prueba debe haber:

* **archivo .psc**: el algoritmo completo del ejemplo a correr.
* **archivo .out**: la salida de referencia. 
  * Si se corre un caso sin este archivo, el script de test lo genera, y muestra tanto la entrada (el psc) como la salida generada.
* **archivo.arg**: argumentos adicionales para pasarle al intérprete.
  * Es opcional, si no está el archivo no se usan argumentos adicionales.
* **archivo.in**: la entrada para la ejecución del algoritmo (para las instrucciones Leer).
  * Es opcional, si el caso de pruba no requiere datos de entrada puede omitirse.

Para correr los casos se utilizan 2 scripts:

* **run.sh**: Ejecuta un caso de prueba y verifica la salida
  * Se le pasa como argumento el nombre de un caso de prueba (el nombre del archivo  del pseudocódigo pero sin la extensión .psc)
  * Por defecto utiliza el ejecutable del intérprete que se encuentra en bin/bin.. si se quiere usar otro (por ej, uno para debug) se debe agregar como argumento "-bin <ruta_al_ejecutable>" antes del nombre del caso a correr
  * Si la salida de un caso ejecutado es correcta (coincide con el archivo .out), entonces no muestra nada.
  * Si no recibe el argumento especial para indicar el ejecutable, y el ejecutable por defecto no existe o está desactualizado, este script lo compila primero (con make ARCH=lnx).
* runall.sh: compila el ejecutable si es necesario y luego corre todos los casos que encuentra en el directorio. Si la salida es correcta para todos los casos, no debería ver más que la salida del "make", y "done" al final.
  * Si la salida es incorrecta, se muestra el nombre del caso y el resultado del diff entre la salida real y la qe se obtuvo.
  * Si un algoritmo de prueba entra en un loop infinito, el script no lo detendrá, pero verá el nombre del caso en la consola (cuando no hay loops infinito, el nombre va cambiando rápidamente hasta pasar por todos los casos).
  * También se puede utilizar un ejecutable alternativo pasando su ruta como primer y único argumento.

Los casos se corren utilizando algunos argumentos especiales para el intérprete para que genere un error en caso de requerir entradas que no estén presentes en el caso de prueba, y para que los errores muestren solo el número pero no el mensaje descriptivo (para poder mejorar un mensaje sin que fallen los tests).

## export

En esta carpeta hay archivos para probar el módulo que exporta de pseudocódigo a otros lenguajes. Hay 10 pseudocódigos de ejemplo en la carpeta .psc. Y por cada otro lenguaje, hay una carpeta de igual nombre que el identificador del lenguaje, con los 10 ejemplos traducidos. Una carpeta adicional ref contiene por cada lenguaje la salida esperada del test para los 10 ejemplos (no siempre la traducción es perfecta, por eso en la salida esperada pueden aparecer diferencias entre lo que arroja psexport y el código de ejemplo de ese lenguaje). Hay además dos scripts de bash para automatizar las corridas:

* **test.sh** recibe el nombre de un lenguaje y corre los 10 ejemplos, mostrando luego de cada uno un diff entre la salida ideal (la de la carpeta del lenguaje) y la que se obtuvo (que se guarda en temp).
* **test_all.sh** corre al script test.sh con todos los lenguajes disponibles, y muestra al final una comparación de los resultados (las salidas de test.sh, que estarán en la carpeta cur) con las salidas esperadas (que estarán en la carpeta ref). Para mostrar la diferencia se usar un comparador gráfico llamado meld.

Para correr los ejemplos se requieren tener compilados los modules pseint y psexport, ya que el primero genera una salida intermedia que sirve de entrada para el segundo (estos archivos intermedios estaran en la carpeta psd).

## draw

En esta carpeta hay un script **run.sh** que genera (con los ejecutables de pseint y psdrawE) el diagrama de flujo de todos los ejemplos que encuentre en la carpeta psc. Luego compara las salidas (que estarán en tmp) con salidas de referencia (que estan en ref) e imprime en consola un indicador de la diferencia (que debería ser 0) generado con la herramienta "compare" de ImageMagik. Si se indica alguna diferencia, en la carpeta dif se puede encontrar una imagen que la resalta.
