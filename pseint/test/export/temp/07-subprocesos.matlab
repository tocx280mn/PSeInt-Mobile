function sin_titulo()
	disp(funciondoble(5));
	noretornanada(3, 9);
	c=0;
	porreferencia(c);
	disp(c);
	a=zeros(10,);
	b=zeros(3,4,);
	recibevector(a);
	recibematriz(b);
end
function a = funciondoble(b)
	a=2*b;
end
function noretornanada(a, b)
	disp(a+b);
end
function porreferencia(b)
	b=7;
end
function recibevector(v)
	v(1)=input('');
end
function recibematriz(m)
	m(1,1)=input('');
end
