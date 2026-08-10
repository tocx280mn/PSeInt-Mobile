36
2 1 1 
* Este ejemplo es para ver la forma de declarar y utilizar
2 2 1 
subprocesos/funciones.
2 3 1 
* Aclarar si hay diferencia entre subprocesos/funciones que no
2 4 1 
retornen nada, y los que sí retornen algún resultado.
2 5 1 
* Las dos primeras reciben argumentos por copia, las tres ultimas
2 6 1 
por referencia (en PSeInt, cuando no se indica se utiliza por copia,
2 7 1 
menos los arreglos que siempre pasan por referencia)
2 8 1 
* Indicar las consideraciones especiales que se deban tener en cuenta
2 9 1 
al pasar arreglos.
2 10 1 
* Si es necesario hacer fordware declarations de las funciones
2 11 1 
cuando no se implementan en orden, indicarlo y ejemplificarlo.
3 13 1 PROCESO SIN_TITULO
1 20

sin_titulo

7 15 1 ESCRIBIR FUNCIONDOBLE(5);
1 1
FuncionDoble(5)
13 17 1 NORETORNANADA(3,9);
NoRetornaNada
(3,9)
6 19 1 C<-0;
c
0
13 20 1 PORREFERENCIA(C);
PorReferencia
(c)
7 21 1 ESCRIBIR C;
1 1
c
8 23 1 DIMENSION A(10),B(3,4);
2
a
10
b
3,4
13 24 1 RECIBEVECTOR(A);
RecibeVector
(a)
13 25 1 RECIBEMATRIZ(B);
RecibeMatriz
(b)
4 27 1 FINPROCESO
1 sin_titulo
3 29 1 FUNCION A<-FUNCIONDOBLE(B)
0 23
a
FuncionDoble
(b)
6 30 1 A<-2*B;
a
2*b
4 31 1 FINFUNCION
0 FuncionDoble
3 33 1 SUBPROCESO NORETORNANADA(A,B)
0 26

NoRetornaNada
(a,b)
7 34 1 ESCRIBIR A+B;
1 1
a+b
4 35 1 FINSUBPROCESO
0 NoRetornaNada
3 37 1 SUBPROCESO PORREFERENCIA(B POR REFERENCIA)
0 29

PorReferencia
(b por referencia)
6 38 1 B<-7;
b
7
4 39 1 FINSUBPROCESO
0 PorReferencia
3 41 1 SUBPROCESO RECIBEVECTOR(V)
0 32

RecibeVector
(v)
5 42 1 LEER V(1);
1
v[1]
4 43 1 FINSUBPROCESO
0 RecibeVector
3 45 1 SUBPROCESO RECIBEMATRIZ(M)
0 35

RecibeMatriz
(m)
5 46 1 LEER M(1,1);
1
m[1,1]
4 47 1 FINSUBPROCESO
0 RecibeMatriz
