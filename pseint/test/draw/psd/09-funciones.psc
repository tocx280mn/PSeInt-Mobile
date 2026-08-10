64
2 1 1 
* Este ejemplo es para ver cuales son las funciones equivalentes
2 2 1 
a las predefinidas en PSeInt.
2 3 1 
* Indicar en cada caso si hay que agregar algún #include, import
2 4 1 
o similar a la cabecera del programa
2 5 1 
* Si una función no existe, pero es fácil de implementar, ofrecer
2 6 1 
la implementación para introducir cuando sea necesario. Por ejemplo,
2 7 1 
en C++ no hay función para pasar toda una cadena a mayúsculas pero
2 8 1 
se puede implementar fácilmente aplicando toupper (que opera sobre
2 9 1 
un única caracter) a cada caracter de la cadena mediante un ciclo
2 10 1 
for.
2 11 1 
* Para las funciones trigonométricas, indicar si el argumento se
2 12 1 
recibe en grados o radianes.
2 13 1 
* En algunos casos pueden reemplazarse por expresiones. Por ejemplo,
2 14 1 
en c++ puedo obtener el truncado casteando a entero, o puedo concatenar
2 15 1 
strings con el operador +, o necesito aplicarle un mod al resultado de
2 16 1 
rand para tener un valor máxim como en la función Azar. Estos cambios
2 17 1 
no ameritan declarar una nueva función, ya que pueden hacerse in-place
2 18 1 
sin generar mucho ruido.
2 19 1 
* Ninguna de las funciones de pseint modifican el argumento (el pasaje
2 20 1 
de parametros es siempre por copia).
3 22 1 PROCESO SIN_TITULO
1 63

sin_titulo

6 23 1 X<-1.5;
x
1.5
7 24 1 ESCRIBIR RC(X);
1 1
rc(x)
2 24 2 
raiz cuadrada
7 25 1 ESCRIBIR ABS(X);
1 1
abs(x)
2 25 2 
valor absoluto
7 26 1 ESCRIBIR LN(X);
1 1
ln(x)
2 26 2 
logaritmo natural
7 27 1 ESCRIBIR EXP(X);
1 1
exp(x)
2 27 2 
exponencial, e^x
7 28 1 ESCRIBIR SEN(X);
1 1
sen(x)
2 28 2 
seno
7 29 1 ESCRIBIR COS(X);
1 1
cos(x)
2 29 2 
coseno
7 30 1 ESCRIBIR TAN(X);
1 1
tan(x)
2 30 2 
tangente
7 31 1 ESCRIBIR ASEN(X);
1 1
asen(x)
2 31 2 
arco-seno
7 32 1 ESCRIBIR ACOS(X);
1 1
acos(x)
2 32 2 
arco-coseno
7 33 1 ESCRIBIR ATAN(X);
1 1
atan(x)
2 33 2 
arco-tangente
7 34 1 ESCRIBIR REDON(X);
1 1
redon(x)
2 34 2 
redondo a entero
7 35 1 ESCRIBIR TRUNC(X);
1 1
trunc(x)
2 35 2 
truncamiento a entero
7 36 1 ESCRIBIR AZAR(15);
1 1
azar(15)
2 36 2 
numero al azar menor a 15 (entre 0 y 14 inclusive)
6 37 1 S<-'Hola';
s
'Hola'
7 38 1 ESCRIBIR LONGITUD(S);
1 1
Longitud(s)
2 38 2 
cantidad de letras
7 39 1 ESCRIBIR MINUSCULAS(S);
1 1
Minusculas(s)
2 39 2 
retorna una copia toda en mayusculas
7 40 1 ESCRIBIR MAYUSCULAS(S);
1 1
Mayusculas(s)
2 40 2 
retorna una copia toda en minusculas
7 41 1 ESCRIBIR SUBCADENA(S,2,3);
1 1
SubCadena(s,2,3)
2 41 2 
retorna la cadena que va desde la posicion 2 a la 3 (posiciones en base 1): "ol"
7 42 1 ESCRIBIR CONCATENAR(S,' Mundo');
1 1
concatenar(s,' Mundo')
7 43 1 ESCRIBIR CONCATENAR('Mundo ',S);
1 1
concatenar('Mundo ',s)
7 44 1 ESCRIBIR CONVERTIRANUMERO('15.5');
1 1
ConvertirANumero('15.5')
2 44 2 
retorna un real (15.5)
7 45 1 ESCRIBIR CONVERTIRATEXTO(15.5);
1 1
ConvertirATexto(15.5)
2 45 2 
retorna una cadena ("15.5")
4 46 1 FINPROCESO
1 sin_titulo
