Program sin_titulo;
Var
	c: Real;
Begin
	c := 0;
	While c<=10 Do Begin
		c := c+1;
		WriteLn(c);
	End;
	c := 0;
	Repeat
		c := c+1;
		WriteLn(c);
	Until c=10;
	c := 0;
	Repeat
		c := c+1;
		WriteLn(c);
	Until c>=10;
	If c=10 Then Begin
		WriteLn('Si');
	End;
	If c=10 Then Begin
		WriteLn('Si');
	End
	Else Begin
		WriteLn('No');
	End;
End.
