funcion llenar(a,n por referencia)
  para cada x en a hacer
    x <- n;
    n <- n+1;
  finpara
finfuncion
funcion muestra(nom,a)
  escribir nom,":" sin bajar;
  para cada x en a hacer
    escribir " ",x sin bajar;
  finpara
  escribir "!";
finfuncion
Proceso PruebaArreglos
  dimension a[1],b[2],c[3,3],d[1];
  n<-1;
  llenar(a,n);
  llenar(b,n);
  llenar(c,n);
  muestra("a",a);
  muestra("b",b);
  muestra("c",c);
  para cada x en d hacer
    escribir "ok";
  fin para
FinProceso