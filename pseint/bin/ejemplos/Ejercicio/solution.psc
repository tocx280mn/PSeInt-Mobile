Proceso Maniana
	
	Escribir "Cantidad de articulos:";
	Leer N;
	
	Escribir "Precios:"
	
	Monto <- 0;
	Para i<-1 hasta N Hacer
		Leer X;
		Monto <- Monto+X;
	FinPara
	Si Monto>=500 Entonces
		Monto <- Monto - 80;
	FinSi
	
	Escribir "Monto a pagar: $", Monto;
	
FinProceso
