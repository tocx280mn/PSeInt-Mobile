SubProceso sub(a)
	a[3]<-a[4];
	Escribir a[2];
	leer a[1];
FinSubProceso
Proceso sin_titulo
	Dimension a[10];
	sub(a);
FinProceso
