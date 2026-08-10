// * Este ejemplo es para observar cómo trabajar con arreglos
// * El perfil flexible de pseint trabaja con índices en base 1, las
// asignaciones son para observar si los ínidices van en base 1 o 0.
// * Si el lenguaje no permite utilizar variables para dimensionar un
// arreglo, aclarar.

Proceso sin_titulo
	Dimension a[10];
	Dimension b[20,30];
	Dimension c[40],d[50,5];
	Leer n;
	Dimension e[n];
	Leer a[1];
	a[10]<-a[1];
	Escribir b[10,10];
FinProceso
