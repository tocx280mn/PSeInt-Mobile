33
2 1 1 
* Este ejemplo es para observar cómo se realiza la entrada y
2 2 1 
salida de cada tipo.
2 3 1 
* Si hay varios mecanismos y alguno es común a todos los tipos
2 4 1 
se prefiere dicho mecanismo. Por ejemplo, en C++ los string se 
2 5 1 
pueden leer con cin>>s o getline(cin,s). Ambos generan problemas
2 6 1 
en casos particulares, pero siendo ambos imperfectos se prefiere
2 7 1 
el primero porque funciona también y sin problemas para los
2 8 1 
demás tipos. 
2 9 1 
* En caso de haber problemas o limitaciones es las lecturas, 
2 10 1 
aclarar cuales son y cuando se presentan para que el código
2 11 1 
generado al exportar incluya esta aclaración como comentario.
3 13 1 PROCESO SIN_TITULO
1 32

sin_titulo

9 14 1 DEFINIR A COMO ENTERO;
1
a
0 1 0 1
9 15 1 DEFINIR B COMO REAL;
1
b
0 1 0 0
9 16 1 DEFINIR C COMO CADENA;
1
c
0 0 1 0
9 17 1 DEFINIR D COMO LOGICO;
1
d
1 0 0 0
2 18 1 
leer de a uno
5 19 1 LEER A;
1
a
5 20 1 LEER B;
1
b
5 21 1 LEER C;
1
c
5 22 1 LEER D;
1
d
2 23 1 
escribir de a uno
7 24 1 ESCRIBIR A;
1 1
a
7 25 1 ESCRIBIR B;
1 1
b
7 26 1 ESCRIBIR C;
1 1
c
7 27 1 ESCRIBIR D;
1 1
d
2 28 1 
lecturas múltiples
5 29 1 LEER A,B,C,D;
4
a
b
c
d
2 30 1 
escrituras múltiples
7 31 1 ESCRIBIR A,B,C,D;
1 4
a
b
c
d
2 32 1 
escritura sin salto de linea al final
7 33 1 ESCRIBIR SIN SALTAR 'Esta linea no lleva enter al final';
0 1
'Esta linea no lleva enter al final'
4 34 1 FINPROCESO
1 sin_titulo
