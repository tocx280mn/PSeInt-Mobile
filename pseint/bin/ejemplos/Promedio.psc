"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

// Calcula el promedio de una lista de N datos

{Proceso} Promedio

	{Definir i,N como Entero}{;}
	{Definir acum,dato,prom como Reales}{;}
	Escribir "Ingrese la cantidad de datos:"{;}
	Leer n{;}
	
	acum<-0{;}
	
	Para i<-1 Hasta n Hacer
		Escribir "Ingrese el dato ",i,":"{;}
		Leer dato{;}
		acum<-acum+dato{;}
	FinPara
	
	prom<-acum/n{;}
	
	Escribir "El promedio es: ",prom{;}
	
{FinProceso}
