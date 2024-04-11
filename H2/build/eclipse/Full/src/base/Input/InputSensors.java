package base.Input;

import android.app.Activity;
import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;


public class InputSensors implements SensorEventListener {
		
	public static native void SetAccelerometerValues( float x, float y, float z );
	
    private static SensorManager mSensorManager;
    private static Sensor mAccelerometer;
    private static SensorEventListener mListener;
    
    private static boolean mAccelerometerState;
    private static float mAccFrequency;
	
    public InputSensors(Activity activity) {
        
        // accelerometer
        mSensorManager = (SensorManager) activity.getSystemService(Context.SENSOR_SERVICE);
        mAccelerometer = mSensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        mListener = this;
        
        mAccelerometerState = false;
    }

    public void onPause() {
        
        // accelerometer off
        mSensorManager.unregisterListener(this);
    }

    public void onResume() {
        
        // accelerometer on
    	if( mAccelerometer != null && mAccelerometerState == true )
    	{
    		mSensorManager.registerListener(this, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);        
    	}
    }    
    
    public void onDestroy() {
        // accelerometer off
    	if( mAccelerometer != null )
    		mSensorManager.unregisterListener(this);
    }  
    
	@Override
	public void onAccuracyChanged(Sensor arg0, int arg1) {
		
	}
	
	@Override
	public void onSensorChanged(SensorEvent event) {

		if( mAccelerometer != null && mAccelerometerState == true )
		{		
			SetAccelerometerValues( event.values[0], event.values[1], event.values[2] );
		}
	}    
	
	// 
	public static void SetAccelerometerState(boolean state, float val) {
		
		if( state )
		{
	        // accelerometer on
	    	if( mAccelerometer != null && mAccelerometerState != state )
	    	{
	    		mSensorManager.registerListener(mListener, mAccelerometer, SensorManager.SENSOR_DELAY_NORMAL);        
	    	}			
		}
		else 
		{
	        // accelerometer off
	    	if( mAccelerometer != null && mAccelerometerState != state )
	    		mSensorManager.unregisterListener(mListener);			
		}
		
		mAccelerometerState = state;
		mAccFrequency = val;		
	}
}
