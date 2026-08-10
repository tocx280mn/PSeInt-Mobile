import java.io.*;
public class sin_titulo {
	public static void main(String args[]) throws IOException {
		BufferedReader bufEntrada = new BufferedReader(new InputStreamReader(System.in));
		int c;
		c = Integer.parseInt(bufEntrada.readLine());
		switch (c) {
		case 1:
			System.out.println("1");
			break;
		case 3:
		case 5:
		case 9:
			System.out.println("3, 5 o 9");
			break;
		case 7:
			System.out.println("7");
			break;
		default:
			System.out.println("ni 1, ni 3, ni 5, ni 7, ni 9");
		}
	}
}
