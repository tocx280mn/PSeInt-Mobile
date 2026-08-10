"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

// funcion que no recibe ni devuelve nada
{SubProceso} Saludar
	Escribir "Hola mundo!"{;}
{FinSubProceso}

// funcion que recibe un argumento por valor, y devuelve su doble
{SubProceso} res <- CalcularDoble(num) 
	{Definir res como real}{;}
	res <- num*2{;} // retorna el doble
{FinSubProceso}

// funcion que recibe un argumento por referencia, y lo modifica
{SubProceso} Triplicar(num por referencia) 
	num <- num*3{;} // modifica la variable duplicando su valor
{FinSubProceso}

// proceso principal, que invoca a las funciones antes declaradas
{Proceso} PruebaFunciones
	
	{Definir x Como Real}{;}
	
	Escribir "Llamada a la funcion Saludar:"{;}
	Saludar{;} // como no recibe argumentos pueden omitirse los paréntesis vacios
	
	Escribir "Ingrese un valor numérico para x:"{;}
	Leer x{;}
	
	Escribir "Llamada a la función CalcularDoble (pasaje por valor)"{;}
	Escribir "El doble de ",x," es ", CalcularDoble(x){;}
	Escribir "El valor original de x es ",x{;}
	
	Escribir "Llamada a la función Triplicar (pasaje por referencia)"{;}
	Triplicar(x){;}
	Escribir "El nuevo valor de x es ", x{;}
	
{FinProceso}
