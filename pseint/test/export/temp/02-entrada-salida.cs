using System;
namespace PSeInt {
	class sin_titulo {
		static void Main(string[] args) {
			int a;
			double b;
			string c;
			bool d;
			a = int.Parse(Console.ReadLine());
			b = Double.Parse(Console.ReadLine());
			c = Console.ReadLine();
			d = Boolean.Parse(Console.ReadLine());
			Console.WriteLine(a);
			Console.WriteLine(b);
			Console.WriteLine(c);
			Console.WriteLine(d);
			a = int.Parse(Console.ReadLine());
			b = Double.Parse(Console.ReadLine());
			c = Console.ReadLine();
			d = Boolean.Parse(Console.ReadLine());
			Console.WriteLine(a+b+c+d);
			Console.Write("Esta linea no lleva enter al final");
		}
	}
}
