package base.Core;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.util.DisplayMetrics;
import android.view.Display;

public class CoreUtility {
	
	static Activity mActivity;
	
	public CoreUtility(Activity activity) {
    	mActivity = activity;
    }
    
    /**
     * Checks if the screen size is equal or above given length
     * @param activity activity screen
     * @param screen_size diagonal size of screen, for example 7.0 inches
     * @return True if its equal or above, else false
     */
    public static boolean checkScreenSize(Activity activity, double screen_size)
    {
        Display display = activity.getWindowManager().getDefaultDisplay();
        DisplayMetrics displayMetrics = new DisplayMetrics();
        display.getMetrics(displayMetrics);

        int width = displayMetrics.widthPixels / displayMetrics.densityDpi;
        int height = displayMetrics.heightPixels / displayMetrics.densityDpi;

        double screenDiagonal = Math.sqrt( width * width + height * height );
        return (screenDiagonal >= screen_size );
    }
    
	public boolean isTabletDevice() 
	{
		// 6 inch or bigger screen is seen as a tablet
		return (checkScreenSize(mActivity, 6.0));
		
	    // Verifies if the generalised Size of the device is XLARGE to be
	    // considered a Tablet
	    /*boolean large = ((mActivity.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_LARGE);
	    boolean xlarge = ((mActivity.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_XLARGE);

	    // If XLarge, checks if the generalised Density is at least MDPI
	    // (160dpi)
	    if (large || xlarge) 
	    {
	        DisplayMetrics metrics = new DisplayMetrics();
	        mActivity.getWindowManager().getDefaultDisplay().getMetrics(metrics);

	        // MDPI=160, DEFAULT=160, DENSITY_HIGH=240, DENSITY_MEDIUM=160,
	        // DENSITY_TV=213, DENSITY_XHIGH=320
	        if (metrics.densityDpi >= DisplayMetrics.DENSITY_HIGH) 
	        {
	            // Yes, this is a tablet!
	            return true;
	        }
	    }

	    // No, this is not a tablet!
	    return false;*/
	}
	
	public boolean isRetinaDisplay() 
	{
	    // Verifies if the generalised Size of the device is XLARGE to be
	    // considered a Tablet
	    boolean large = ((mActivity.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_LARGE);
	    boolean xlarge = ((mActivity.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) == Configuration.SCREENLAYOUT_SIZE_XLARGE);

	    // If XLarge, checks if the generalised Density is at least MDPI
	    // (160dpi)
	    if (large || xlarge) 
	    {
	        DisplayMetrics metrics = new DisplayMetrics();
	        mActivity.getWindowManager().getDefaultDisplay().getMetrics(metrics);

	        // MDPI=160, DEFAULT=160, DENSITY_HIGH=240, DENSITY_MEDIUM=160,
	        // DENSITY_TV=213, DENSITY_XHIGH=320
	        if (metrics.densityDpi >= DisplayMetrics.DENSITY_MEDIUM) 
	        {
	            // Yes, this is a retina!
	            return true;
	        }
	    }

	    // No, this is not a tablet!
	    return false;
	}	
	
	public boolean isOnline() 
	{
	    ConnectivityManager cm = (ConnectivityManager) mActivity.getSystemService(Context.CONNECTIVITY_SERVICE);

	    return cm.getActiveNetworkInfo() != null && 
	       cm.getActiveNetworkInfo().isConnectedOrConnecting();
	}
	
	public static void OpenWebLink(String url)
	{
		Uri uri = Uri.parse( url );
		mActivity.startActivity( new Intent( Intent.ACTION_VIEW, uri ) );
	}	
	
}
