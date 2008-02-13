package javaviewer;

import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Properties;
import java.io.*;

import java.awt.*;
import java.awt.event.*;

import javax.media.opengl.*;
import com.sun.opengl.util.*;
import java.lang.Math;



/**
 * Viewer.java <BR>
 * author: Brian Paul (converted to Java by Ron Cemer and Sven Goethel) <P>
 *
 * This version is equal to Brian Paul's version 1.2 1999/10/21
 */

public class Viewer implements GLEventListener, MouseListener, MouseMotionListener, MouseWheelListener {
	
	public static void main(String[] args) {
    Frame frame = new Frame("KOKOMPE Viewer");
    GLCanvas canvas = new GLCanvas();
    
    canvas.addGLEventListener(new Viewer());
    frame.add(canvas);
    frame.setSize(300, 300);
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
    frame.show();
    animator.start();
  }

  private float view_rotx = 20.0f, view_roty = 30.0f, view_rotz = 0.0f;
  private int gear1, gear2, gear3;
  private float angle = 0.0f;

  private int prevMouseX, prevMouseY;
  private boolean mouseRButtonDown = false;
  private Vector3 rotAxis;
  private float rotAngle;
  private boolean rotationSet = false;
  float viewDistance = 40.0f;
  
  private int readIntLittleEndian(DataInputStream in) throws IOException {
	  
	 // 4 bytes
	   int accum = 0;
	   for ( int shiftBy=0; shiftBy<32; shiftBy+=8 )
	      {
	      accum |= ( in.readByte() & 0xff ) << shiftBy;
	      }
	   return accum;
	  

  }
  
