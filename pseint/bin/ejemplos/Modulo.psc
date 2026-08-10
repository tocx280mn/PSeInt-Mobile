"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

{Proceso} Modulo
	{Definir N,M Como Enteros}{;}
	Escribir "Ingrese el numero: "{;}
	Leer N{;}
	Escribir "Ingrese el divisor: "{;}
	Leer M{;}
	Si N {%} M = 0 Entonces
		Escribir M," es divisor exacto de ",N,"."{;}
	SiNo
		Escribir "El resto de dividir ",N," por ",M," es: ",N {%} M{;}
	FinSi
{FinProceso}
