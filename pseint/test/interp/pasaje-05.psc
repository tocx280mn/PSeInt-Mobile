subproceso ref(a por referencia)
	escribir a;
FinSubProceso
Proceso sin_titulo
	Dimension w[10];
	w[1]<-"hola";
	x<-1;
	ref(w[x]);
FinProceso