  private float readFloatLittleEndian(DataInputStream in) throws IOException {
	  int accum = 0;
	   for ( int shiftBy=0; shiftBy<32; shiftBy+=8 )
	      {
	      accum |= ( in.readByte() & 0xff ) << shiftBy;
	      }
	   return Float.intBitsToFloat( accum );
	  
  }
  
  
  public void init(GLAutoDrawable drawable) {
    // Use debug pipeline
    // drawable.setGL(new DebugGL(drawable.getGL()));

	  
	// Get the URL pointing to the STL file from system.getproperties
	// for now, it is hard-coded
	// I modified the Viewer class to throw any exception "Throws "Exception""
	// This is totally bogus --- it should check for malformed URL's and report an error.
	// this code may need to be modified to work with proxy servers
	
	String urltext;
	String defaulturl = "http://phmgrid1.media.mit.edu/kokompe/STL/teapot.stl"; 

	urltext = System.getProperty("jnlp.stlurl");	
	if (urltext == null) {
		urltext = defaulturl;
	}
	
	try {
		
	URL stlfile = new URL(urltext);
	URLConnection filestream = stlfile.openConnection();
	DataInputStream in = new DataInputStream(filestream.getInputStream());   

	// Skip past the header
	byte[] header;
	header = new byte[80];
	in.read(header, 0, 80);    
	
	for(int m=0;m<80;m++) {
		Byte firstchar = new Byte(header[m]);
		System.err.print(firstchar.toString() + " ");
	}
	System.err.println();
	
	// Read in the number of facets
	int numStlFacets = 0;
	numStlFacets = readIntLittleEndian(in);
	System.err.println("STL file in URL contains " + numStlFacets + " facets.");
	
	// Read the actual facet data
	
	/*byte [] facetdata = new byte[numStlFacets*50];  // facet data is 12 floats
													// plus a short (4*12 + 2 = 50)
	int bytesRead = in.read(facetdata);

	System.err.println("bytesRead: " + bytesRead + " bytes expected: " + numStlFacets*50);
	
    in.close();			// Close the URL connection to the STL file          

    float[] normalArray = new float[numStlFacets*9];
    float[] vertexArray = new float[numStlFacets*9];
    
    
	// Now reformat data into float arrays
    
    int byteCtr = 0;
    int normalCtr = 0;
    int vertexCtr = 0;
    
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
    	  vertexArray[vertexCtr++] = ((float)Float.intBitsToFloat( accum )) - 10.0f;
      }	 
      
      // Skip attribute byte count
      byteCtr += 2;
      
    }
	
    System.err.println("Wrote " + vertexCtr + " floats to vertexArray");

    
    ByteBuffer underlyingVertexBuffer = ByteBuffer.allocateDirect(numStlFacets*9*4).order(ByteOrder.nativeOrder());
    FloatBuffer vertexBuffer = 	underlyingVertexBuffer.asFloatBuffer();   
    vertexBuffer.put(vertexArray, 0, numStlFacets*9);
    vertexBuffer.position(0);
    
    ByteBuffer underlyingNormalBuffer = ByteBuffer.allocateDirect(numStlFacets*9*4).order(ByteOrder.nativeOrder());
    FloatBuffer normalBuffer = 	underlyingNormalBuffer.asFloatBuffer();   
    normalBuffer.put(normalArray, 0, numStlFacets*9);
    normalBuffer.position(0);
    
   // ByteBuffer underlyingNormalBuffer = ByteBuffer.allocateDirect(numStlFacets*4);
    //FloatBuffer vertexBuffer = 	underlyingVertexBuffer.asFloatBuffer();   
    //vertexBuffer.put(vertexArray, 0, numStlFacets*9);
    
    
    System.err.println("Length of vertexBuffer is " + vertexBuffer.capacity());*/
    
	// Set up to draw window
	
    GL gl = drawable.getGL();

    System.err.println("INIT GL IS: " + gl.getClass().getName());

    System.err.println("Chosen GLCapabilities: " + drawable.getChosenGLCapabilities());

    gl.setSwapInterval(1);

    float pos[] = { 5.0f, 5.0f, 10.0f, 0.0f };
    float red[] = { 0.8f, 0.1f, 0.0f, 1.0f };
    float green[] = { 0.0f, 0.8f, 0.2f, 1.0f };
    float blue[] = { 0.2f, 0.2f, 1.0f, 1.0f };

    gl.glLightfv(GL.GL_LIGHT0, GL.GL_POSITION, pos, 0);
    gl.glEnable(GL.GL_CULL_FACE);
    gl.glEnable(GL.GL_LIGHTING);
    gl.glEnable(GL.GL_LIGHT0);
    gl.glEnable(GL.GL_DEPTH_TEST);
            
    /* draw the object in the STL file */
    gear1 = gl.glGenLists(1);
    gl.glNewList(gear1, GL.GL_COMPILE);
    gl.glScalef(6.0f, 6.0f, 6.0f);
    gl.glMaterialfv(GL.GL_FRONT, GL.GL_AMBIENT_AND_DIFFUSE, red, 0);
    
    // Draw the triangles for the object from a vertex array
    /*gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
    gl.glEnableClientState(GL.GL_NORMAL_ARRAY); 
    gl.glVertexPointer(3, GL.GL_FLOAT, 0, vertexBuffer);
    gl.glNormalPointer(GL.GL_FLOAT, 0, normalBuffer);
    
    System.err.println("Got to DrawArrays.");
    gl.glDrawArrays(GL.GL_TRIANGLES, 0, numStlFacets*3);*/
    
       for (int i=0; i < numStlFacets; i++) {
    			gl.glBegin(GL.GL_TRIANGLES);
    			gl.glNormal3f(readFloatLittleEndian(in), readFloatLittleEndian(in), readFloatLittleEndian(in));
    			gl.glVertex3f(readFloatLittleEndian(in)-10.0f, readFloatLittleEndian(in)-10.0f, readFloatLittleEndian(in)-10.0f);
    			gl.glVertex3f(readFloatLittleEndian(in)-10.0f, readFloatLittleEndian(in)-10.0f, readFloatLittleEndian(in)-10.0f);
    			gl.glVertex3f(readFloatLittleEndian(in)-10.0f, readFloatLittleEndian(in)-10.0f, readFloatLittleEndian(in)-10.0f);
    			gl.glEnd();
    			int attributeByteCount = in.readShort();
    	    }
    	   
    	    gl.glEndList();
    	    in.close();			// Close the URL connection to the STL file          
    	   
    	    gl.glEnable(GL.GL_NORMALIZE);
    
    
    
    
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
    
    
    // Set up Mouse Listeners
    
    drawable.addMouseListener(this);
    drawable.addMouseMotionListener(this);
    drawable.addMouseWheelListener(this);
    
	}
	catch (MalformedURLException e) {
		System.err.println("Malformed URL!");
		
	}
	catch (IOException e) {;
		System.err.println("I/O Exception!");
		
	}

  }
    
  float windowAspectRatio = 1.0f;
  
  public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
    GL gl = drawable.getGL();

