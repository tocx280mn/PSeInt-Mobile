Funcion muestra_arreglo_1(A)
	a[2]<-'z';
FinFuncion

funcion muestra_arreglo_2(A)
	para cada elemento de a Hacer
		Escribir elemento
	FinPara
FinFuncion

Proceso sin_titulo
	dimension arreglo[5];
	arreglo[1]<-'a';
	arreglo[2]<-'b';
	arreglo[3]<-'c';
	arreglo[4]<-'d';
	arreglo[5]<-'e';
	muestra_arreglo_1(arreglo);
	muestra_arreglo_2(arreglo);
FinProceso
