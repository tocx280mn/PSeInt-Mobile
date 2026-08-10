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
			for (int aux_index_0=0; aux_index_0<10; ++aux_index_0) {
				Console.WriteLine(a[aux_index_0]);
			}
			double[,] b = new double[3,6];
			c = 0;
			for (int aux_index_0=0; aux_index_0<3; ++aux_index_0) {
				for (int aux_index_1=0; aux_index_1<6; ++aux_index_1) {
					c = c+1;
					b[aux_index_0,aux_index_1] = c;
				}
			}
			for (i=3; i>=1; --i) {
				for (j=1; j<=5; j+=2) {
					Console.WriteLine(b[i-1,j-1]);
				}
			}
		}
	}
}
