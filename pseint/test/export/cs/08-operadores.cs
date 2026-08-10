using System;
namespace PSeInt {
	class sin_titulo {
		static void Main(string[] args) {
			double a;
			double b;
			bool c;
			bool d;
			string e;
			string f;
			a = 11;
			b = 2;
			c = true;
			d = false;
			e = "Hola";
			f = "Mundo";
			Console.WriteLine(a+b);
			Console.WriteLine(a*b);
			Console.WriteLine(a/b);
			Console.WriteLine(a-b);
			Console.WriteLine(Math.Pow(a, b));
			Console.WriteLine(a%b);
			Console.WriteLine(a==b);
			Console.WriteLine(a!=b);
			Console.WriteLine(a<b);
			Console.WriteLine(a>b);
			Console.WriteLine(a<=b);
			Console.WriteLine(a>=b);
			Console.WriteLine(c && d);
			Console.WriteLine(c || d);
			Console.WriteLine(!(c && d));
			Console.WriteLine(e+" "+f);
			Console.WriteLine(e.Equals(f));
			Console.WriteLine(!e.Equals(f));
			Console.WriteLine(e.CompareTo(f)<0);
			Console.WriteLine(e.CompareTo(f)>0);
			Console.WriteLine(e.CompareTo(f)<=0);
			Console.WriteLine(e.CompareTo(f)>=0);
		}
	}
}
