import java.io.*;
public class sin_titulo {
	BufferedReader bufEntrada = new BufferedReader(new InputStreamReader(System.in));
	public static void main(String args[]) {
		String a[];
		String b[][];
		double c;
		System.out.println(funciondoble(5));
		noretornanada(3, 9);
		c = 0;
		porreferencia(c);
		System.out.println(c);
		a = new String[10];
		b = new String[3][4];
		recibevector(a);
		recibematriz(b);
	}
	public static double funciondoble(double b) {
		double a;
		a = 2*b;
		return a;
	}
	public static void noretornanada(String a, String b) {
		System.out.println(a+b);
	}
	public static void porreferencia(double b) {
		b = 7;
	}
	public static void recibevector(String v[]) throws IOException {
		v[0] = bufEntrada.readLine();
	}
	public static void recibematriz(String m[][]) throws IOException {
		m[0][0] = bufEntrada.readLine();
	}
}
