Module SIN_TITULO
	Sub Main()
		Dim c As Double
		c = Double.Parse(Console.ReadLine())
		Select Case c
		Case 1
			Console.WriteLine("1")
		Case 3, 5, 9
			Console.WriteLine("3, 5 o 9")
		Case 7
			Console.WriteLine("7")
		Case Else
			Console.WriteLine("ni 1, ni 3, ni 5, ni 7, ni 9")
		End Select
	End Sub
End Module
