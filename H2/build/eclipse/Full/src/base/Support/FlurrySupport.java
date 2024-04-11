package base.Support;

import android.app.Activity;
import com.flurry.android.FlurryAgent;

public class FlurrySupport {
	
	static Activity mActivity;
	static String mAPIKey;
	
	public FlurrySupport(Activity activity) {
    	mActivity = activity;
    }
	
	public static void SetFlurryAPIKey( String apiKey )
	{
		mAPIKey = apiKey;
		FlurryAgent.onStartSession(mActivity, apiKey);
	}

	public static void LogFlurryEvent( String event )
	{
		FlurryAgent.logEvent(event);
	}
	
	public void CloseFlurry()
	{
		FlurryAgent.onEndSession(mActivity);
	}
}
