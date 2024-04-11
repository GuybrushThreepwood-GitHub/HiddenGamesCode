package uk.co.hiddengames.cabby;

import me.kiip.sdk.Kiip;
import me.kiip.sdk.Kiip.Callback;
import me.kiip.sdk.KiipFragmentCompat;
import me.kiip.sdk.Modal;
import me.kiip.sdk.Notification;
import me.kiip.sdk.Poptart;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.support.v4.app.FragmentActivity;

import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
//import android.content.pm.PackageInfo;
//import android.content.pm.PackageManager.NameNotFoundException;
//import android.content.res.AssetManager;
import android.os.Bundle;
import android.util.Log;
import base.Core.CoreUtility;
import base.Support.FlurrySupport;
import base.Input.InputSensors;
import base.Render.AndroidSurfaceView;
import base.Support.KiipSupport;
import uk.co.hiddengames.cabby.MusicMediaPlayer;

public class CabbyActivity extends FragmentActivity implements Poptart.OnShowListener, Poptart.OnDismissListener, Notification.OnShowListener, Notification.OnClickListener, Notification.OnDismissListener,
Modal.OnShowListener, Modal.OnDismissListener
{

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
	private FlurrySupport mFlurrySupport;
	private KiipSupport mKiipSupport;
	private boolean kiipIsEnabled = false;
	private MusicMediaPlayer mMusicMediaPlayer;
	
   	private boolean mInit = false;
    private final static String KIIP_TAG = "kiip_fragment_tag";

    private KiipFragmentCompat mKiipFragment;
    
    @Override protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        mView = new AndroidSurfaceView(getApplication(), this);
        mInputSensors = new InputSensors( this );
        mCoreUtils = new CoreUtility( this );
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

		kiipIsEnabled = mCoreUtils.isOnline();
		
        // Create or re-use KiipFragment.
        if (savedInstanceState != null) {
            mKiipFragment = (KiipFragmentCompat) getSupportFragmentManager().findFragmentByTag(KIIP_TAG);
        } else {
            mKiipFragment = new KiipFragmentCompat();
            getSupportFragmentManager().beginTransaction().add(mKiipFragment, KIIP_TAG).commit();
        }
        mKiipFragment.setOnShowListener(this);
        mKiipFragment.setOnDismissListener(this);
        
        mKiipSupport = new KiipSupport( kiipIsEnabled, this, mKiipFragment);
    }
    
    @Override protected void onStart() {
    	super.onStart();
    	
    	if( mInit == false )
    	{
    		create();
    		mInit = true;
    	}
    	
    	if( kiipIsEnabled )
    	{
	        // Must call startSession in every Activity#onStart.
	        Kiip.getInstance().startSession(new Callback() {
	            @Override
	            public void onFailed(Kiip kiip, Exception exception) {
	                //Log.e(TAG, "Failed to start session", exception);
	
	                onException(exception);
	            }
	
	            @Override
	            public void onFinished(Kiip kiip, Poptart poptart) {
	                onPoptart(poptart);
	            }
	        });
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
    	
    	if( kiipIsEnabled )
    	{
	        Kiip.getInstance().endSession(new Callback() {
	            @Override
	            public void onFailed(Kiip kiip, Exception exception) {
	                //Log.e(TAG, "Failed to end session", exception);
	            }
	
	            @Override
	            public void onFinished(Kiip kiip, Poptart poptart) {
	                // do nothing
	            }
	        });
    	}
    	
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
		                    	CabbyActivity.this.finish();
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
		                        dialog.dismiss();
		                    }
		                });
		        AlertDialog alert = builder.create();
		        alert.show();   
			  }
    	});	   	
    }
    
	/**
     * Helper method to show a Poptart from any Activity extending BaseFragmentActivity.
     *
     * @param poptart
     */
    public void onPoptart(Poptart poptart) 
    {
    	if( kiipIsEnabled )
    		mKiipFragment.showPoptart(poptart);
    }

    public void onException(Exception exception) 
    {
        Dialog dialog = new AlertDialog.Builder(this)
            .setTitle("Error")
            .setMessage(exception.getMessage())
            .setNegativeButton(android.R.string.ok, null)
            .create();
        dialog.show();
    }

    public void onShow(Poptart poptart)
    {
       //Log.d(TAG, "Poptart#onShow");

       final Notification notification = poptart.getNotification();
       final Modal modal = poptart.getModal();

       if (notification != null) {
           notification.setOnShowListener(this);
           notification.setOnClickListener(this);
           notification.setOnDismissListener(this);
       }

       if (modal != null) {
           modal.setOnShowListener(this);
           modal.setOnDismissListener(this);
       }
    }


    public void onDismiss(Poptart poptart) 
    {
        //Log.d(TAG, "Poptart#onDismiss");
    }

    @Override
    public void onShow(Notification notification) 
    {
        //Log.d(TAG, "Notification#onShow");
    }

    @Override
    public void onClick(Notification notification) 
    {
        //Log.d(TAG, "Notification#onClick");
    }

    @Override
    public void onDismiss(Notification notification) 
    {
        //Log.d(TAG, "Notification#onDismiss");
    }

    @Override
    public void onShow(Modal modal) 
    {
       //Log.d(TAG, "Modal#onShow");
    }

    @Override
    public void onDismiss(Modal modal) 
    {
        //Log.d(TAG, "Modal#onDismiss");
    } 
    
    static 
    {
    	System.loadLibrary("openal");
        System.loadLibrary("cabby");
    }

	@Override
	public void onDismiss(DialogInterface arg0) {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void onShow(DialogInterface dialog) {
		// TODO Auto-generated method stub
		
	}
}