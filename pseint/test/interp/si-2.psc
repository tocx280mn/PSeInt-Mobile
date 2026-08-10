Proceso sin_titulo
	// prueba entonces: ok
	Si x Entonces
		a<-b
	FinSi
	Si x 
		Entonces
		a<-b
	FinSi
	Si x Entonces a<-b
	FinSi
	Si x 
		a<-b
	FinSi
	
	// prueba entonces: bad
	Si x Entonces
		Entonces
		a<-b
	FinSi
	Si x Entonces
		a<-b
		Entonces
	FinSi
	Si x
		a<-b
		Entonces
	FinSi
	Si x Entonces
		a<-b
		Entonces
	FinSi
	
	// prueba sino: ok
	Si x Entonces
		a<-b
	Sino
		d<-e
	FinSi
	Si x 
		a<-b
	SiNo
		d<-e
	FinSi
	Si x Entonces
		a<-b
	Sino
		d<-e
	FinSi
	
	// prueba sino: bad
	Si x Entonces
		a<-b
	SiNo
		d<-e
	SiNo
		d<-e
	FinSi
	Si x Entonces
	Sino
		d<-e
	FinSi
FinProceso
