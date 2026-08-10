Proceso sin_titulo
	leer N
	Si N<0  entonces 
		Nu<-N*(-1)
	SiNo
		Nu<-N
	FinSi
	Si N mod 2 = 0 Entonces 
		Escribir 'Numero Primo:',Nu=2
		Si Nu<>2 Entonces
			Escribir N,'=2x',N/2
		FinSi
	Sino
		EsPrimo<-Nu<>1 
		Nu<-RC(Nu)
		f<-3
		Mientras f<=Nu Y EsPrimo Hacer
			Si N%F=0 Entonces 
				EsPrimo<-Falso
			FinSi
			f<-f+2
		FinMientras
		Escribir 'Numero Primo:',EsPrimo
		Si N>1 Y NO EsPrimo Entonces 
			Escribir N,'=',f-2,'x',N/(f-2)
		FinSi
	FinSi
FinProceso
