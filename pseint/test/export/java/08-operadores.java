import java.io.*;
import java.math.*;
public class sin_titulo {
	public static void main(String args[]) {
		double a;
		double b;
		boolean c;
		boolean d;
		String e;
		String f;
		a = 11;
		b = 2;
		c = true;
		d = false;
		e = "Hola";
		f = "Mundo";
		System.out.println(a+b);
		System.out.println(a*b);
		System.out.println(a/b);
		System.out.println(a-b);
		System.out.println(Math.pow(a, b));
		System.out.println(a%b);
		System.out.println(a==b);
		System.out.println(a!=b);
		System.out.println(a<b);
		System.out.println(a>b);
		System.out.println(a<=b);
		System.out.println(a>=b);
		System.out.println(c && d);
		System.out.println(c || d);
		System.out.println(!(c && d));
		System.out.println(e+" "+f);
		System.out.println(e.equals(f));
		System.out.println(!e.equals(f));
		System.out.println(e.compareTo(f)<0);
		System.out.println(e.compareTo(f)>0);
		System.out.println(e.compareTo(f)<=0);
		System.out.println(e.compareTo(f)>=0);
	}
}
