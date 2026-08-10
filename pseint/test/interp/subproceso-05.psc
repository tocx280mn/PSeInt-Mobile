SubProceso x<-Booga1(a,b)
  Escribir A;
  Escribir B;
  x<-1;
FinSubproceso

SubProceso x<-Booga2
  x<-2;
FinSubproceso

Proceso PruebaFuncion
  Escribir Booga1("a","b");
  Escribir Booga1("c","d")+10;
  Escribir Booga2;
  Escribir Booga2+20;
  Escribir Booga2;
  Escribir Booga2+20;
FinProceso

  
