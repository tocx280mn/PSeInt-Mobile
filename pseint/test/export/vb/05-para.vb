Module SIN_TITULO
	Sub Main()
		Dim c As Double
		Dim i As Integer
		Dim j As Integer
		Dim a(10) As Double
		For i=1 To 10
			a(i) = i*10
		Next i
		For Each elemento In a
			Console.WriteLine(elemento)
		Next
		Dim b(3,6) As Double
		c = 0
		For Each x In b
			c = c+1
			x = c
		Next
		For i=3 To 1 Step -1
			For j=1 To 5 Step 2
				Console.WriteLine(b(i,j))
			Next j
		Next i
	End Sub
End Module
