"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

{Proceso} TaTeTi
	
	// incializa dos matrices de 3x3, una para guardar la ficha que se ve, 
	// y otra para un valor asociado a la ficha, para un jugador sera 1, para
	// el otro 2, entoces para ver quien gano se multiplica por fila, por 
	// columna y por diagonal, si el resultado es 1 gano el primer jugador,
	// si es 8 gano el segundo, si es 0 es porque faltaba completar, si
	// es otra cosa, estan las tres fichas, pero no son del mismo jugador
	{Definir Tab1 Como Entero}{;}
	{Definir Tab2 Como Caracter}{;}
	Dimension Tab1[3,3]{;}
	Dimension Tab2[3,3]{;}
	{Definir i, j, CantTurnos, Valor, Pos Como Entero}{;}
	{Definir Objetivo, aux_i, aux_j, aux_d1, aux_d2 Como Entero}{;}
	{Definir Terminado, HayGanador Como Logico}{;}
	{Definir Ficha Como Caracter;}{;}
	Para i <- {1#0} Hasta {3#2} Hacer
		Para j <- {1#0} Hasta {3#2} Hacer
			Tab1[i,j] <- 0{;}
			Tab2[i,j] <- " "{;}
		FinPara
	FinPara
	Terminado <- Falso{;}
	HayGanador <- Falso{;}
	CantTurnos <- 0{;}
	
	Mientras ~ Terminado hacer
		
		// dibuja el tablero
		Borrar Pantalla{;}
		Escribir " "{;}
		Escribir "      ||     ||     "{;}
		Escribir "   ",Tab2[{1#0},{1#0}],"  ||  ",Tab2[{1#0},{2#1}],"  ||  ",Tab2[{1#0},{3#2}]{;}
		Escribir "     1||    2||    3"{;}
		Escribir " =====++=====++======"{;}
		Escribir "      ||     ||     "{;}
		Escribir "   ",Tab2[{2#1},{1#0}],"  ||  ",Tab2[{2#1},{2#1}],"  ||  ",Tab2[{2#1},{3#2}]{;}
		Escribir "     4||    5||    6"{;}
		Escribir " =====++=====++======"{;}
		Escribir "      ||     ||     "{;}
		Escribir "   ",Tab2[{3#2},{1#0}],"  ||  ",Tab2[{3#2},{2#1}],"  ||  ",Tab2[{3#2},{3#2}]{;}
		Escribir "     7||    8||    9"{;}
		Escribir " "{;}
		
		Si ~ HayGanador y CantTurnos<9 Entonces
			
			// carga auxiliares segun a qué jugador le toca
			CantTurnos <- CantTurnos+1{;}
			Si CantTurnos%2=1 Entonces
				Ficha <- 'X'; Valor <-  1; Objetivo <- 1{;}
				Escribir "Turno del jugador 1 (X)"{;}
			SiNo
				Ficha <- 'O'; Valor <-  2; Objetivo <- 8{;}
				Escribir "Turno del jugador 2 (O)"{;}
			FinSi
			
			// pide la posición para colocar la ficha y la valida
			Escribir "Ingrese la Posición (1-9):"{;}
			
			Repetir
				Leer Pos{;}
				Si Pos<1 o Pos>9 Entonces
					Escribir "Posición incorrecta, ingrese nuevamente: "{;}
					Pos <- 99;
				SiNo
					i <- trunc((Pos-1)/3){+1#}{;}
					j <- ((Pos-1) {%} 3){+1#}{;}
					Si Tab1[i,j]<>0 Entonces
						pos <- 99{;}
						Escribir "Posición incorrecta, ingrese nuevamente: "{;}
					FinSi
				FinSi
			Hasta Que Pos<>99
			// guarda la ficha en la matriz tab2 y el valor en tab1
			Tab1[i,j] <- Valor{;}
			Tab2[i,j] <- Ficha{;}
			
			// verifica si ganó, buscando que el producto de las fichas en el tablero de Objetivo
			aux_d1 <- 1; aux_d2 <- 1{;}
			Para i <- {1#0} hasta {3#2} hacer
				aux_i <- 1; aux_j <- 1{;}
				aux_d1 <- aux_d1*Tab1[i,i]{;}
				aux_d2 <- aux_d2*Tab1[i,{4#2}-i]{;}
				Para j <- {1#0} hasta {3#2} hacer
					aux_i <- aux_i*Tab1[i,j]{;}
					aux_j <- aux_j*Tab1[j,i]{;}
				FinPara
				Si aux_i=Objetivo o aux_j=Objetivo Entonces
					HayGanador <- Verdadero{;}
				FinSi
			FinPara
			Si aux_d1=Objetivo o aux_d2=Objetivo Entonces
				HayGanador <- Verdadero{;}
			FinSi
			
		SiNo
			
			Si HayGanador Entonces
				Escribir "Hay ganador: " sin saltar{;}
				Si CantTurnos%2=1 Entonces
					Escribir "Jugador 1!"{;}
				SiNo
					Escribir "Jugador 2!"{;}
				FinSi
			SiNo
				Escribir "Empate!"{;}
			FinSi
			Terminado <- Verdadero{;}
			
		FinSi
		
	FinMientras
	
{FinProceso}
