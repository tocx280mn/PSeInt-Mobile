using System;
namespace PSeInt {
	class sin_titulo {
		static void Main(string[] args) {
			int c;
			c = int.Parse(Console.ReadLine());
			switch (c) {
			case 1:
				Console.WriteLine("1");
				break;
			case 3:
			case 5:
			case 9:
				Console.WriteLine("3, 5 o 9");
				break;
			case 7:
				Console.WriteLine("7");
				break;
			default:
				Console.WriteLine("ni 1, ni 3, ni 5, ni 7, ni 9");
			}
		}
	}
}
