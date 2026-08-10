subproceso val(a por valor)
	escribir a;
FinSubProceso
subproceso ref(a por referencia)
	escribir a;
FinSubProceso
Proceso sin_titulo
	Dimension w[10];
	w[1]<-"hola";
	ref(w[1]);
	val(w[1]);
FinProceso
