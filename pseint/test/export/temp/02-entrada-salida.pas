Program sin_titulo;
Var
	a: Integer;
	aux_leer_logica: String;
	b: Real;
	c: String;
	d: Boolean;
Begin
	ReadLn(a);
	ReadLn(b);
	ReadLn(c);
	ReadLn(aux_leer_logica);
	d := (lowercase(aux_leer_logica)='verdadero');
	WriteLn(a);
	WriteLn(b);
	WriteLn(c);
	WriteLn(d);
	ReadLn(a,b,c);
	ReadLn(aux_leer_logica);
	d := (lowercase(aux_leer_logica)='verdadero');
	WriteLn(a,b,c,d);
	Write('Esta linea no lleva enter al final');
End.
