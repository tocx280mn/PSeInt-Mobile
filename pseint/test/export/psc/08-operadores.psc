// * Este ejemplo es para ver cómo trabajan los operadores.
// * Si hay operadores que no están en el lenguaje, o que no aplican
// para todos los tipos, indicarlo y utilizar si es posible alguna
// función estándar que lo reemplace (por ejemplo, pow en c++ en lugar
// de ^).
// * Si se utilizan bibliotecas, aclarar como se incluyen las bibliotecas
// en ese lenguaje (#include, import, etc).
// * Al final se repiten los operadores, pero aplicados a cadenas de 
// caracteres, ya que en algunos lenguajes las comparaciones se realizan
// de forma especial, y ademas el + tambien tiene un significado 
// especial (concatenar). 

Proceso sin_titulo
	a<-11; b<-2;
	c<-Verdadero;
	d<-falso;
	e<-"Hola"; f<-"Mundo";
	// algebraicos
	Escribir a+b;
	Escribir a*b;
	Escribir a/b;
	Escribir a-b;
	Escribir a^b; // potencia
	Escribir a%b; // resto de la división entera
	// relacionales (comparaciones)
	Escribir a=b;
	Escribir a<>b; // distinto
	Escribir a<b; 
	Escribir a>b; 
	Escribir a<=b; 
	Escribir a>=b; 
	// logicos
	Escribir c y d;
	Escribir c o d;
	Escribir no (c y d);	
	// para cadenas
	Escribir e+" "+f; // el + para cadenas las concatena
	Escribir e=f;
	Escribir e<>f;
	Escribir e<f; 
	Escribir e>f; 
	Escribir e<=f; 
	Escribir e>=f; 
FinProceso
