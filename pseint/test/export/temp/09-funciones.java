import java.io.*;
import java.math.*;
public class sin_titulo {
	public static void main(String args[]) {
		String s;
		double x;
		x = 1.5;
		System.out.println(Math.sqrt(x));
		System.out.println(Math.abs(x));
		System.out.println(Math.log(x));
		System.out.println(Math.exp(x));
		System.out.println(Math.sin(x));
		System.out.println(Math.cos(x));
		System.out.println(Math.tan(x));
		System.out.println(Math.asin(x));
		System.out.println(Math.acos(x));
		System.out.println(Math.atan(x));
		System.out.println(Math.round(x));
		System.out.println(Math.floor(x));
		System.out.println(Math.floor(Math.random()*15));
		s = "Hola";
		System.out.println(s.length());
		System.out.println(s.toLowerCase());
		System.out.println(s.toUpperCase());
		System.out.println(s.substring(1, 3));
		System.out.println(s.concat(" Mundo"));
		System.out.println("Mundo ".concat(s));
		System.out.println(String.valueOf("15.5"));
		System.out.println(Double.toString(15.5));
	}
}
