32
2 1 1 
* Este ejemplo es para observar la estructura para/for y su 
2 2 1 
alternativa para arreglos para cada/for each
2 3 1 
* La estructura para cada itera por todos los elementos de un
2 4 1 
arreglo (puede ser multidimensional), generando en cada iteración
2 5 1 
un alias al elemento del arreglo con el nombre que se indique luegoe 
2 6 1 
de la palabra cada. Puede que el lenguaje no tenga esta estructura. 
2 7 1 
Si hay una forma de simularla, puede utilizarla, sino alcarar que no
2 8 1 
es posible. Por ejemplo, C++99/03 no tiene esta estructura de control,
2 9 1 
pero dado que los elementos de una matriz están contiguos en memoria
2 10 1 
se pueden recorrer con un puntero de una forma generalizable.
2 11 1 
* Si el for del lenguaje tiene partes opcionales, puede omitirlas donde
2 12 1 
no sean estrictamente necesarias (como el paso en pseint cuando es 1).
3 14 1 PROCESO SIN_TITULO
1 31

sin_titulo

8 16 1 DIMENSION A(10);
1
a
10
26 18 1 PARA I DESDE 1 HASTA 10 HACER
16
i
1

10
6 19 1 A(I)<-I*10;
a[i]
i*10
28 20 1 FINPARA
27 22 1 PARA CADA ELEMENTO DE A HACER
19
elemento
a
7 23 1 ESCRIBIR ELEMENTO;
1 1
elemento
28 24 1 FINPARA
8 26 1 DIMENSION B(3,6);
1
b
3,6
6 27 1 C<-0;
c
0
27 28 1 PARA CADA X DE B HACER
25
x
b
6 29 1 C<-C+1;
c
c+1
6 30 1 X<-C;
x
c
28 31 1 FINPARA
26 33 1 PARA I DESDE 3 HASTA 1 CON PASO -1 HACER
30
i
3
-1
1
26 34 1 PARA J DESDE 1 HASTA 5 CON PASO 2 HACER
29
j
1
2
5
7 35 1 ESCRIBIR B(I,J);
1 1
b[i,j]
28 36 1 FINPARA
28 37 1 FINPARA
4 39 1 FINPROCESO
1 sin_titulo
