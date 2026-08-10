37
2 1 1 
* Este ejemplo es para ver cómo se declaran y definen variables 
2 2 1 
y constantes de distintos tipos. En pseudocódigo flexible no hace 
2 3 1 
falta explicitar el tipo, pero en muchos lenguajes sí. 
2 4 1 
* Si el lenguaje permite abreviar las acciones de declarar y asignar
2 5 1 
el valor en una sola, es preferible que la traducción no utilice
2 6 1 
ese recurso. Ej: en C++ preferiría "int a; a=1;" antes que "int a=1;"
2 7 1 
* También es para observar cómo se escriben las constantes
2 8 1 
de los diferentes tipos, y cómo se realizan las asignaciones.
2 9 1 
* Si el lenguaje permite diferenciar entero de real, a debe ser entero
2 10 1 
y b real.
2 11 1 
* Si hay varias formas de operar con cadenas, pensar con cual es
2 12 1 
más fácil realizar la entrada/salida, y aplicar los operadores.
2 13 1 
Por ejemplo, en C++ usando la clase string, los operadores se
2 14 1 
comportan como en pseudocódigo, mientras que usando arreglos
2 15 1 
de chars hay que invocar funciones.
2 16 1 
* Alcarar además si para las cadenas literales puede llegar a ser
2 17 1 
necesario agregar caracteres de escape (por ej, en C/C++ si está
2 18 1 
el caracter de escape "\" hay que agregar otra ("\\").
2 19 1 
* Por último, el caso de la variable p, es un caso donde
2 20 1 
no se puede determinar el tipo, ya que dependerá de qué ingrese
2 21 1 
el usuario cuando ejecute el programa. Indicar si el lenguaje es
2 22 1 
tipado o permite hacer esto. Si es tipado, indicar si existe
2 23 1 
algun mecanismo simple para que el programa compile asumiendo que
2 24 1 
es un número, pero permita al usuario ver fácilmente el problema
2 25 1 
y cambiar el tipo. Por ejemplo, en C/C++ uso "#define SIN_TIPO float"
2 26 1 
para luego declarar las variables como "SIN_TIPO x;".
3 28 1 PROCESO SIN_TITULO
1 36

sin_titulo

9 29 1 DEFINIR A COMO ENTERO;
1
a
0 1 0 1
6 30 1 A<-1;
a
1
6 31 1 B<-2.5;
b
2.5
6 32 1 C<-VERDADERO;
c
verdadero
6 33 1 D<-FALSO;
d
Falso
6 34 1 E<-'Hola';
e
'Hola'
6 35 1 F<-'Hola Mundo';
f
'Hola Mundo'
5 37 1 LEER P;
1
p
7 38 1 ESCRIBIR P;
1 1
p
4 39 1 FINPROCESO
1 sin_titulo
