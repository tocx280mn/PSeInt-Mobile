"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

// Si tiene problemas para ejecutar este ejemplo seleccione el perfil "Flexible"
// en el cuadro de "Opciones del lenguaje", al cual se accede desde el menú Configurar.

// Este ejemplo muestra algunas de las variaciones posibles cuando se utiliza
// syntaxis flexible, instrucciones adicionales, y expresiones en lenguaje
// coloquial para las condiciones.

Algoritmo sin_titulo
	
	Definir a, b Como Entero
	
	Imprimir "Ingrese 3 numeros (los dos primeros deben ser enteros):"
	Leer a b c
	
	Si a Es Mayor Que b Y a Es Mayor Que c Entonces
		Escribir a " es el mayor"
	SiNo 
		Si b Es Mayor Que c Entonces
			Escribir b " es el mayor"
		SiNo
			Escribir c " es el mayor"
		FinSi
	FinSi

	
	Mostrar "Pulse una tecla para continuar"
	Esperar Tecla
	Limpiar Pantalla
	
	Si a Es Par
		Escribir a " es par"
	SiNo
		Escribir a " es impar"
	FinSi
	
	Escribir Sin Bajar "Los divisores de " b " son: "
	Para i Desde 1 Hasta b-1
		Si b Es Multiplo De i
			Escribir sin bajar i " "
		Fin Si
	Fin Para
	Escribir b
	
	Si c Es Entero 
		Mostrar C " es entero"
	SiNo
		Mostrar C " es real"
	FinSi
	
FinAlgoritmo
