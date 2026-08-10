funcion fc1(b por copia)
    Escribir b
finfuncion

funcion a<-fc2(b por copia)
    a<-b
finfuncion

funcion fr1(b por referencia)
    Escribir b
finfuncion

funcion a<-fr2(b por referencia)
    b<-42
    a<-b
finfuncion

algoritmo prueba
    x<-1
    fc1(x) // ok
    fr1(x) // ok
    Escribir fc2(x) // ok
    Escribir fr2(x) // ok
    fc1(42) // ok
    fr1(42) // error
    Escribir fc2(42) // ok
    Escribir fr2(42) // error
finalgoritmo
