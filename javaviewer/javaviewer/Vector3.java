package javaviewer;

import java.lang.Math;

public class Vector3 {
	float x;
	float y; 
	float z;
	
	Vector3() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	
	Vector3(float xi, float yi, float zi) {
		x = xi;
		y = yi;
		z = zi;
	}
	
	public String toString() {
		Float xf = new Float(x);
		Float yf = new Float(y);
		Float zf = new Float(z);
		String retstr = "("+ xf.toString() +","+ yf.toString() +","+ zf.toString() +")";
		return(retstr);		
	}

	public float length() {
		return((float)Math.sqrt((double)(x*x + y*y + z*z)));
	}
	
	public void normalize() {
		float d = this.length();
		x /= d;
		y /= d;
		z /= d;
	}
	
	public void setArcballPos(int xm, int ym, int xs, int ys) {
		
		x = (2.0f*(float)xm - (float)xs) / (float)xs;
	    y = ((float)ys - 2.0f*(float)ym) / (float)ys;
	    z = 0.0f;
	    
	    float d = this.length();
	    
	    if (d > 1.0) {
	    	d = 1.0f;
	    }
	   
	    z = (float)Math.sqrt((double)(1.001f - d*d));
	    this.normalize();	
	
	}
	
	public static Vector3 sub(Vector3 a, Vector3 b) {
		Vector3 c = new Vector3(a.x-b.x, a.y-b.y, a.z -b.z);
		return(c);
	}
	
	public static Vector3 cross(Vector3 a, Vector3 b) {
		Vector3 c = new Vector3();
		c.x = a.y*b.z - a.z*b.y;
		c.y = a.z*b.x - a.x*b.z;
		c.z = a.x*b.y - a.y*b.x;
		return(c);
	}
}
