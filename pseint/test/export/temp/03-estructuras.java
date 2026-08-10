import java.io.*;
public class sin_titulo {
	public static void main(String args[]) {
		double c;
		c = 0;
		while (c<=10) {
			c = c+1;
			System.out.println(c);
		}
		c = 0;
		do {
			c = c+1;
			System.out.println(c);
		} while (c!=10);
		c = 0;
		do {
			c = c+1;
			System.out.println(c);
		} while (c<10);
		if (c==10) {
			System.out.println("Si");
		}
		if (c==10) {
			System.out.println("Si");
		} else {
			System.out.println("No");
		}
	}
}
