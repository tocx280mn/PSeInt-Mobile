Program sin_titulo;
Var
	a: Real;
	b: Real;
	c: Boolean;
	d: Boolean;
	e: String;
	f: String;
Begin
	a := 11;
	b := 2;
	c := True;
	d := False;
	e := 'Hola';
	f := 'Mundo';
	WriteLn(a+b);
	WriteLn(a*b);
	WriteLn(a/b);
	WriteLn(a-b);
	WriteLn(Power(a,b));
	WriteLn(a Mod b);
	WriteLn(a=b);
	WriteLn(a<>b);
	WriteLn(a<b);
	WriteLn(a>b);
	WriteLn(a<=b);
	WriteLn(a>=b);
	WriteLn(c And d);
	WriteLn(c Or d);
	WriteLn(Not (c And d));
	WriteLn(e+' '+f);
	WriteLn(e=f);
	WriteLn(e<>f);
	WriteLn(e<f);
	WriteLn(e>f);
	WriteLn(e<=f);
	WriteLn(e>=f);
End.
