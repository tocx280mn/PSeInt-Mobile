Program sin_titulo;
Var
	a: Array [1..10] Of Real;
	aux_para_1: Integer;
	aux_para_2: Integer;
	b: Array [1..3,1..6] Of Real;
	c: Real;
	i: Integer;
	j: Integer;
Begin
	For i:=1 To 10 Do Begin
		a[i] := i*10;
	End;
	For aux_para_1:=Low(a) To High(a) Do Begin
		WriteLn(a[aux_para_1]);
	End;
	c := 0;
	For aux_para_1:=Low(b) To High(b) Do Begin
		For aux_para_2:=Low(b[aux_para_1]) To High(b[aux_para_1]) Do Begin
			c := c+1;
			b[aux_para_1,aux_para_2] := c;
		End;
	End;
	For i:=3 DownTo 1 Do Begin
		For aux_para_1:=0 To ((5-1) Div 2) Do Begin
			j := 1+aux_para_1*2;
			WriteLn(b[i,j]);
		End;
	End;
End.
