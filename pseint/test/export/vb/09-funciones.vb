Module SIN_TITULO
	Sub Main()
		Dim s As String
		Dim x As Double
		x = 1.5
		Console.WriteLine(Math.Sqrt(x))
		Console.WriteLine(Math.Abs(x))
		Console.WriteLine(Math.Log(x))
		Console.WriteLine(Math.Exp(x))
		Console.WriteLine(Math.Sin(x))
		Console.WriteLine(Math.Cos(x))
		Console.WriteLine(Math.Tan(x))
		Console.WriteLine(Math.Asin(x))
		Console.WriteLine(Math.Acos(x))
		Console.WriteLine(Math.Atan(x))
		Console.WriteLine(Math.Round(x))
		Console.WriteLine(Math.Truncate(x))
		Console.WriteLine(New Random().Next(15))
		s = "Hola"
		Console.WriteLine(s.Length())
		Console.WriteLine(s.ToUpper())
		Console.WriteLine(s.ToLower())
		Console.WriteLine(Mid(s,1,2))
		Console.WriteLine(s & " Mundo")
		Console.WriteLine("Mundo " & s)
		Console.WriteLine(CDbl("15.5"))
		Console.WriteLine(CStr(15.5))
	End Sub
End Module
