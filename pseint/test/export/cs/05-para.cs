using System;
namespace PSeInt {
	class sin_titulo {
		static void Main(string[] args) {
			double c;
			int i;
			int j;
			double[] a = new double[10];
			for (i=1; i<=10; ++i) {
				a[i-1] = i*10;
			}
			foreach (int elemento in a) {
				Console.WriteLine(elemento);
			}
			double[,] b = new double[3,6];
			c = 0;
			foreach (int x in b) {
				c = c + 1;
				// No se puede asignar el valor de c en x ya que x es la variable de iteracion 
				//x = c;
			}
			for (i=3; i>=1; --i) {
				for (j=1; j<=5; j+=2) {
					Console.WriteLine(b[i-1,j-1]);
				}
			}
		}
	}
}
