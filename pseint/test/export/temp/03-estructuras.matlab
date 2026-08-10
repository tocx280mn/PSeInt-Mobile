function sin_titulo()
	c=0;
	while c<=10
		c=c+1;
		disp(c);
	end
	c=0;
	aux_logica_1=true;
	while aux_logica_1
		c=c+1;
		disp(c);
		aux_logica_1=c~=10;
	end
	c=0;
	aux_logica_1=true;
	while aux_logica_1
		c=c+1;
		disp(c);
		aux_logica_1=c<10;
	end
	if c==10
		disp('Si');
	end
	if c==10
		disp('Si');
	else
		disp('No');
	end
end
