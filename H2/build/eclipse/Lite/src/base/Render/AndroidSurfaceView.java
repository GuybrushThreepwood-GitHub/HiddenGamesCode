package base.Render;

import uk.co.hiddengames.cabbylite.CabbyActivity;
import android.app.Activity;
import android.content.Context;
//import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
//import android.util.AttributeSet;
//import android.util.Log;
//import android.view.KeyEvent;
import android.view.Display;
import android.view.MotionEvent;
import android.view.WindowManager;

//import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
//import javax.microedition.khronos.egl.EGLContext;
//import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

/**
 * A simple GLSurfaceView sub-class that demonstrate how to perform
 * OpenGL ES 1.0 rendering into a GL Surface. Note the following important
 * details:
 *
 */
public class AndroidSurfaceView extends GLSurfaceView {
    
    private BaseRenderer mRenderer;   
    static Activity mActivity;
    
    private static int mSurfaceWidth;
    private static int mSurfaceHeight;
    
	public AndroidSurfaceView(Context context, CabbyActivity activity) {
        super(context);

        mActivity = activity;

        //setEGLConfigChooser( true );
        setEGLConfigChooser( 8, 8, 8, 8, 16, 0 );
        //getHolder().setFormat(PixelFormat.RGBA_8888);
        setPreserveEGLContextOnPause(true);

        mRenderer = new BaseRenderer();
        
        // Set the renderer responsible for frame rendering
        setRenderer(mRenderer);
    }
    
    @Override 
    public void onResume() {
    	
    }
    
    @Override 
    public void onPause() {
    	
    }
    
    @Override 
    public boolean onTouchEvent(MotionEvent e) {
    	//super.onTouchEvent(e);

    	int i=0;
    	int action = e.getAction();
    	int actionCode = action & MotionEvent.ACTION_MASK;  
    	//int pid = (action >> MotionEvent.ACTION_POINTER_ID_SHIFT);
    	int pointCnt = e.getPointerCount();
    	int actionIndex = e.getActionIndex();
    	
        boolean handledEvent = false;    
        	switch(actionCode)
        	{ 
    			case MotionEvent.ACTION_POINTER_DOWN:	
    			case MotionEvent.ACTION_DOWN: 
    			{			
    		        int pointerId = e.getPointerId(actionIndex);
	    		    if( pointerId >= 0 && pointerId < pointCnt )
	    			{	    		        	
	    		        float x = e.getX(pointerId);
	    		        float y = e.getY(pointerId);    	
	    		        	
	    		        CabbyActivity.AddTouch(pointerId, x, mSurfaceHeight-y);
	
	    				handledEvent = true;
	    			}
    			}break;
    			case MotionEvent.ACTION_POINTER_UP:
    			case MotionEvent.ACTION_UP:
    			case MotionEvent.ACTION_CANCEL:
    			{
    				int pointerId = e.getPointerId(actionIndex);	
	    		    //if( pointerId >= 0 && pointerId < pointCnt )
	    			{	    		        	
	    		        CabbyActivity.RemoveTouch(pointerId);	             
	    		        
	    				handledEvent = true;
	    			}
    			}break;				   			
    			case MotionEvent.ACTION_MOVE:
    			{	
    		    	for( i=0; i < pointCnt; i++ )
    		    	{    				
    		        	int pointerId = e.getPointerId(i);
    	    		    if( pointerId >= 0 && pointerId < pointCnt )
    	    			{	
	    		        	float x = e.getX(i);
	    		        	float y = e.getY(i);     				
	    				
	    		        	CabbyActivity.TouchMoved(pointerId, x, mSurfaceHeight-y);
    	    			}
    		    	}
    		    	
    				handledEvent = true;
    			}break;	
        	}
        	
    	return handledEvent;
    } 

    public static int getSurfaceWidth() {
		return mSurfaceWidth;
	}

	public static void setSurfaceWidth(int surfaceWidth) {
		AndroidSurfaceView.mSurfaceWidth = surfaceWidth;
	}

    public static int getSurfaceHeight() {
		return mSurfaceHeight;
	}

	public static void setSurfaceHeight(int surfaceHeight) {
		AndroidSurfaceView.mSurfaceHeight = surfaceHeight;
	}
    
    public boolean isInitialised() {
		return mRenderer.isInitialised();
	}
    
	private static class BaseRenderer implements GLSurfaceView.Renderer {
    	
		private boolean mInit = false;
		
	    public boolean isInitialised() {
			return mInit;
		}
	    
    	public void onDrawFrame(GL10 gl) {
        	
    		if( mInit == true )
    		{
    			CabbyActivity.execute(); 
    		}
        }

        public void onSurfaceChanged(GL10 gl, int width, int height) {
        	Display display = ((WindowManager) mActivity.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay(); 
        	int rotation = display.getRotation(); 
        	
        	CabbyActivity.resize( width, height, rotation ); 
        	
        	setSurfaceWidth( width );
        	setSurfaceHeight( height ); 
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Do nothing.   
        	mInit = true;
        }
    }
}
