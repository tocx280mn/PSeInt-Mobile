using System;
namespace PSeInt {
	class sin_titulo {
		static void Main(string[] args) {
			string s;
			double x;
			Random azar = new Random();
			x = 1.5;
			Console.WriteLine(Math.Sqrt(x));
			Console.WriteLine(Math.Abs(x));
			Console.WriteLine(Math.Log(x));
			Console.WriteLine(Math.Exp(x));
			Console.WriteLine(Math.Sin(x));
			Console.WriteLine(Math.Cos(x));
			Console.WriteLine(Math.Tan(x));
			Console.WriteLine(Math.Asin(x));
			Console.WriteLine(Math.Acos(x));
			Console.WriteLine(Math.Atan(x));
			Console.WriteLine(Math.Round(x));
			Console.WriteLine(Math.Truncate(x));
			Console.WriteLine(azar.Next(0, 15));
			s = "Hola";
			Console.WriteLine(s.Length);
			Console.WriteLine(s.ToLower());
			Console.WriteLine(s.ToUpper());
			Console.WriteLine(s.Substring(1, 3-1));
			Console.WriteLine(s+" Mundo");
			Console.WriteLine("Mundo "+s);
			Console.WriteLine(Convert.ToDouble("15.5"));
			Console.WriteLine(Convert.ToString(15.5));
		}
	}
}
