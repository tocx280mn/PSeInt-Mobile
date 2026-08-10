// * Este ejemplo es para ver la forma de declarar y utilizar
// subprocesos/funciones.
// * Aclarar si hay diferencia entre subprocesos/funciones que no
// retornen nada, y los que sí retornen algún resultado.
// * Las dos primeras reciben argumentos por copia, las tres ultimas
// por referencia (en PSeInt, cuando no se indica se utiliza por copia,
// menos los arreglos que siempre pasan por referencia)
// * Indicar las consideraciones especiales que se deban tener en cuenta
// al pasar arreglos.
// * Si es necesario hacer fordware declarations de las funciones
// cuando no se implementan en orden, indicarlo y ejemplificarlo.
	
Proceso sin_titulo
	
	Escribir FuncionDoble(5);
	
	NoRetornaNada(3,9);
	
	c<-0;
	PorReferencia(c);
	Escribir c;
	
	Dimension a[10],b[3,4];
	RecibeVector(a);
	RecibeMatriz(b);
	
FinProceso

Funcion a<-FuncionDoble(b)
	a<-2*b;
FinFuncion

SubProceso NoRetornaNada(a,b)
	escribir a+b;
FinSubProceso

SubProceso PorReferencia(b por referencia)
	b<-7;
FinSubProceso

SubProceso RecibeVector(v)
	Leer v[1];
FinSubProceso

SubProceso RecibeMatriz(m)
	Leer m[1,1];
FinSubProceso
