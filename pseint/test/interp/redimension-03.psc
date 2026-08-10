Algoritmo prueba_redimension
	dimension a[3]
	a[3] <- 1
	Escribir "3: ", a[3]
	redimension a[2]
	redimension a[3]
	Escribir "3: ", a[3]
	Escribir ""
	
	dimension M[5,5]
	m[5,5] <- 1
	Escribir "5,5: ", m[5,5]
	redimension m[5,4]
	redimension m[5,5]
	Escribir "5,5: ", m[5,5]
	Escribir ""
	m[5,5] <- 1
	Escribir "5,5: ", m[5,5]
	redimension m[4,5]
	redimension m[5,5]
	Escribir "5,5: ", m[5,5]
FinAlgoritmo
