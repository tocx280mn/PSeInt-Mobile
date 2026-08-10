function r=comparar_cadenas(a,b)
	n1=length(a);
	n2=length(b);
	n=min(n1,n2);
	i=0;
	while i<n
		if (a(i)!=b(i))
			r=a(i)-b(i);
			break;
		end
		i=i+1;
	end
	if i==n
		r=n1-n2;
	end
end
function sin_titulo()
	a=11;
	b=2;
	c=true;
	d=false;
	e='Hola';
	f='Mundo';
	disp(a+b);
	disp(a*b);
	disp(a/b);
	disp(a-b);
	disp(a^b);
	disp(mod(a,b));
	disp(a==b);
	disp(a~=b);
	disp(a<b);
	disp(a>b);
	disp(a<=b);
	disp(a>=b);
	disp(c && d);
	disp(c || d);
	disp(~(c && d));
	disp([[e,' '],f]);
	disp(strcmp(e,f));
	disp(~strcmp(e,f));
	disp(comparar_cadenas(e,f)<0);
	disp(comparar_cadenas(e,f)>0);
	disp(comparar_cadenas(e,f)<=0);
	disp(comparar_cadenas(e,f)>=0);
end
