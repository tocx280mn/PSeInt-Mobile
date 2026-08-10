import java.io.*;
public class sin_titulo {
	public static void main(String args[]) throws IOException {
		BufferedReader bufEntrada = new BufferedReader(new InputStreamReader(System.in));
		String a[];
		String b[][];
		String c[];
		String d[][];
		String e[];
		int n;
		a = new String[10];
		b = new String[20][30];
		c = new String[40];
		d = new String[50][5];
		n = Integer.parseInt(bufEntrada.readLine());
		e = new String[n];
		a[0] = bufEntrada.readLine();
		a[9] = a[0];
		System.out.println(b[9][9]);
	}
}
