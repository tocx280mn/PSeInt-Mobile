"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

//    Lee los tres lados de un triangulo rectangulo, determina 
// si corresponden (por Pitargoras) y en caso afirmativo 
// calcula el area

{Proceso} TrianguloRectangulo
	

	// cargar datos
	{Definir l1,l2,l3 Como Real}{;}
	Escribir "Ingrese el lado 1:"{;}
	Leer l1{;}
	Escribir "Ingrese el lado 2:"{;}
	Leer l2{;}
	Escribir "Ingrese el lado 3:"{;}
	Leer l3{;}
	
	// encontrar la hipotenusa (mayor lado)
	{Definir cat1,cat2,hip Como Real}{;}
	Si l1>l2 Entonces
		cat1<-l2{;}
		Si l1>l3 Entonces
			hip<-l1{;}
			cat2<-l3{;}
		SiNo
			hip<-l3{;}
			cat2<-l1{;}
		FinSi
	SiNo
		cat1<-l1{;}
		Si l2>l3 Entonces
			hip<-l2{;}
			cat2<-l3{;}
		SiNo
			hip<-l3{;}
			cat2<-l2{;}
		FinSi
	FinSi
	
	// ver si cumple con Pitagoras
	Si hip^2 = cat1^2 + cat2^2 Entonces
		// calcualar area
		{Definir area como real;}
		area<-(cat1*cat2)/2{;}
		Escribir "El area es: ",area{;}
	SiNo
		Escribir "No es un triangulo rectangulo."{;}
	FinSi
	
{FinProceso}
