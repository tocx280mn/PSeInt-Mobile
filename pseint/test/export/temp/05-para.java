import java.io.*;
public class sin_titulo {
	public static void main(String args[]) {
		double a[];
		double b[][];
		double c;
		int i;
		int j;
		a = new double[10];
		for (i=1; i<=10; ++i) {
			a[i-1] = i*10;
		}
		for (double elemento:a) {
			System.out.println(elemento);
		}
		b = new double[3][6];
		c = 0;
		for (int aux_index_0=0; aux_index_0<b.length; aux_index_0++) {
			for (int aux_index_1=0; aux_index_1<b[aux_index_0].length; aux_index_1++) {
				c = c+1;
				b[aux_index_0][aux_index_1] = c;
			}
		}
		for (i=3; i>=1; --i) {
			for (j=1; j<=5; j+=2) {
				System.out.println(b[i-1][j-1]);
			}
		}
	}
}
