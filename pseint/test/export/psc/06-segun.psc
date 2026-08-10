// * Este ejemplo es para observar la estructura segun:
// * Aclarar de qué tipo puede ser o no ser la variable de control
// (por ejemplo, en C++ c solo podría ser numérica)
// * Aclarar si es obligatorio o no colocar la sección por defecto
// (la correspondiente al "De Otro Modo:").

Proceso sin_titulo
	Leer c;
	segun c hacer
		1: 
			Escribir "1";
		3,5,9: 
			Escribir "3, 5 o 9";
		7:
			Escribir "7";
		De otro modo:
			Escribir "ni 1, ni 3, ni 5, ni 7, ni 9";
	FinSegun
FinProceso
