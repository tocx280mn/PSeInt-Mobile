"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

//   Se ingresa una lista de nombres (la lista termina
// cuando se ingresa un nombre en blanco) no permitiendo
// ingresar repetidos y luego se ordena y muestra

{Proceso} OrdenaLista
	
	{Definir nombre,lista,aux Como Cadenas}{;}
	{Definir se_repite Como Logico}{;}
	{Definir cant,i,j,pos_menor Como Enteros}{;}
	Dimension lista[200]{;}
	
	Escribir "Ingrese los nombres (enter en blanco para terminar):"{;}
	
	// leer la lista
	cant<-0{;}
	Leer nombre{;}
	Mientras nombre<>"" Hacer
		{cant<-cant+1#lista[cant]<-nombre}{;}
		{lista[cant]<-nombre#cant<-cant+1}{;}
		Repetir // leer un nombre y ver que no este ya en la lista
			Leer nombre{;}
			se_repite<-Falso{;}
			Para i<-{1#0} Hasta {cant#cant-1} Hacer
				Si nombre=lista[i] Entonces
					se_repite<-Verdadero{;}
				FinSi
			FinPara
		Hasta Que {~} se_repite
	FinMientras
	
	// ordenar
	Para i<-{1#0} Hasta {cant-1#cant-2} Hacer
		// busca el menor entre i y cant
		pos_menor<-i{;}
		Para j<-i+1 Hasta {cant#cant-1} Hacer
			Si lista[j]<lista[pos_menor] Entonces
				pos_menor<-j{;}
			FinSi
		FinPara
		// intercambia el que estaba en i con el menor que encontro
		aux<-lista[i]{;}
		lista[i]<-lista[pos_menor]{;}
		lista[pos_menor]<-aux{;}
	FinPara	
	
	// mostrar como queda la lista
	Escribir "La lista ordenada es:"{;}
	Para i<-{1#0} Hasta {cant#cant-1} Hacer
		Escribir "   ",lista[i]{;}
	FinPara
	
{FinProceso}
