package javaviewer;

public class part {
	public String partURL;
	float[] modelviewMatrix;
	float[] color;

	part() {
		modelviewMatrix = new float[16];
		color = new float[3];
	}

	public String toString() {
		String outString = "partURL: " + partURL;
		return(outString);
	}
}
