Funcion x3(a Por Referencia)
	a<-20;
FinFuncion

Funcion x2(a Por Referencia)
	a[1]<-10;
FinFuncion

Funcion x1(a Por Referencia)
	x2(a);
	x3(a[2]);
FinFuncion

Proceso sin_titulo
	Dimension a[5];
	a[1]<-1;
	a[2]<-2;
	x1(a);
	Escribir a[1];
	Escribir a[2];
FinProceso
