package uk.co.hiddengames.firewall;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
//import android.content.pm.PackageInfo;
//import android.content.pm.PackageManager.NameNotFoundException;
//import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import android.widget.RelativeLayout;
import base.Core.CoreUtility;
//import base.Support.MobclixSupport;
import base.Support.FlurrySupport;
import base.Input.InputSensors;
import base.Render.AndroidSurfaceView;
import uk.co.hiddengames.firewall.MusicMediaPlayer;

public class FirewallActivity extends Activity {

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
	public static native void TouchMoved( int index, float x, float y );
	public static native void RemoveTouch( int index );
	
    AndroidSurfaceView mView;
    private InputSensors mInputSensors;
	private CoreUtility mCoreUtils;
	//private static MobclixSupport mMobclixSupport;
	private FlurrySupport mFlurrySupport;
	private MusicMediaPlayer mMusicMediaPlayer;
	
	private boolean mInit = false;
	
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        mView = new AndroidSurfaceView(getApplication(), this);
        mInputSensors = new InputSensors( this );
        mCoreUtils = new CoreUtility( this );
        //mMobclixSupport = new MobclixSupport( this, RelativeLayout.ALIGN_PARENT_TOP, RelativeLayout.CENTER_HORIZONTAL );
        mFlurrySupport = new FlurrySupport(this);
        mMusicMediaPlayer = new MusicMediaPlayer( this );
        
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        setContentView(mView);
        
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
        
        mView.onPause();
        
        // accelerometer off
        mInputSensors.onPause();
    }

    @Override protected void onResume() {
  	
        super.onResume();
        
        mView.onResume();
        
        // accelerometer on
        mInputSensors.onResume();        
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
			                    @Override
			                    public void onClick(DialogInterface dialog,
			                            int which) {
			                    	dialog.dismiss();
			                    	FirewallActivity.this.finish();
			                        mFlurrySupport.CloseFlurry();
			                        mInputSensors.onDestroy();        
			                        destroy();
			                        
			                        mInit = false;                    	
			                    }
			                });
			        builder.setNegativeButton(R.string.dialog_no,
			                new DialogInterface.OnClickListener() {
			                    @Override
			                    public void onClick(DialogInterface dialog,
			                            int which) {
			                    	FirewallActivity.this.setVisible(true);
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
        System.loadLibrary("firewall");
    }
}