Algoritmo Vacio
	Dimensionar X[0]
	Leer A
	C<-0
	
	Mientras A<>0 Hacer
		C<-C+1
		Redimensionar X[C]
		X[C] <- A
		
		Leer A
	FinMientras
	
	Para i<-1 Hasta C Hacer
		Escribir X[i]
	FinPara
FinAlgoritmo
