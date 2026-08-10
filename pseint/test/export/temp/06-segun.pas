Program sin_titulo;
Var
	c: Integer;
Begin
	ReadLn(c);
	Case c Of
		1: Begin
			WriteLn('1');
		End;
		3, 5, 9: Begin
			WriteLn('3, 5 o 9');
		End;
		7: Begin
			WriteLn('7');
		End;
		Else Begin
			WriteLn('ni 1, ni 3, ni 5, ni 7, ni 9');
		End;
	End;
End.