    float h = (float)height / (float)width;
            
    windowAspectRatio = h;
    
    gl.glMatrixMode(GL.GL_PROJECTION);

    System.err.println("GL_VENDOR: " + gl.glGetString(GL.GL_VENDOR));
    System.err.println("GL_RENDERER: " + gl.glGetString(GL.GL_RENDERER));
    System.err.println("GL_VERSION: " + gl.glGetString(GL.GL_VERSION));
    gl.glLoadIdentity();
    gl.glFrustum(-1.0f, 1.0f, -h, h, 5.0f, 60.0f);
    gl.glMatrixMode(GL.GL_MODELVIEW);
    gl.glLoadIdentity();  					
  }

  
  FloatBuffer curMatrix;
  FloatBuffer lastMatrix;
  float zoomFactor = 1.0f;
  
  
  public void display(GLAutoDrawable drawable) {
    	
    GL gl = drawable.getGL();
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
    gl.glFrustum(-1.0f*zoomFactor, 1.0f*zoomFactor, -windowAspectRatio*zoomFactor, windowAspectRatio*zoomFactor, 5.0f, 60.0f);
 
   // gl.glPushMatrix();
    gl.glMatrixMode(GL.GL_MODELVIEW);

    // Start un-rotated
    gl.glLoadIdentity();
   // Translate last
    gl.glTranslatef(0.0f, 0.0f, -40.0f*zoomFactor);
    
    // Current rotation
    if (rotationSet) {
     	gl.glRotatef(rotAngle, rotAxis.x, rotAxis.y, rotAxis.z);
    }
    
    // Apply rotation from previous time
    gl.glMultMatrixf(lastMatrix);
    
    // Store the current matrix; when the mouse is released, it becomes the last
    gl.glGetFloatv(GL.GL_MODELVIEW_MATRIX, curMatrix);
    
    // Draw the object
    gl.glCallList(gear1);
   // gl.glPopMatrix();       
  }

  public void displayChanged(GLAutoDrawable drawable, boolean modeChanged, boolean deviceChanged) {}
  // Methods required for the implementation of MouseListener
  public void mouseEntered(MouseEvent e) {}
  public void mouseExited(MouseEvent e) {}

  public void mousePressed(MouseEvent e) {
    prevMouseX = e.getX();
    prevMouseY = e.getY();
    
    if ((e.getModifiers() & e.BUTTON3_MASK) != 0) {
      mouseRButtonDown = true;
    }  
  }
    
  public void mouseReleased(MouseEvent e) {
    if ((e.getModifiers() & e.BUTTON3_MASK) != 0) {
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
  }
    
  public void mouseClicked(MouseEvent e) {}
    
  // Methods required for the implementation of MouseMotionListener
  public void mouseDragged(MouseEvent e) {
    int x = e.getX();
    int y = e.getY();
    Dimension size = e.getComponent().getSize();

    Vector3 lastPoint = new Vector3();
    Vector3 curPoint = new Vector3();
    lastPoint.setArcballPos(prevMouseX, prevMouseY, size.width, size.height);
    curPoint.setArcballPos(x, y, size.width, size.height);
    Vector3 arcBallVector = Vector3.sub(curPoint, lastPoint);
    
    float scale_factor = 180.0f;
    rotAngle = arcBallVector.length() * scale_factor;
    rotAxis = Vector3.cross(lastPoint, curPoint);
    rotAxis.normalize();
    rotationSet = true;
  }
    
  public void mouseMoved(MouseEvent e) {}
  
  float zoomX = 0.0f; 
  float zoomY = 0.0f;
  
  public void mouseWheelMoved(MouseWheelEvent e) {
      int notches = e.getWheelRotation();
      int x = e.getX();
      int y = e.getY();
      Dimension size = e.getComponent().getSize();

      System.err.println("Mouse wheel moved " + notches + " notches");
      float units_per_notch = 1.0f;
      if (notches > 0)
    	  zoomFactor *= 1.1;
      else
    	  zoomFactor /= 1.1;

      //zoomX = (x - size.width)/size.width;
      
      
      
      //viewDistance += units_per_notch*(float)notches;
      
      //if (viewDistance > 80.0f)
      //	viewDistance = 80.0f;
      //if (viewDistance < 0.0f)
    //	  viewDistance = 0.0f;
      
//      System.err.println("viewDistance is " + viewDistance);
      
  }
}

