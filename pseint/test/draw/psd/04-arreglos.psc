15
2 1 1 
* Este ejemplo es para observar cómo trabajar con arreglos
2 2 1 
* El perfil flexible de pseint trabaja con índices en base 1, las
2 3 1 
asignaciones son para observar si los ínidices van en base 1 o 0.
2 4 1 
* Si el lenguaje no permite utilizar variables para dimensionar un
2 5 1 
arreglo, aclarar.
3 7 1 PROCESO SIN_TITULO
1 14

sin_titulo

8 8 1 DIMENSION A(10);
1
a
10
8 9 1 DIMENSION B(20,30);
1
b
20,30
8 10 1 DIMENSION C(40),D(50,5);
2
c
40
d
50,5
5 11 1 LEER N;
1
n
8 12 1 DIMENSION E(N);
1
e
n
5 13 1 LEER A(1);
1
a[1]
6 14 1 A(10)<-A(1);
a[10]
a[1]
7 15 1 ESCRIBIR B(10,10);
1 1
b[10,10]
4 16 1 FINPROCESO
1 sin_titulo
