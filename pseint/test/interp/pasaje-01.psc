subproceso lala(a por valor, b por referencia, c,d)
	a<-a+1;
	b<-b+2;
	c<-c+3;
	d[2]<-7;
FinSubProceso
Proceso sin_titulo
	xx<-10;
	yy<-20;
	zz<-30;
	Dimension ww[10];
	lala(xx,yy,zz,ww);
	escribir xx;
	escribir yy;
	escribir zz;
	escribir ww[2];
FinProceso
