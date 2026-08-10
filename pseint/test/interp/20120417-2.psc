Proceso SegunAnidado
	leer x; 
	Segun x Hacer 
		1: 
			a<- 1; 
			Segun a Hacer 
				1: 
					b<-1; 
				2: 
					b<-2; 
				3: 
					b<-3; 
				De Otro Modo: 
					b<-0; 
			FinSegun 
			si b>=0 
				Entonces
				Escribir "si funciona"; 
			FinSi 
		2: 
			a<-2; 
		3: 
			a<-3; 
		De Otro Modo: 
			a<-0; 
	FinSegun 
	Escribir a,b,x;
FinProceso