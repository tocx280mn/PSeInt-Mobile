Program sin_titulo;
Type
	SIN_TIPO = Real;
	TIPO_VECTOR_10 = Array [1..10] Of SIN_TIPO;
	TIPO_MATRIZ_3X4 = Array [1..3, 1..4] Of SIN_TIPO;
Var
	a: Array [1..10] Of SIN_TIPO;
	b: Array [1..3,1..4] Of SIN_TIPO;
	c: Real;
Function funciondoble(b: Real): Real;
Begin
	funciondoble := 2*b;
End;
Procedure noretornanada(a: SIN_TIPO; b: SIN_TIPO);
Begin
	WriteLn(a+b);
End;
Procedure porreferencia(Var b: Real);
Begin
	b := 7;
End;
Procedure recibevector(Var v: TIPO_VECTOR_10);
Begin
	ReadLn(v[1]);
End;
Procedure recibematriz(Var m: TIPO_MATRIZ_3X4);
Begin
	ReadLn(m[1,1]);
End;
Begin
	WriteLn(funciondoble(5));
	noretornanada(3, 9);
	c := 0;
	porreferencia(c);
	WriteLn(c);
	recibevector(a);
	recibematriz(b);
End.
