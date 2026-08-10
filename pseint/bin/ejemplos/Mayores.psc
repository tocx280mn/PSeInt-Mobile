"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

// Busca los dos mayores de una lista de N datos

{Proceso} Mayores

	{Definir datos,n,may1,may2,i Como Numeros}{;}
	// primero se declara un arreglo de 200 elementos
	Dimension datos[200]{;}
	
	// luego se pide al usuario ingresar la cantidad de datos, 
	// que no debera ser mayor a 200
	Escribir "Ingrese la cantidad de datos (de 2 a 200):"{;}
	Leer n{;}
	
	// se leen uno por uno los 200 datos y se los guarda en el arreglo
	Para i<-{1#0} Hasta {n#n-1} Hacer
		Escribir "Ingrese el dato ",{i#i+1},":"{;}
		Leer datos[i]{;}
	FinPara

	// se comparan los dos primeros y se toman como may1 (el mayor de los
	// dos) y may2 (el segundo mayor).
	Si datos[{1#0}]>datos[{2#1}] Entonces
		may1<-datos[{1#0}]{;}
		may2<-datos[{2#1}]{;}
	SiNo
		may1<-datos[{2#1}]{;}
		may2<-datos[{1#0}]{;}
	FinSi
	// se recorren los demas elementos buscan si hay alguno mayor que may1 o may2
	Para i<-{3#2} Hasta {n#n-1} Hacer
		Si datos[i]>may1 Entonces // si hay un valor mayor que may1
			may2<-may1{;} // como may1 era el más grande, pasa a estar en segundo lugar
			may1<-datos[i]{;} // y el nuevo dato toma el primer puesto (mayor de todos)
		SiNo // si no era mas grande que may1, todavia puede ser mas grande que may2
			Si datos[i]>may2 Entonces // si supera al segundo mayor que teniamos
				may2<-datos[i]{;} // se lo guarda como segundo mayor
			FinSi
		FinSi
	FinPara
	
	// se muestran los resultados
	Escribir "El mayor es: ",may1{;}
	Escribir "El segundo mayor es: ",may2{;}
	
{FinProceso}
