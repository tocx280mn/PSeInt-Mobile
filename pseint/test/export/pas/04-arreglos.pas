Program sin_titulo;
Type
	SIN_TIPO = Real;
Const
	ARREGLO_MAX = 100;
Var
	a: Array [1..10] Of SIN_TIPO;
	b: Array [1..20,1..30] Of SIN_TIPO;
	c: Array [1..40] Of SIN_TIPO;
	d: Array [1..50,1..5] Of SIN_TIPO;
	e: Array [1..ARREGLO_MAX] Of SIN_TIPO;
	n: Integer;
Begin
	ReadLn(n);
	ReadLn(a[1]);
	a[10] := a[1];
	WriteLn(b[10,10]);
End.
