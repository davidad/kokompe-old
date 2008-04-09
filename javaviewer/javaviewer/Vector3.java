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

	public void set(float ix, float iy, float iz)  {
		x = ix;
		y = iy;
		z = iz;
	}

	public void setToMin() {
		x = Float.MIN_VALUE;
		y = Float.MIN_VALUE;
		z = Float.MIN_VALUE;
	}

	public void setToMax() {
		x = Float.MAX_VALUE;
		y = Float.MAX_VALUE;
		z = Float.MAX_VALUE;
	}

	public void setMinimumComponents(Vector3 a) {
		if (a.x < x)
			x = a.x;
		if (a.y < y)
			y = a.y;
		if (a.z < z)
			z = a.z;
	}

	public void setMaximumComponents(Vector3 a) {
		if (a.x > x)
			x = a.x;
		if (a.y > y)
			y = a.y;
		if (a.z > z)
			z = a.z;
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

	public void clear() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	public static Vector3 sub(Vector3 a, Vector3 b) {
		Vector3 c = new Vector3(a.x-b.x, a.y-b.y, a.z -b.z);
		return(c);
	}

	public static Vector3 add(Vector3 a, Vector3 b) {
		Vector3 c = new Vector3(a.x+b.x, a.y+b.y, a.z + b.z);
		return(c);
	}

	public static Vector3 mul(Vector3 a, float b) {
		Vector3 c = new Vector3(a.x*b, a.y*b, a.z*b);
		return(c);
	}

	public static float computeScaleFactor(Vector3 minBox, Vector3 maxBox, Vector3 boxCenter) {
		Vector3 vectorScaleFactor = new Vector3();

		vectorScaleFactor.x = Math.max(Math.abs(maxBox.x-boxCenter.x), Math.abs(minBox.x-boxCenter.x));
		vectorScaleFactor.y = Math.max(Math.abs(maxBox.y-boxCenter.y), Math.abs(minBox.y-boxCenter.y));
		vectorScaleFactor.z = Math.max(Math.abs(maxBox.z-boxCenter.z), Math.abs(minBox.z-boxCenter.z));

	    return(vectorScaleFactor.maxComponent());
	}

	public float maxComponent() {
		return(Math.max(Math.max(x, y), z));
	}

	public static Vector3 cross(Vector3 a, Vector3 b) {
		Vector3 c = new Vector3();
		c.x = a.y*b.z - a.z*b.y;
		c.y = a.z*b.x - a.x*b.z;
		c.z = a.x*b.y - a.y*b.x;
		return(c);
	}

	public static Vector3 transform(float[] m, Vector3 a) {
		Vector3 b = new Vector3();
		//   b.x   0  4  8   12     a.x
		//   b.y   1  5  9   13     a.y
		//   b.z   2  6  10  14     a.z
		//         3  7  11  15

		b.x = m[0]*a.x + m[4]*a.y + m[8]*a.z + m[12];
		b.y = m[1]*a.x + m[5]*a.y + m[9]*a.z + m[13];
		b.z = m[2]*a.x + m[6]*a.y + m[10]*a.z + m[14];

		return(b);
	}

	// given two extreme corners of a bounding box,
	// construct an array of all eight corners
	public static Vector3[] constructCorners(Vector3 minBox, Vector3 maxBox) {
		Vector3[] corners = new Vector3[8];

		corners[0] = new Vector3(minBox.x, minBox.y, minBox.z);
		corners[1] = new Vector3(minBox.x, minBox.y, maxBox.z);
		corners[2] = new Vector3(minBox.x, maxBox.y, minBox.z);
		corners[3] = new Vector3(minBox.x, maxBox.y, maxBox.z);
		corners[4] = new Vector3(maxBox.x, minBox.y, minBox.z);
		corners[5] = new Vector3(maxBox.x, minBox.y, maxBox.z);
		corners[6] = new Vector3(maxBox.x, maxBox.y, minBox.z);
		corners[7] = new Vector3(maxBox.x, maxBox.y, maxBox.z);

		return(corners);
	}

}

