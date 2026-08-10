Program sin_titulo;
Uses
	Math,
	Sysutils;
Var
	s: String;
	x: Real;
Begin
	Randomize;
	x := 1.5;
	WriteLn(Sqrt(x));
	WriteLn(Abs(x));
	WriteLn(Ln(x));
	WriteLn(Exp(x));
	WriteLn(Sin(x));
	WriteLn(Cos(x));
	WriteLn(Tan(x));
	WriteLn(Arcsin(x));
	WriteLn(Arccos(x));
	WriteLn(Arctan(x));
	WriteLn(Round(x));
	WriteLn(Trunc(x));
	WriteLn(Random(15));
	s := 'Hola';
	WriteLn(Length(s));
	WriteLn(Lowercase(s));
	WriteLn(Upcase(s));
	WriteLn(Copy(s,2,3-1));
	WriteLn(s+' Mundo');
	WriteLn('Mundo '+s);
	WriteLn(StrToFloat('15.5'));
	WriteLn(FloatToStr(15.5));
End.
