SubProceso asigna(a Por Referencia)
	a<-5;
FinSubProceso

SubProceso hace(v Por Referencia) 
	asigna(v[1]);
FinSubProceso

Proceso sin_titulo
	dimension v[5];
	hace(v)
	Escribir v[1];
FinProceso
