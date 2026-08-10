Funcion asignar_y_mostrar(a)
	i<-0
	para cada x de a Hacer
		i<-i+1
		x<-i
	FinPara
	Escribir "Dim: ",i
	para j<-1 hasta i Hacer
		Escribir j,": ",a[j]
	FinPara
FinFuncion

Algoritmo prueba_redimension
	dimension a[3]
	asignar_y_mostrar(a)
	
	redimension a[5]
	asignar_y_mostrar(a)
	
	redimension a[2]
	asignar_y_mostrar(a)
	
FinAlgoritmo
