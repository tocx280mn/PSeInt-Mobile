// * Este ejemplo es para ver cómo se declaran y definen variables 
// y constantes de distintos tipos. En pseudocódigo flexible no hace 
// falta explicitar el tipo, pero en muchos lenguajes sí. 
// * Si el lenguaje permite abreviar las acciones de declarar y asignar
// el valor en una sola, es preferible que la traducción no utilice
// ese recurso. Ej: en C++ preferiría "int a; a=1;" antes que "int a=1;"
// * También es para observar cómo se escriben las constantes
// de los diferentes tipos, y cómo se realizan las asignaciones.
// * Si el lenguaje permite diferenciar entero de real, a debe ser entero
// y b real.
// * Si hay varias formas de operar con cadenas, pensar con cual es
// más fácil realizar la entrada/salida, y aplicar los operadores.
// Por ejemplo, en C++ usando la clase string, los operadores se
// comportan como en pseudocódigo, mientras que usando arreglos
// de chars hay que invocar funciones.
// * Alcarar además si para las cadenas literales puede llegar a ser
// necesario agregar caracteres de escape (por ej, en C/C++ si está
// el caracter de escape "\" hay que agregar otra ("\\").
// * Por último, el caso de la variable p, es un caso donde
// no se puede determinar el tipo, ya que dependerá de qué ingrese
// el usuario cuando ejecute el programa. Indicar si el lenguaje es
// tipado o permite hacer esto. Si es tipado, indicar si existe
// algun mecanismo simple para que el programa compile asumiendo que
// es un número, pero permita al usuario ver fácilmente el problema
// y cambiar el tipo. Por ejemplo, en C/C++ uso "#define SIN_TIPO float"
// para luego declarar las variables como "SIN_TIPO x;".

Proceso sin_titulo
	definir a como entero;
	a<-1;
	b<-2.5;
	c<-verdadero;
	d<-Falso;
	e<-"Hola";
	f<-"Hola Mundo";
	
	Leer p;
	Escribir p;
FinProceso
