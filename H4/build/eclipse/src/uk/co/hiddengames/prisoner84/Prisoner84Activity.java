package uk.co.hiddengames.prisoner84;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.util.Log;
import base.Core.CoreUtility;
import base.Support.FlurrySupport;
import base.Input.InputSensors;
import base.Render.AndroidSurfaceView;

public class Prisoner84Activity extends Activity {

	public static native void setAPK( String apkDir );
	public static native void setSaveDirectory( String saveDir );
	public static native void setTabletFlag( boolean isTablet );
	public static native void setRetinaFlag( boolean isRetina );
	public static native void create();
	public static native void resize( int width, int height, int rotation );
	public static native void execute();
	public static native void destroy();
	
	public static native void appInBackground();
	public static native void appInForeground();
	
	public static native void AddTouch( int index, float x, float y );
	public static native void AddTap( int index );
	public static native void TouchMoved( int index, float x, float y );
	public static native void RemoveTouch( int index );
	
    AndroidSurfaceView mView;
    private InputSensors mInputSensors;
	private CoreUtility mCoreUtils;
	private FlurrySupport mFlurrySupport;
	
	private boolean mInit = false;
	
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mView = new AndroidSurfaceView(getApplication(), this);
        mInputSensors = new InputSensors( this );
        mCoreUtils = new CoreUtility( this );
        mFlurrySupport = new FlurrySupport(this);
        
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        setContentView(mView);
        //mView.setSystemUiVisibility(View.STATUS_BAR_HIDDEN);
        
        String assetFolder = getApplicationInfo().sourceDir;
        Log.d("GAMEJAVA", "App path = " + assetFolder);
		
		String saveFolder = getFilesDir().getAbsolutePath();
        Log.d("GAMEJAVA", "Save path = " + saveFolder);
		
        // set the zip/apk folder
        setAPK( assetFolder );
        // set the save folder		
		setSaveDirectory( saveFolder );
		// set tablet flag
		setTabletFlag( mCoreUtils.isTabletDevice() );
		// set retina flag
		setRetinaFlag( mCoreUtils.isRetinaDisplay() );		
    }

    @Override protected void onStart() {
    	super.onStart();
    	
    	if( mInit == false )
    	{
    		create();
    		mInit = true;
    	}
    	
    	if( mInit &&
    		mView.isInitialised() )
    	{
    		appInForeground(); 	
    	}
    }
    
    @Override protected void onPause() {	
        super.onPause();
           
        // accelerometer off
        mInputSensors.onPause();
        
        mView.onPause();
    }

    @Override protected void onResume() {        	
    	
    	super.onResume();
    	        
        // accelerometer on
        mInputSensors.onResume();  
        
        mView.onResume();  
    }    
    
    
    @Override protected void onStop() {
    	super.onStop(); 
    	
    	if( mInit &&
        	mView.isInitialised() )
    	{
    		appInBackground(); 	    
    	}
    }
    
    @Override protected void onDestroy() {
        super.onDestroy();
        
        mFlurrySupport.CloseFlurry();
        mInputSensors.onDestroy();        
        destroy();
        
        mInit = false;
    }  
    
    @Override public void onBackPressed()
    {
    	//super.onBackPressed();
    	final AlertDialog.Builder builder = new AlertDialog.Builder(this);
    	
    	this.runOnUiThread(new Runnable() {
			  public void run() {    	
		        builder.setCancelable(true);
		        builder.setTitle(R.string.app_quit);
		        builder.setInverseBackgroundForced(true);
		        builder.setPositiveButton(R.string.dialog_yes,
		                new DialogInterface.OnClickListener() {
		                    public void onClick(DialogInterface dialog,
		                            int which) {
		                    	dialog.dismiss();
		                    	Prisoner84Activity.this.finish();
		                    	mFlurrySupport.CloseFlurry();
		                        mInputSensors.onDestroy();        
		                        destroy();
		                        
		                        mInit = false;
		                    }
		                });
		        builder.setNegativeButton(R.string.dialog_no,
		                new DialogInterface.OnClickListener() {
		                    public void onClick(DialogInterface dialog,
		                            int which) {
		                        dialog.dismiss();
		                    }
		                });
		        AlertDialog alert = builder.create();
		        alert.show();   
			  }
    	});			        
    }
    
    static 
    {
    	System.loadLibrary("openal");
        System.loadLibrary("prisoner84");
    }    
}