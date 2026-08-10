function sin_titulo()
	a=zeros(10,);
	for i=1:10
		a(i)=i*10;
	end
	for aux_index_1=1:size(a,1)
		disp(a(aux_index_1));
	end
	b=zeros(3,6,);
	c=0;
	for aux_index_1=1:size(b,1)
		for aux_index_2=1:size(b,2)
			c=c+1;
			b(aux_index_1,aux_index_2)=c;
		end
	end
	for i=3:-1:1
		for j=1:2:5
			disp(b(i,j));
		end
	end
end
