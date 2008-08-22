package javaviewer;

import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.io.*;

import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.StringTokenizer;
import java.util.Timer;
import java.util.TimerTask;
import java.util.zip.*;

import java.awt.*;
import java.awt.event.*;

import javax.media.opengl.*;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;

import com.sun.opengl.util.*;


/*  Kokompe Java Viewer 0.2
 *  By Ara Knaian, 2008
 *
 *  Originally adapted from Gears demo by Brian Paul */

public class Viewer implements GLEventListener, MouseListener, MouseMotionListener, MouseWheelListener, KeyListener {
	static Frame frame;
	static String defaultURL;

	public static void main(String[] args) {

	String appname = System.getProperty("jnlp.title");
	if (appname == null) {
		appname = "KOKOMPE Viewer";
	}

	if (args.length >= 1) {
		defaultURL = args[0];
	}
	else {
		defaultURL = "http://cluster.media.mit.edu/kokompe/STL/teapot.stl";
	}

	frame = new Frame(appname);
    GLCanvas canvas = new GLCanvas();

    progressBar = new JProgressBar(0, 100);
    progressBar.setValue(0);
    progressBar.setStringPainted(true);

    canvas.addGLEventListener(new Viewer());
    frame.add(canvas, BorderLayout.CENTER);

    frame.add(progressBar, BorderLayout.PAGE_END);
    frame.setSize(600, 600);

    Point windowCorner = new Point(50,50);
    frame.setLocation(windowCorner);

    final Animator animator = new Animator(canvas);
    frame.addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          // Run this on another thread than the AWT event queue to
          // make sure the call to Animator.stop() completes before
          // exiting
          new Thread(new Runnable() {
              public void run() {
                animator.stop();
                System.exit(0);
              }
            }).start();
        }
      });
    frame.setVisible(true);
    animator.start();
  }

  static private JProgressBar progressBar;
  private int prevMouseX, prevMouseY;
  private boolean mouseRButtonDown = false;
  private Vector3 rotAxis;
  private float rotAngle;
  private boolean rotationSet = false;
  float viewDistance = 40.0f;
  float windowAspectRatio = 1.0f;
  FloatBuffer curMatrix;
  FloatBuffer lastMatrix;
  float zoomFactor = 1.0f;
  boolean panSet = false;
  Vector3 panVector = new Vector3();
  Vector3 lastPanVector = new Vector3();
  URL stlfile;

  Timer urlCheckTimer;
  long urlLastModified = 0;
  boolean newData = true;
  boolean assemblyMode;

  HashMap<String,mapentry> urlMap;
  LinkedList<part> partList;

  private int readIntLittleEndian(DataInputStream in) throws IOException {

	 // 4 bytes
	   int accum = 0;
	   for ( int shiftBy=0; shiftBy<32; shiftBy+=8 )
	      {
	      accum |= ( in.readByte() & 0xff ) << shiftBy;
	      }
	   return accum;

  }

  private int readIntLittleEndianFromBuffer(byte buffer[], int startIndex) {
		 // 4 bytes
	   int accum = 0;
	   int index = startIndex;
	   for ( int shiftBy=0; shiftBy<32; shiftBy+=8 )
	      {
	      accum |= ( buffer[index] & 0xff ) << shiftBy;
	      index++;
	      }
	   return accum;
	  
	  
  }
  
  
  public void init(GLAutoDrawable drawable) {

		// ************** SET UP TO DRAW WINDOW

	    GL gl = drawable.getGL();

	    System.err.println("INIT GL IS: " + gl.getClass().getName());
	    System.err.println("Chosen GLCapabilities: " + drawable.getChosenGLCapabilities());

	    gl.setSwapInterval(1);

	    // Position of the Light
	    float pos[] = {60.0f, 60.0f, 120.0f, 0.0f };
	    // Color of the Lighting
	    float ambientColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
	    float diffuseColor[] = {0.3f, 0.3f, 0.3f, 1.0f};
	    float specularColor[] = {0.2f, 0.2f, 0.2f, 1.0f};
	    // Color of the material
	    // Four colors for an openGL object: ambient, diffuse, specular, and emission
	    // specular and emission are fixed for the whole object, set here
	    // ambient and diffuse are set by glColor
	    float materialSpecularColor[] = {0.05f, 0.05f, 0.05f, 1.0f};  // set to 0 to diasable shadows
	    float materialEmissionColor[] = {0.0f, 0.0f, 0.0f, 1.0f};     // set to nonzero to make object glow

	    // ************* SET UP OPENGLSCENE LIGHTING ****************************

	    gl.glLightfv(GL.GL_LIGHT0, GL.GL_POSITION, pos, 0);

	    // my params
	    gl.glLightfv(GL.GL_LIGHT0, GL.GL_AMBIENT, ambientColor, 0);
	    gl.glLightfv(GL.GL_LIGHT0, GL.GL_DIFFUSE, diffuseColor, 0);
	    gl.glLightfv(GL.GL_LIGHT0, GL.GL_SPECULAR, specularColor, 0);

	    gl.glEnable(GL.GL_CULL_FACE);
	    gl.glEnable(GL.GL_LIGHTING);
	    gl.glEnable(GL.GL_LIGHT0);
	    gl.glEnable(GL.GL_DEPTH_TEST);
	    gl.glEnable(GL.GL_COLOR_MATERIAL);

	    gl.glMaterialfv(GL.GL_FRONT, GL.GL_SPECULAR, materialSpecularColor, 0);
	    gl.glMaterialfv(GL.GL_FRONT, GL.GL_EMISSION, materialEmissionColor, 0);
	    gl.glColorMaterial(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE);


	// ******** INITIALIZE ROTATION MATRICIES FOR ARCBALL ROTATION

	// Set up the rotation storage matricies
	curMatrix = FloatBuffer.allocate(16);
	lastMatrix = FloatBuffer.allocate(16);
	// fill in the last matrix with identity
	for(int i=0; i<16; i++) {
		lastMatrix.put(i, 0.0f);
	}
	lastMatrix.put(0, 1.0f);
	lastMatrix.put(5, 1.0f);
	lastMatrix.put(10, 1.0f);
	lastMatrix.put(15, 1.0f);

	// ********** GET URL TO STL/KAF FILE TO DOWNLOAD ********************

	String urltext;

	urltext = System.getProperty("jnlp.stlurl");
	if (urltext == null) {
		urltext = defaultURL;
	}

	try {
		stlfile = new URL(urltext);
	}
 		catch (MalformedURLException e) {
		System.err.println("Malformed URL!");
		System.exit(0);
	}

 	partList = new LinkedList<part>();
 	urlMap = new HashMap<String,mapentry>();

 	// Check to see whether the file is part or assembly
 	// by checking the file extension.  kaf is assembly,
 	// stl is a part

 	String filename = stlfile.getFile();
 	filename = filename.toLowerCase();
 	if (filename.endsWith("kaf"))
 		assemblyMode = true;
 	else if ((filename.endsWith("stl")) ||
 			 (filename.endsWith("kpf")) ||
 			 (filename.endsWith("kpf.gz")) ||
 			 (filename.endsWith("stl.gz")))
 		assemblyMode = false;
 	else {
 		System.err.println("Unknown file extension, must be kaf, stl, kpf, stl.gz, or kpf.gz");
 		System.exit(0);
 	}

 	// Create assembly table.  (Even for parts, create a trivial
 	// assembly table with only one watched URL.)

 	if (assemblyMode) {
 		// Load initial data for assembly mode
 		updateAssemblyData(gl);
 		newData = false;
 	}
 	else {
 		// Load initial data for part mode
 		createTrivialAssemblyData(gl, urltext);
 	}
	updateAssemblyDisplayLists(gl);

    // **************** SET UP MOUSE LISTENERS *****************

    drawable.addMouseListener(this);
    drawable.addMouseMotionListener(this);
    drawable.addMouseWheelListener(this);

    // *************** SETUP KEYBOARD LISTENERS

    drawable.addKeyListener(this);

    // *************** SETUP FILE UPDATE CHECK TIMER

    urlCheckTimer = new Timer();
    urlCheckTimer.scheduleAtFixedRate(new TimerTask() {
    	public void run() {
    		try {

    			// If in assembly mode, check the assembly file for changes

    			if (assemblyMode && (newData == false)) {
    				URLConnection filestream2;
    				filestream2 = stlfile.openConnection();
    				filestream2.connect();
    				if (filestream2.getLastModified() > urlLastModified) {
    					if (filestream2.getContentLength() > 0) {
    						newData = true;
    					}
    				}
    			}

    			// Check the list of part URL's for changes, mark if needed
    			Collection<mapentry> mapCollection = urlMap.values();
    			Iterator<mapentry> mapIterator = mapCollection.iterator();
    			while (mapIterator.hasNext()) {
    				mapentry thisEntry = mapIterator.next();
    				URLConnection filestream3;
    				filestream3 = thisEntry.partURL.openConnection();
    				filestream3.connect();
    				if (filestream3.getLastModified() > thisEntry.urlLastModified) {
   				        progressBar.setValue(0);
    					progressBar.setIndeterminate(true);
    					progressBar.setString("Rendering Model...");
    					if (filestream3.getContentLength() >= 84) {
    						thisEntry.newData = true;
    					}
    				}
    			}

    		}

    		catch (IOException e) {
    			System.out.println(" I/O Exception trying to re-read modification time!");
    		}
    	}
    }, (long)1000, (long)1000);


  }

  public void updateAssemblyDisplayLists(GL gl) {
	  Collection<mapentry> mapCollection = urlMap.values();
	  Iterator<mapentry> mapIterator = mapCollection.iterator();
	  while (mapIterator.hasNext()) {
		  mapentry thisEntry = mapIterator.next();
		  if (thisEntry.newData) {
			// There is new data, so call updateModel to read it.
			if(updateModel(gl, thisEntry))
				thisEntry.newData = false;
		  }
	  }
  }
  
  float[] computeFacetNormal(float[] vertexArray, int startIndex) {
	  float[] normalout = new float[3];
	  
	  Vector3[] verticies = new Vector3[3];
	  
	  Vector3 normal;
	    
	  int index = startIndex;
	  for (int i=0; i<3; i++) {
		  verticies[i] = new Vector3(vertexArray[index], vertexArray[index+1], vertexArray[index+2]);
		  index += 3;
	  }
	  
	  // Compute facet normal by cross product
	  // TODO: can openGL do this automatically with hardware acceleration?
	  normal =  Vector3.cross(Vector3.sub(verticies[1],verticies[0]), Vector3.sub(verticies[2], verticies[0]));
      normal.normalize();
	  
	  normalout[0] = normal.x;
	  normalout[1] = normal.y;
	  normalout[2] = normal.z;
	  return(normalout);
  
  }

  
  

  public boolean updateModel(GL gl, mapentry thisEntry) {
	  int numStlFacets = 0;
	  int bytesToRead = 0;
	  byte [] facetdata = new byte[0];

	  try {

		  try {


		  	URLConnection filestream;

			filestream = thisEntry.partURL.openConnection();

			thisEntry.urlLastModified = filestream.getLastModified();
			int urlContentLength = filestream.getContentLength();

	
			// Check to see if enough of the file is present
			// to read its length

			int minLength;
			if (thisEntry.isKPF)
				minLength = 4;
			    
			else 
			    minLength = 84;
			
			// Not strictly correct for gzipped files but probably OK
			if (urlContentLength < minLength) {
				// file is not finished writing.  return false,
				// which will result in a retry in a few moments
				return(false);
			}

			DataInputStream in;
			if (thisEntry.isCompressed)
				in = new DataInputStream(new GZIPInputStream(filestream.getInputStream()));
			else
				in = new DataInputStream(filestream.getInputStream());

			
			if (!thisEntry.isKPF) {
				// Skip past the STL header
				System.err.println("Skipping STL header.");  // TODO remove
				byte[] header;
				header = new byte[80];
				in.read(header, 0, 80);
			}		

			if (thisEntry.isKPF) {
				bytesToRead = readIntLittleEndian(in) - 8;
				numStlFacets = readIntLittleEndian(in);
				System.err.println("KPF file in URL contains " + numStlFacets + " facets in " + bytesToRead + " uncompressed bytes.");
			}
			else {
				
				// Read in the number of facets
				numStlFacets = 0;
				numStlFacets = readIntLittleEndian(in);
				System.err.println("STL file in URL contains " + numStlFacets + " facets.");

				//	 facet data is 12 floats plus a short (4*12 + 2 = 50 bytes/facet)
				bytesToRead = numStlFacets*50;
			}
			
			
			int totalFileLength;
			if (thisEntry.isKPF) 
				totalFileLength = bytesToRead + 8;
			else
				totalFileLength = bytesToRead + 84;
			
			// TODO: How can we determine if a GZIP file is finished writing?
			// Workaround for now is that transfer will crash and restart on
			// premature EOF
			if (!thisEntry.isCompressed) {
					// file is not yet finished writing.
				if (urlContentLength < totalFileLength) {
					// return false, which will result in trying again in a moment
					
					System.err.println("urlContentLength: " + urlContentLength + "totalFileLength:" + totalFileLength);		
					return(false);
				}
			}

			// OK, we have a valid file!!! set up progress bar
			progressBar.setMinimum(0);
			progressBar.setMaximum(bytesToRead);
			progressBar.setValue(0);
			progressBar.setIndeterminate(false);
			Rectangle progressRect = progressBar.getBounds();
			progressRect.x = 0;
			progressRect.y = 0;


			facetdata = new byte[bytesToRead];
			int bytesRead = 0;
			int bytesThisRead = 0;

			while ((bytesRead < bytesToRead) && (bytesThisRead != -1)) {
				bytesThisRead = in.read(facetdata, bytesRead, bytesToRead - bytesRead);
				if (bytesThisRead > 0) {
					bytesRead += bytesThisRead;
					progressBar.setValue(bytesRead);
					progressBar.setString("Downloading Model...");
					progressBar.paintImmediately(progressRect);

				}
				System.err.println("Bytes Read: " + bytesRead);
			}

			// Done reading URL - close connection
		    in.close();

		    if (bytesThisRead == -1) {
		    	System.err.println("File ended unexpectedly while downloading");
		    	return(false);
		    }
		  }
		catch (IOException e) {
			System.err.println("I/O Exception reading data from server.");
			return(false);
		}

		progressBar.setString("Displaying Model...");

		if (numStlFacets > 0) {

		    // *************** REFORMAT DATA FOR OPENGL ***************************
		    // Little Endian vs. Big Endian byte ordering, etc.

		    float[] normalArray = new float[numStlFacets*9];
		    float[] vertexArray = new float[numStlFacets*9];

			// Now reformat data into float arrays

		    int byteCtr = 0;
		    int normalCtr = 0;
		    int vertexCtr = 0;
		    int coordCtr = 0;

		    if (thisEntry.isKPF) {
		    	// KPF parsing
		    	
		    	byteCtr = 0;
		    	int i,j;
		    	
	    		float[] vertexTable = new float[65536*3];
	    		int[] triangleVerticies = new int[3];
		    	int tv;
	    		
		    	while(byteCtr < bytesToRead) {
		    		
		    		// Read the next block
	
		    		int numVerticies = readIntLittleEndianFromBuffer(facetdata, byteCtr);
		    		byteCtr += 4;
		    		int numTriangles = readIntLittleEndianFromBuffer(facetdata, byteCtr);
		    		byteCtr += 4;
	
		    		System.err.println("Reading block with " + numVerticies + "verticies and " + numTriangles + " triangles.");
		    		
		    		// Read the verticies
		    		int vertexTableIndex = 0;
		    		for (i=0; i<numVerticies; i++) {	
			    		// Get the 3 floats defining the vertex
			    		for (int vertexFloat = 0; vertexFloat < 3; vertexFloat++) {
			    			int accum = 0;
			    			for ( int shiftBy=0; shiftBy<32; shiftBy+=8 ) {
			    				accum |= ( facetdata[byteCtr] & 0xff ) << shiftBy;
			    				byteCtr++;
			    			}
			    			vertexTable[vertexTableIndex] = (float)Float.intBitsToFloat( accum );
			    			
			    			// find bounding box and store
					    	  if (vertexTable[vertexTableIndex] > thisEntry.maxBox[vertexFloat])
					    		  thisEntry.maxBox[vertexFloat] = vertexTable[vertexTableIndex];
					    	  if (vertexTable[vertexTableIndex] < thisEntry.minBox[vertexFloat])
					    		  thisEntry.minBox[vertexFloat] = vertexTable[vertexTableIndex];
					    	  vertexTableIndex++;
			    		}	
		    		}
		    		
		    		// Read the triangles and write out to openGL data structure
		    		
		    		for (i=0; i<numTriangles; i++) {
		    			// Get the three 16-bit ints defining the triangles
		    			for (j=0; j<3; j++) {
			    			int accum = 0;
		    				for ( int shiftBy=0; shiftBy<16; shiftBy+=8 ) {
			    				accum |= ( facetdata[byteCtr] & 0xff ) << shiftBy;
			    				byteCtr++;
			    			}
		    				triangleVerticies[j] = accum;
		    			}
		    			
		    			// Fill the vertex array for openGL
		    			for (tv=0; tv<3; tv++) {
		    				int vertexIndex = 3*triangleVerticies[tv];
		    				for (j=0; j<3; j++) {
		    					vertexArray[vertexCtr] = vertexTable[vertexIndex+j];
		    					vertexCtr++;
		    				}
		    			
		    			}
		    			
		    			// Compute the facet normal
		    			float facetNormal[] = computeFacetNormal(vertexArray, vertexCtr-9);
		    			
		    			// Replicate the facet normal 3 times into the normal array
		    			for(tv=0; tv<3; tv++) {
		    				for (j=0; j<3; j++) {
		    					normalArray[normalCtr] = facetNormal[j];
		    					normalCtr++;
		    				}
		    			}
		    		}
		    	}
		    	System.err.println("Done Parsing KPF.");		    		
		    }
		    else {
		    	// STL parsing
		    	
		    	for (int facet = 0; facet < numStlFacets; facet++) {

		    		// Get the 3 floats defining the facet normal
		    		for (int normalFloat = 0; normalFloat < 3; normalFloat++) {
		    			int accum = 0;
		    	  for ( int shiftBy=0; shiftBy<32; shiftBy+=8 ) {
		    		  accum |= ( facetdata[byteCtr] & 0xff ) << shiftBy;
		    		  byteCtr++;
			      }
		    	  normalArray[normalCtr++] = (float)Float.intBitsToFloat( accum );
		      }
		      // Duplicate normal twice
		      for(int k=0; k<6; k++) {
		    		  normalArray[normalCtr] = normalArray[normalCtr-3];
		    		  normalCtr++;
		      }


		      //    Get the 9 floats defining the vertex coordinates
		      for (int vertexFloat = 0; vertexFloat < 9; vertexFloat++) {
		    	  int accum = 0;
		    	  for ( int shiftBy=0; shiftBy<32; shiftBy+=8 ) {
		    		  accum |= ( facetdata[byteCtr++] & 0xff ) << shiftBy;
			      }
		    	  vertexArray[vertexCtr] = ((float)Float.intBitsToFloat( accum ) - 10.0f);
		    	  // find bounding box and store
		    	  if (vertexArray[vertexCtr] > thisEntry.maxBox[coordCtr])
		    		  thisEntry.maxBox[coordCtr] = vertexArray[vertexCtr];
		    	  if (vertexArray[vertexCtr] < thisEntry.minBox[coordCtr])
		    		  thisEntry.minBox[coordCtr] = vertexArray[vertexCtr];
		    	  vertexCtr++;
		    	  coordCtr++;
		    	  if (coordCtr == 3)
		    		  coordCtr = 0;
		      }

		      // Skip attribute byte count
		      byteCtr += 2;
		    	}
		    	
		    	System.err.println("Done parsing STL.");
		    	

		    }
		    	
		    // ************** WRAP FLOAT ARRAYS IN FLOATBUFFER OBJECTS FOR JOGL

		    ByteBuffer underlyingVertexBuffer = ByteBuffer.allocateDirect(numStlFacets*9*4).order(ByteOrder.nativeOrder());
		    FloatBuffer vertexBuffer = 	underlyingVertexBuffer.asFloatBuffer();
		    vertexBuffer.put(vertexArray, 0, numStlFacets*9);
		    vertexBuffer.position(0);

		    ByteBuffer underlyingNormalBuffer = ByteBuffer.allocateDirect(numStlFacets*9*4).order(ByteOrder.nativeOrder());
		    FloatBuffer normalBuffer = 	underlyingNormalBuffer.asFloatBuffer();
		    normalBuffer.put(normalArray, 0, numStlFacets*9);
		    normalBuffer.position(0);

		    // ************ CREATE A DISPLAY LIST CONTAINING THE STL OBJECT *********
		    
		    gl.glNewList(thisEntry.displayList, GL.GL_COMPILE);

		    // Create vertex arrays
		    gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
		    gl.glEnableClientState(GL.GL_NORMAL_ARRAY);
		    gl.glVertexPointer(3, GL.GL_FLOAT, 0, vertexBuffer);
		    gl.glNormalPointer(GL.GL_FLOAT, 0, normalBuffer);

		    // Draw Triangles
		    gl.glDrawArrays(GL.GL_TRIANGLES, 0, numStlFacets*3);
		    //gl.glPopMatrix();

		    gl.glEndList();

		}

		else {
			// display blank screen on valid STL file with no facets
			 gl.glNewList(thisEntry.displayList, GL.GL_COMPILE);
			 gl.glEndList();

		}
		progressBar.setString("");

	  }
	  catch (OutOfMemoryError e) {

		outOfMemoryBox.start();
  	  }
	  return(true);
	}


  Thread outOfMemoryBox = new Thread() {
	  public void run() {
	     JOptionPane.showMessageDialog(null, "Viewer out of memory.", "KOKOMPE Viewer", JOptionPane.ERROR_MESSAGE);
	     System.exit(1);
	  }
  };


  public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
    windowAspectRatio = (float)height / (float)width;
  }

  // called in part mode to create a single-entry
  // assembly table and url map to just display a part
  public void createTrivialAssemblyData(GL gl, String urltext) {

	  try {

		  partList.clear();

		  part thisPart = new part();
		  // Set default identity Modelview Matrix
		  thisPart.modelviewMatrix[0] = 1.0f;
		  thisPart.modelviewMatrix[1] = 0.0f;
		  thisPart.modelviewMatrix[2] = 0.0f;
		  thisPart.modelviewMatrix[3] = 0.0f;

		  thisPart.modelviewMatrix[4] = 0.0f;
		  thisPart.modelviewMatrix[5] = 1.0f;
		  thisPart.modelviewMatrix[6] = 0.0f;
		  thisPart.modelviewMatrix[7] = 0.0f;

		  thisPart.modelviewMatrix[8] = 0.0f;
		  thisPart.modelviewMatrix[9] = 0.0f;
		  thisPart.modelviewMatrix[10] = 1.0f;
		  thisPart.modelviewMatrix[11] = 0.0f;

		  thisPart.modelviewMatrix[12] = 0.0f;
		  thisPart.modelviewMatrix[13] = 0.0f;
		  thisPart.modelviewMatrix[14] = 0.0f;
		  thisPart.modelviewMatrix[15] = 1.0f;

		  // Set part color to default red
		  thisPart.color[0] = 1.0f;
		  thisPart.color[1] = 0.0f;
		  thisPart.color[2] = 0.0f;

		  thisPart.partURL = new String(urltext);

		  partList.add(thisPart);

		  // add an entry to the URL hashtable for the part

		  mapentry thisEntry = new mapentry();
		  thisEntry.partURL = new URL(thisPart.partURL);
		  thisEntry.urlLastModified = 0;
		  thisEntry.newData = true;
		  thisEntry.displayList = gl.glGenLists(1); // make a new display list
		  String lowerURL = urltext.toLowerCase();
		  if ((lowerURL.endsWith("kpf")) ||
			  (lowerURL.endsWith("kpf.gz")))
		      thisEntry.isKPF = true;
		  else
		      thisEntry.isKPF = false;
		  if (lowerURL.endsWith("gz"))
			  thisEntry.isCompressed = true;
		  else
			  thisEntry.isCompressed = false;
		  
		  urlMap.put(thisPart.partURL, thisEntry);

	  }
	  catch (IOException e) {
		  System.out.println("Malformed URL attempting to create assembly data for single part viewing.");
		  System.exit(1);
	  }
  }

  // in assembly mode, calling this function re-reads the assembly
  // file from stlurl and updates the urlMap table and the drawTable
  public void updateAssemblyData(GL gl) {

	  System.err.println("Updating assembly data.");
	  // Read in the assembly file
	  try {

		  URLConnection filestream = stlfile.openConnection();
		  urlLastModified = filestream.getLastModified();

		  InputStreamReader in2 = new InputStreamReader(filestream.getInputStream());
		  BufferedReader in = new BufferedReader(in2);

		  partList.clear();
		  boolean readError = false;
		  while (in.ready() && (readError == false)) {

			  part thisPart = new part();
			  try {
				  String nextLine = in.readLine();
				  StringTokenizer tokenizer = new StringTokenizer(nextLine, " [],", false);

				  // Read part URL string
				  thisPart.partURL = tokenizer.nextToken();

		   		  // Read modelview matrix
				  Float thisElement;
				  int i;
				  for (i=0; i<16; i++) {
					  thisElement = Float.valueOf(tokenizer.nextToken());
					  thisPart.modelviewMatrix[i] = thisElement;
				  }
				  // Read color vector
				  for (i=0; i<3; i++) {
					  thisElement = Float.valueOf(tokenizer.nextToken());
				  thisPart.color[i] = thisElement;
				  }
			  }
			  catch (Exception e) {
				  readError = true;
			  }

			  if (readError == false) {
				  partList.add(thisPart);

				  // Now, check to see if this URL is in the urlMap, and
				  // add it if not
				  if (urlMap.containsKey(thisPart.partURL) == false) {
					  mapentry thisEntry = new mapentry();
					  thisEntry.partURL = new URL(thisPart.partURL);

					  String lowerURL = thisPart.partURL.toLowerCase();
		  if (lowerURL.endsWith("kcf"))
		      thisEntry.isKPF = true;
		  else
		      thisEntry.isKPF = false;
		  if (lowerURL.endsWith("gz"))
			  thisEntry.isCompressed = true;
		  else
			  thisEntry.isCompressed = false;

					  thisEntry.urlLastModified = 0;
					  thisEntry.newData = true; // is this right?
					  thisEntry.displayList = gl.glGenLists(1); // make a new display list
					  urlMap.put(thisPart.partURL, thisEntry);
				  }
			  }
		  }

		  // TODO: At some point, it might be nice to garbage-collect
		  // no-longer-needed display lists here, but this is not necessarily
		  // even desirable (now we are caching display lists!) and is extra
		  // work and may introduce extra bugs, so I am going to put this off
		  // for now.

	  }
	  catch (IOException e) {
		  System.out.println("I/O Exception reading assembly file.");
	  }
  }

  public void display(GLAutoDrawable drawable) {
	float viewDistance = 6.0f;
    GL gl = drawable.getGL();

    // Check for updates to assembly file
    if (assemblyMode && newData) {
    	updateAssemblyData(gl);
    	newData = false;
    }

    // Check for updates to parts
    updateAssemblyDisplayLists(gl);

    // OPENGL CONFIGURATION

    if ((drawable instanceof GLJPanel) &&
        !((GLJPanel) drawable).isOpaque() &&
        ((GLJPanel) drawable).shouldPreserveColorBufferIfTranslucent()) {
      gl.glClear(GL.GL_DEPTH_BUFFER_BIT);
    } else {
      gl.glClear(GL.GL_COLOR_BUFFER_BIT | GL.GL_DEPTH_BUFFER_BIT);
    }

    // Set up the projection matrix
    gl.glMatrixMode(GL.GL_PROJECTION);
    gl.glLoadIdentity();
    gl.glOrtho(-1.0f*zoomFactor, 1.0f*zoomFactor, -windowAspectRatio*zoomFactor, windowAspectRatio*zoomFactor, viewDistance*zoomFactor - 2.0f, viewDistance*zoomFactor + 2.0f);

   // gl.glPushMatrix();
    gl.glMatrixMode(GL.GL_MODELVIEW);

    // Start un-rotated
    gl.glLoadIdentity();

    // Translate first to pan object
    gl.glTranslatef(panVector.x, panVector.y, panVector.z);
    gl.glTranslatef(lastPanVector.x, lastPanVector.y, lastPanVector.z);


   // Translate last to move the rotated object away from the viewer
    gl.glTranslatef(0.0f, 0.0f, -viewDistance*zoomFactor);

    // Current rotation
    if (rotationSet) {
     	gl.glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
    }

    // Apply rotation from previous time
    gl.glMultMatrixf(lastMatrix);

    // Store the current matrix; when the mouse is released, it becomes the last
    gl.glGetFloatv(GL.GL_MODELVIEW_MATRIX, curMatrix);

    // Determine the global translation and scaling required for the assembly.
    // Run through the parts list and create a global bounding box
    ListIterator<part> partPreIterator = partList.listIterator();
    Vector3 minBox = new Vector3();
    Vector3 maxBox = new Vector3();
    Vector3[] corners;
    int i;

    minBox.setToMax();
    maxBox.setToMin();

    while (partPreIterator.hasNext()) {
    	part prePart = partPreIterator.next();
    	mapentry preEntry = urlMap.get(prePart.partURL);

    	Vector3 thisMaxBox = new Vector3(preEntry.maxBox[0], preEntry.maxBox[1],preEntry.maxBox[2]);
    	Vector3 thisMinBox = new Vector3(preEntry.minBox[0], preEntry.minBox[1],preEntry.minBox[2]);

    	corners = Vector3.constructCorners(thisMinBox, thisMaxBox);

    	for (i=0; i<8; i++) {
    		corners[i] = Vector3.transform(prePart.modelviewMatrix, corners[i]);
    		minBox.setMinimumComponents(corners[i]);
    		maxBox.setMaximumComponents(corners[i]);
    	}
    }

    Vector3 boxCenter = Vector3.mul(Vector3.add(maxBox, minBox), 0.5f);
    float scaleFactor = 1.0f/Vector3.computeScaleFactor(maxBox, minBox, boxCenter);
    gl.glScalef(scaleFactor, scaleFactor, scaleFactor);
    gl.glTranslatef(-boxCenter.x, -boxCenter.y, -boxCenter.z);

    // Run through the part list and draw all the parts
    ListIterator<part> partIterator = partList.listIterator();
    while (partIterator.hasNext()) {
    	part thisPart = partIterator.next();
    	gl.glPushMatrix();
    	gl.glColor3f(thisPart.color[0], thisPart.color[1], thisPart.color[2]);
    	gl.glMultMatrixf(thisPart.modelviewMatrix, 0);
    	mapentry thisEntry = urlMap.get(thisPart.partURL);
    	gl.glCallList(thisEntry.displayList);
    	gl.glPopMatrix();
    }
  }

  // Methods required for the implementation of MouseListener
  public void displayChanged(GLAutoDrawable drawable, boolean modeChanged, boolean deviceChanged) {}
  public void mouseEntered(MouseEvent e) {}
  public void mouseExited(MouseEvent e) {}

  public void mousePressed(MouseEvent e) {
    prevMouseX = e.getX();
    prevMouseY = e.getY();

    if ((e.getModifiers() & InputEvent.BUTTON3_MASK) != 0) {
      mouseRButtonDown = true;
    }
  }

  public void mouseReleased(MouseEvent e) {
    if ((e.getModifiers() & InputEvent.BUTTON3_MASK) != 0) {
      mouseRButtonDown = false;
    }

    // When the mouse button goes up, copy the rotational part
    // of curMatrix into lastMatrix --- this is the starting point
    // for the current arcball rotation.
    for(int i=0; i<16; i++) {
    	lastMatrix.put(i, curMatrix.get(i));
    }
    // identity-out the translation and scaling parts of the matrix
    lastMatrix.put(3, 0.0f);
    lastMatrix.put(7, 0.0f);
    lastMatrix.put(11, 0.0f);
    lastMatrix.put(12, 0.0f);
    lastMatrix.put(13, 0.0f);
    lastMatrix.put(14, 0.0f);
    lastMatrix.put(15, 1.0f);
    rotAngle = 0.0f;  // once the mouse goes up, rotation is over

    // update the pan vector base
    lastPanVector = Vector3.add(lastPanVector, panVector);
    // Panning stops when mouse is released
    panVector.clear();

  }

  public void mouseClicked(MouseEvent e) {}

  // Methods required for the implementation of MouseMotionListener
  public void mouseDragged(MouseEvent e) {
    int x = e.getX();
    int y = e.getY();
    Dimension size = e.getComponent().getSize();



    if (mouseRButtonDown) {
    	// If right button is down, pan
    	panVector.x = 2.0f * zoomFactor * ((float)x - (float)prevMouseX) / ( (float)size.width );
    	panVector.y = 2.0f * zoomFactor * ((float)prevMouseY - (float)y) / ( (float)size.height );
    	panVector.z = 0.0f;
    	panSet = true;
    }
    else
    {
    	// othwise, rotate

    	Vector3 lastPoint = new Vector3();
    	Vector3 curPoint = new Vector3();
    	lastPoint.setArcballPos(prevMouseX, prevMouseY, size.width, size.height);
    	curPoint.setArcballPos(x, y, size.width, size.height);
    	Vector3 arcBallVector = Vector3.sub(curPoint, lastPoint);

    	float scale_factor = 180.0f * zoomFactor;
    	rotAngle = arcBallVector.length() * scale_factor;
    	rotAxis = Vector3.cross(lastPoint, curPoint);
    	rotAxis.normalize();
    	rotationSet = true;
    }
  }

  public void mouseMoved(MouseEvent e) {}

  public void mouseWheelMoved(MouseWheelEvent e) {
      int notches = e.getWheelRotation();

      if (notches > 0)
    	  zoomFactor *= 1.1;
      else
    	  zoomFactor /= 1.1;

  }

  public void keyTyped(KeyEvent e) {}
  public void keyReleased(KeyEvent e) {}

  // Implement keyboard +/- zoom for systems
  // without a mousewheel (like laptops)
  public void keyPressed(KeyEvent e) {
      int keyCode = e.getKeyCode();

      if (keyCode == 45)
    	  zoomFactor *= 1.1;
      if (keyCode == 61)
    	  zoomFactor /= 1.1;
  }

}
