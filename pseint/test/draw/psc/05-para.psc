// * Este ejemplo es para observar la estructura para/for y su 
// alternativa para arreglos para cada/for each
// * La estructura para cada itera por todos los elementos de un
// arreglo (puede ser multidimensional), generando en cada iteración
// un alias al elemento del arreglo con el nombre que se indique luegoe 
// de la palabra cada. Puede que el lenguaje no tenga esta estructura. 
// Si hay una forma de simularla, puede utilizarla, sino alcarar que no
// es posible. Por ejemplo, C++99/03 no tiene esta estructura de control,
// pero dado que los elementos de una matriz están contiguos en memoria
// se pueden recorrer con un puntero de una forma generalizable.
// * Si el for del lenguaje tiene partes opcionales, puede omitirlas donde
// no sean estrictamente necesarias (como el paso en pseint cuando es 1).

Proceso sin_titulo
	
	Dimension a[10];
	
	Para i desde 1 hasta 10 Hacer
		a[i]<-i*10;
	FinPara
	
	Para cada elemento de a Hacer
		Escribir elemento;
	FinPara
	
	Dimension b[3,6];
	c<-0;
	para cada x de b Hacer
		c<-c+1;
		x<-c;
	FinPara
	
	para i desde 3 hasta 1 con paso -1 Hacer
		para j desde 1 hasta 5 con paso 2 hacer
			Escribir b[i,j];
		FinPara
	FinPara
	
FinProceso
