18
2 1 1 
* Este ejemplo es para observar la estructura segun:
2 2 1 
* Aclarar de qué tipo puede ser o no ser la variable de control
2 3 1 
(por ejemplo, en C++ c solo podría ser numérica)
2 4 1 
* Aclarar si es obligatorio o no colocar la sección por defecto
2 5 1 
(la correspondiente al "De Otro Modo:").
3 7 1 PROCESO SIN_TITULO
1 17

sin_titulo

5 8 1 LEER C;
1
c
22 9 1 SEGUN C HACER
c
16 4 8 10 12 14
23 10 1 1:
0 1
1
7 11 1 ESCRIBIR '1';
1 1
'1'
23 12 1 3,5,9:
0 3
3
5
9
7 13 1 ESCRIBIR '3, 5 o 9';
1 1
'3, 5 o 9'
23 14 1 7:
0 1
7
7 15 1 ESCRIBIR '7';
1 1
'7'
24 16 1 DE OTRO MODO:
7 17 1 ESCRIBIR 'ni 1, ni 3, ni 5, ni 7, ni 9';
1 1
'ni 1, ni 3, ni 5, ni 7, ni 9'
25 18 1 FINSEGUN
4 19 1 FINPROCESO
1 sin_titulo
