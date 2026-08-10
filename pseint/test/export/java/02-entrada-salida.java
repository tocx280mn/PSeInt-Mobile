import java.io.*;
public class sin_titulo {
	public static void main(String args[]) throws IOException {
		BufferedReader bufEntrada = new BufferedReader(new InputStreamReader(System.in));
		int a;
		double b;
		String c;
		boolean d;
		a = Integer.parseInt(bufEntrada.readLine());
		b = Double.parseDouble(bufEntrada.readLine());
		c = bufEntrada.readLine();
		d = Boolean.parseBoolean(bufEntrada.readLine());
		System.out.println(a);
		System.out.println(b);
		System.out.println(c);
		System.out.println(d);
		a = Integer.parseInt(bufEntrada.readLine());
		b = Double.parseDouble(bufEntrada.readLine());
		c = bufEntrada.readLine();
		d = Boolean.parseBoolean(bufEntrada.readLine());
		System.out.println(a+b+c+d);
		System.out.print("Esta linea no lleva enter al final");
	}
}
