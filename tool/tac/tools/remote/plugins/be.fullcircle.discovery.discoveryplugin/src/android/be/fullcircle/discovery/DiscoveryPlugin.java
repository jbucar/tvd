package be.fullcircle.discovery;

import org.apache.cordova.CordovaArgs;
import org.apache.cordova.CordovaPlugin;
import org.apache.cordova.CallbackContext;
import org.apache.cordova.CordovaInterface;
import org.apache.cordova.PluginResult;
import org.apache.cordova.CordovaWebView;

import org.json.JSONObject;
import org.json.JSONArray;
import org.json.JSONException;
import android.app.Activity;
import android.content.Intent;
import be.fullcircle.discovery.Discoverer;
import android.net.wifi.WifiManager;
import android.content.Context;
import java.lang.InterruptedException;

public class DiscoveryPlugin extends CordovaPlugin {
	public static final String START_DISCOVERY_SERVICE = "cordovaDiscoveryService";
	public static final String STOP_DISCOVERY_SERVICE = "cordovaStopDiscoveryService";
	public Discoverer d = null;
	public static final Boolean DEBUG = true;
	private CallbackContext connectionCallbackContext;
	
	@Override
	public boolean execute(String action, JSONArray args, CallbackContext callbackContext) throws JSONException {
		try {
		    if (START_DISCOVERY_SERVICE.equals(action)) { 
	    		JSONObject arg_object = args.getJSONObject(0);
	    		String broadcastIP = arg_object.getString("broadcastIP");
	    		String broadcastPort = arg_object.getString("broadcastPort");
	    		String broadcastMessage = arg_object.getString("broadcastMessage");
		           
		    	if(d == null || d.bKeepLooking == false)
		    	{
		    		Context ctx = this.cordova.getActivity().getApplicationContext();
		    		WifiManager wifi = (WifiManager) ctx.getSystemService(Context.WIFI_SERVICE);
		    		d = new Discoverer(wifi, this, broadcastIP, broadcastPort, broadcastMessage);	    		
		    		d.start();
		    		if(DEBUG)
		    			sendValue("Scanning Thread (Re)Started");
		    				    		
		    	}else{
		    		if(d != null && d.bKeepLooking == true)
		    		{
		    			if(DEBUG)
		    				sendValue("Thread is not interrupted?");
		    		}
		    	}
		    	
	    		/*PluginResult pluginResult = new PluginResult(PluginResult.Status.OK, "Discovery Started");
	    		pluginResult.setKeepCallback(true);
	    		callbackContext.sendPluginResult(pluginResult);*/

	    		this.connectionCallbackContext = callbackContext;

                if(DEBUG)
                    sendValue("Broadcasting Message... Scanning for devices");

			    return true;
		    }else if(STOP_DISCOVERY_SERVICE.equals(action))
		    {
		    	if(DEBUG)
		    		sendValue("Trying to interrupt Thread");
		    	
		    	try{
		    		if (d != null) {
			    		d.interrupt();
			    		d.join();
			    		if(DEBUG)
			    			sendValue("Thread successfully stopped.");
			        }else{
			        	if(DEBUG)
			        		sendValue("No Thread was running");
			        }
		    	}catch (Exception e)
		    	{
		    		if(DEBUG)
		    			sendValue("Something went wrong stopping this thread: "+ e.toString());
		    		
		    		callbackContext.error("Something went wrong stopping this thread: "+ e.toString());
		    	}
		    	
		    	callbackContext.success();
			    return true;
		    }
		    callbackContext.error("Invalid action");
		    return false;
		} catch(Exception e) {
		    System.err.println("Exception: " + e.getMessage());
		    callbackContext.error(e.getMessage());
		    return false;
		}
	}
	
	public void foundDevice(String value)
	{
	    /*String js = String.format("console.log('found device:" + value + "')");
	    this.webView.sendJavascript(js);
	    js = String.format("window.angularBridge(" + value + ")");
	    this.webView.sendJavascript(js);*/

		PluginResult pluginResult = new PluginResult(PluginResult.Status.OK, value);
		pluginResult.setKeepCallback(true);
		this.connectionCallbackContext.sendPluginResult(pluginResult);
	}
	
	public void sendValue(String value1) {		
	    JSONObject data = new JSONObject();
	    try {
	      data.put("value1", value1);
	    } catch (JSONException e) {
	    	 String js = String.format("console.log('CommTest: " + e.getMessage() + "')");
	      //Log.e("CommTest", e.getMessage());
	    	this.webView.sendJavascript(js);
	    }
	    
	    String js = String.format("console.log('SENDVALUE: " + data.toString() + "')");
	    this.webView.sendJavascript(js);
	 }
}
