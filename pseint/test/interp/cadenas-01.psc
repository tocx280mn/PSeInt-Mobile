Proceso sin_titulo
	frase<-"HoLA mUNdo";
	frase<-Minusculas(frase);
	Vocales<-"aeiouáéíóúü";
	cantvocales<-0;
	Para i<-1 hasta Longitud(frase) Hacer
		Para j<-1 hasta Longitud(vocales) Hacer
			Si Subcadena(frase,i,i)=Subcadena(vocales,j,j) Entonces
				cantVocales<-cantVocales+1;
			FinSi
		FinPara
	FinPara
	Escribir cantVocales;
FinProceso
