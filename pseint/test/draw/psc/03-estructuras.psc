// * Este ejemplo es para observar las estructuras condicionales y 
// repetitivas más simples

Proceso sin_titulo
	c<-0;
	Mientras c<=10 Hacer
		c<-c+1;
		Escribir c;
	FinMientras
	
	c<-0;
	Repetir 
		c<-c+1;
		Escribir c;
	Hasta que c=10
	
	c<-0;
	Repetir 
		c<-c+1;
		Escribir c;
	Mientras que c<10
	
	si c=10 Entonces
		Escribir "Si";
	FinSi
	
	si c=10 Entonces
		Escribir "Si";
	Sino
		Escribir "No";
	FinSi
	
FinProceso
