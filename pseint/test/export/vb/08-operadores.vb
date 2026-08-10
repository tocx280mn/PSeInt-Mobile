Module SIN_TITULO
	Sub Main()
		Dim a As Double
		Dim b As Double
		Dim c As Boolean
		Dim d As Boolean
		Dim e As String
		Dim f As String
		a = 11
		b = 2
		c = True
		d = False
		e = "Hola"
		f = "Mundo"
		Console.WriteLine(a+b)
		Console.WriteLine(a*b)
		Console.WriteLine(a/b)
		Console.WriteLine(a-b)
		Console.WriteLine(a^b)
		Console.WriteLine(a Mod b)
		Console.WriteLine(a=b)
		Console.WriteLine(a<>b)
		Console.WriteLine(a<b)
		Console.WriteLine(a>b)
		Console.WriteLine(a<=b)
		Console.WriteLine(a>=b)
		Console.WriteLine(c And d)
		Console.WriteLine(c Or d)
		Console.WriteLine(Not (c And d))
		Console.WriteLine(e&" "&f)
		Console.WriteLine(e.Equals(f))
		Console.WriteLine(Not e.Equals(f))
		Console.WriteLine(e.CompareTo(f)<0)
		Console.WriteLine(e.CompareTo(f)>0)
		Console.WriteLine(e.CompareTo(f)<=0)
		Console.WriteLine(e.CompareTo(f)>=0)
	End Sub
End Module
