package javaviewer;

import java.net.*;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.Properties;
import java.io.*;

import java.util.Timer;
import java.util.TimerTask;


import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;

import javax.media.opengl.*;
import javax.swing.JProgressBar;

import com.sun.opengl.util.*;
import java.lang.Math;

/*  Kokompe Java Viewer 0.2
 *  By Ara Knaian, 2008
 *  
 *  Originally adapted from Gears demo by Brian Paul */

public class Viewer implements GLEventListener, MouseListener, MouseMotionListener, MouseWheelListener {
	
	public static void main(String[] args) {
   
	String appname = System.getProperty("jnlp.title");	
	if (appname == null) {
		appname = "KOKOMPE Viewer";
	}	
	Frame frame = new Frame(appname);
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
    frame.show();
    animator.start();
  }

  static private JProgressBar progressBar;
  private int gear1;
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
  
  private int readIntLittleEndian(DataInputStream in) throws IOException {
	  
	 // 4 bytes
	   int accum = 0;
	   for ( int shiftBy=0; shiftBy<32; shiftBy+=8 )
	      {
	      accum |= ( in.readByte() & 0xff ) << shiftBy;
	      }
	   return accum;
	  

  }
  
  public void init(GLAutoDrawable drawable) {
    // Use debug pipeline
    // drawable.setGL(new DebugGL(drawable.getGL()));

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
	    
	// ********** GET URL TO STL FILE TO DOWNLOAD ********************
	  
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
		stlfile = new URL(urltext);
	}
 		catch (MalformedURLException e) {
		System.err.println("Malformed URL!");
		System.exit(0);
	}
		
   if(updateModel(gl))
	   newData = false;
    
    // **************** SET UP MOUSE LISTENERS *****************
    
    drawable.addMouseListener(this);
    drawable.addMouseMotionListener(this);
    drawable.addMouseWheelListener(this);
    
	
    urlCheckTimer = new Timer();
    urlCheckTimer.scheduleAtFixedRate(new TimerTask() {
    	public void run() {
    		try {
    			URLConnection filestream2 = stlfile.openConnection();
    			filestream2.connect();
    			System.err.println(" Last modified 1: " + filestream2.getLastModified() + " Length: " + filestream2.getContentLength());
    			if ((filestream2.getLastModified() > urlLastModified) && (newData == false)) {
    				// Let the user know rendering has started
    				progressBar.setValue(0);
    				progressBar.setIndeterminate(true);
    				progressBar.setString("Rendering Model...");
    				if (filestream2.getContentLength() >= 84) {
    				
    				newData = true;				// file must be long enough to be valid STL to start reading
    				}
    			}
    		
    	}
    		catch (IOException e) {
    			System.out.println(" I/O Exception trying to re-read modification time!");	
    		}
    	} 	
    }, (long)1000, (long)1000);
    
    
    
    

  }
  
  public boolean updateModel(GL gl) {
	  int numStlFacets = 0;
	  byte [] facetdata = new byte[0];
	  
	  try {
			
		  	
		  	URLConnection filestream;
		  
			filestream = stlfile.openConnection();

			urlLastModified = filestream.getLastModified();
			int urlContentLength = filestream.getContentLength();
			
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
			numStlFacets = 0;
			numStlFacets = readIntLittleEndian(in);
			System.err.println("STL file in URL contains " + numStlFacets + " facets.");
			
			// Read the actual facet data
			//	 facet data is 12 floats plus a short (4*12 + 2 = 50 bytes/facet)
			
			int bytesToRead = numStlFacets*50;
			
			if (urlContentLength < (bytesToRead + 84)) {
				// file is not yet finished writing.
				// return false, which will result in trying again in a moment
				return(false);
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
		    
		    gear1 = gl.glGenLists(1);
		    gl.glNewList(gear1, GL.GL_COMPILE);
		  //  gl.glScalef(6.0f, 6.0f, 6.0f);
		    
	    
		    // Red object --- change here to change color - RGB
		    // (or could render color with a color array)
		    gl.glColor3f(1.0f, 0.0f, 0.0f);
		    
		    // Create vertex arrays
		    gl.glEnableClientState(GL.GL_VERTEX_ARRAY);
		    gl.glEnableClientState(GL.GL_NORMAL_ARRAY); 
		    gl.glVertexPointer(3, GL.GL_FLOAT, 0, vertexBuffer);
		    gl.glNormalPointer(GL.GL_FLOAT, 0, normalBuffer);
		    
		    // Draw Triangles
		    gl.glDrawArrays(GL.GL_TRIANGLES, 0, numStlFacets*3);
		    
		    gl.glEndList();   
	  
		}
		
		else {
			// display blank screen on valid STL file with no facets
			 gear1 = gl.glGenLists(1);
			 gl.glNewList(gear1, GL.GL_COMPILE);
			 gl.glEndList();
			
		}
		progressBar.setString("");
		
		return(true);
	}
		
  
  
  
  
  public void reshape(GLAutoDrawable drawable, int x, int y, int width, int height) {
    windowAspectRatio = (float)height / (float)width;			
  }

  public void display(GLAutoDrawable drawable) {
    	
	float viewDistance = 6.0f;

    GL gl = drawable.getGL();  
    
    if (newData){
    	if(updateModel(gl));
    	newData = false;
    }
    
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
    
    // Draw the object
    gl.glCallList(gear1);
   // gl.glPopMatrix();    
	
  }
  
  // Methods required for the implementation of MouseListener
  public void displayChanged(GLAutoDrawable drawable, boolean modeChanged, boolean deviceChanged) {}
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
      int x = e.getX();
      int y = e.getY();
      Dimension size = e.getComponent().getSize();

      if (notches > 0)
    	  zoomFactor *= 1.1;
      else
    	  zoomFactor /= 1.1;
  
  }
  
}

