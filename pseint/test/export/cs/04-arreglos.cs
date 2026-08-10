using System;
namespace PSeInt {
	class sin_titulo {
		static void Main(string[] args) {
			int n;
			string[] a = new string[10];
			string[,] b = new string[20,30];
			string[] c = new string[40];
			string[,] d = new string[50,5];
			n = int.Parse(Console.ReadLine());
			string[] e = new string[n];
			a[0] = Console.ReadLine();
			a[9] = a[0];
			Console.WriteLine(b[9,9]);
		}
	}
}
