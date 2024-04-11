package base.Support;

import java.util.LinkedList;

import android.app.Activity;
import android.app.Application;
import android.content.Context;
import android.net.ConnectivityManager;
import android.provider.Settings;
import me.kiip.sdk.Kiip;
import me.kiip.sdk.Kiip.OnContentListener;
import me.kiip.sdk.Kiip.OnSwarmListener;
import me.kiip.sdk.Kiip.Callback;
import me.kiip.sdk.KiipFragmentCompat;
import me.kiip.sdk.Poptart;

public class KiipSupport extends Application implements OnSwarmListener, OnContentListener {
	
	static Activity mActivity;
	static boolean isAvailable;
	static String mAppKey;
	static String mSecretKey;
	static KiipFragmentCompat mKiipFragment;
	
	public KiipSupport(boolean isNetworkAvailable, Activity activity, KiipFragmentCompat kiipFragment) {
    	mActivity = activity;
    	mKiipFragment = kiipFragment;
    	
    	isAvailable = isNetworkAvailable;
    }
	
	public static void SetKiipKeys( String appKey, String secretKey )
	{
		mAppKey = appKey;
		mSecretKey = secretKey;

		if( isAvailable )
		{
			// Instantiate and set the shared Kiip instance
			Kiip kiip = Kiip.init(mActivity.getApplication(), mAppKey, mSecretKey);
			Kiip.setInstance(kiip);
		}
        //String mAndroidID = Settings.Secure.getString(mActivity.getContentResolver(), Settings.Secure.ANDROID_ID);
        //String mAndroidVersion = "Android " + android.os.Build.VERSION.RELEASE;
        //String mDevice = android.os.Build.MANUFACTURER + " " + android.os.Build.MODEL + "/" + android.os.Build.DEVICE;
	}

	public static void AwardKiipAchievement( final String achievementId )
	{
		if( isAvailable )
		{
			mActivity.runOnUiThread(new Runnable() {
				  public void run() {		
					Kiip.getInstance().saveMoment(achievementId, new Kiip.Callback() {
						@Override
						public void onFinished(Kiip kiip, Poptart reward) {
							mKiipFragment.showPoptart(reward);
						}
			
						@Override
						public void onFailed(Kiip kiip, Exception exception) {
							// handle failure
						}
					});
				  }
			});	 
		}
	}
    
    @Override
    public void onSwarm(Kiip kiip, String id) {
        //Log.d(TAG, "onSwarm id=" + id);
    }

    @Override
    public void onContent(Kiip kiip, String content, int quantity, String transactionId, String signature) {
        //Log.d(TAG, "onContent content=" + content + " quantity=" + quantity + " transactionId=" + transactionId + " signature=" + signature);
    }
}
