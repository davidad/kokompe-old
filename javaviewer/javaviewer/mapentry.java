package javaviewer;
import java.net.*;

public class mapentry {
	public int displayList;
	public long urlLastModified;
	public URL partURL;
	public boolean newData;
	public float[] minBox;
	public float[] maxBox;

	mapentry() {
		displayList = 0;
		urlLastModified = 0;
		newData = false;

		minBox = new float[3];
	    maxBox = new float[3];

	      minBox[0] = Float.POSITIVE_INFINITY;
	      minBox[1] = Float.POSITIVE_INFINITY;
	      minBox[2] = Float.POSITIVE_INFINITY;
	      maxBox[0] = Float.NEGATIVE_INFINITY;
	      maxBox[1] = Float.NEGATIVE_INFINITY;
	      maxBox[2] = Float.NEGATIVE_INFINITY;
	}
}
