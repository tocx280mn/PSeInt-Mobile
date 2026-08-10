Proceso sin_titulo
	Dimension a[2,2];
	i<-1; j<-2;
	leer a[i,1];
	leer a[i,2];
	leer a[2,1];
	leer a[2,j];
	a[1,1]<-a[j-1,i];
	a[j,2*i]<-a[2,2*i];
	Escribir a[i,i];
	Escribir a[j-i,2*i];
	Escribir a[j,1];
	Escribir a[2,j+i-1];
FinProceso
