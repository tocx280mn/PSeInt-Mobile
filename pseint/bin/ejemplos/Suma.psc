"ABRA ESTOS EJEMPLOS DESDE LA AYUDA ( MENU AYUDA -> EJEMPLOS ) de otra forma no podrá ejecutarlos"

// este es el ejemplo más simple de esta ayuda, 
// toma dos numeros, los suma y muestra el resultado

{Proceso} Suma

    {Definir A,B,C como Reales}{;}

    // para cargar un dato, se le muestra un mensaje al usuario
    // con la instrucción Escribir, y luego se lee el dato en 
    // una variable (A para el primero, B para el segundo) con 
    // la instrucción Leer

    Escribir "Ingrese el primer numero:"{;}
    Leer A{;}

    Escribir "Ingrese el segundo numero:"{;}
    Leer B{;}


    // ahora se calcula la suma y se guarda el resultado en la
    // variable C mediante la asignación (<-)
    
    C <- A+B{;}


    // finalmente, se muestra el resultado, precedido de un 
    // mensaje para avisar al usuario, todo en una sola
    // instrucción Escribir
    
    Escribir "El resultado es: ",C{;}

{FinProceso}
