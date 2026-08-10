"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

// Separa un numero entero en sus digitos

{Proceso} Digitos
	
	{Definir i,digito,pot,n,aux,cont Como Enteros}{;}
	Escribir "Ingrese un numero entero postivo:"{;}
	Leer n{;}
	
	// primero, contar cuantos digitos
	cont <- 0{;} 
	aux <- n{;}
	Mientras aux>0 hacer // mientras no sea cero
		cont <- cont + 1{;} // contar cuantos digitos
		aux <- trunc(aux/10){;} // dividir por 10 y despreciar los de
	FinMientras
	Escribir "El numero tiene ",cont," digitos"{;}
	
	// luego, mostrarlos uno por uno 
	aux<-n{;}
	Para i<-1 hasta cont Hacer
		pot <- 10^(cont-i){;} // por cuanto hay que dividir para obtener el primer digito
		digito <- trunc (aux / pot){;} // obtener el digito
		aux <- aux - digito*pot{;} // quitar ese digito al numero
		Escribir "El digito ",i," es ",digito{;}
	FinPara
	
{FinProceso}
