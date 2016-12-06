package be.fullcircle.discovery;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;
import android.net.DhcpInfo;
import android.net.wifi.WifiManager;
import android.util.Log;
import be.fullcircle.discovery.DiscoveryPlugin;

import java.lang.InterruptedException;
import java.lang.Object;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;


//import android.util.Log;

/*
 * This class tries to send a broadcast UDP packet over your wifi network to discover the renson service. 
 */

public class Discoverer extends Thread {
  private static final String TAG = "Discovery";
  private static final int TIMEOUT_MS = 3000;

  private WifiManager mWifi;

  private String broadcastIP;
  private String broadcastMessage;
  private int broadcastPort;
  
  private DiscoveryPlugin dp;
  
  public boolean bKeepLooking = true;
  boolean interrupted = false;

  interface DiscoveryReceiver {
    void addAnnouncedServers(InetAddress[] host, int port[]);
  }

  Discoverer(WifiManager wifi, DiscoveryPlugin dp, String broadcastIP, String broadcastPort, String broadcastMessage) {
    this.mWifi = wifi;
    this.dp = dp;
    this.broadcastIP = broadcastIP;
    this.broadcastPort = Integer.parseInt(broadcastPort);
    this.broadcastMessage = broadcastMessage;
  }

  public void run() {
    try {
      if(DiscoveryPlugin.DEBUG)
        dp.sendValue("Running thread...");
      DatagramSocket socket = new DatagramSocket(broadcastPort);
      socket.setBroadcast(true);
      socket.setSoTimeout(TIMEOUT_MS);
      
      while(!Thread.currentThread().isInterrupted() && bKeepLooking)
      {
    	  sendDiscoveryRequest(socket);
          listenForResponses(socket);
          try {
        	    Thread.sleep(2000);
          } catch(InterruptedException e) {
        	  // this is executed when an exception (in this example InterruptedException) occurs
        	  Thread.currentThread().interrupt();
        	  if(DiscoveryPlugin.DEBUG)
        	    dp.sendValue("Interrupted exception error while sleep");
        	  interrupted = true;
        	  bKeepLooking = false;
        	  socket.close();
          }
      }
    } catch (IOException e) {
      Log.e(TAG, "Could not send discovery request", e);
      dp.sendValue("Could not send discovery request: " + e.toString());
    }
  }

  /**
   * Send a broadcast UDP packet containing a request for boxee services to
   * announce themselves.
   * 
   * @throws IOException
   */
  private void sendDiscoveryRequest(DatagramSocket socket) throws IOException {
    String data = broadcastMessage;
    if(DiscoveryPlugin.DEBUG)
    {
        Log.d(TAG, "Sending data " + data);
        dp.sendValue("Sending data " + data + " broadcast " + getBroadcastAddress() + " from " + socket.getLocalAddress());
    }

    DatagramPacket packet = new DatagramPacket(data.getBytes("UTF-8"), data.length(),
        getBroadcastAddress(), broadcastPort);
    socket.send(packet);
  }

  /**
   * Calculate the broadcast IP we need to send the packet along. If we send it
   * to 255.255.255.255, it never gets sent. I guess this has something to do
   * with the mobile network not wanting to do broadcast.
   */
  private InetAddress getBroadcastAddress() throws IOException {
    DhcpInfo dhcp = mWifi.getDhcpInfo();
    if (dhcp == null) {
      Log.d(TAG, "Could not get dhcp info");
      if(DiscoveryPlugin.DEBUG)
        dp.sendValue("Could not get dhcp info");
      return null;
    }

    int broadcast = (dhcp.ipAddress & dhcp.netmask) | ~dhcp.netmask;
    byte[] quads = new byte[4];
    for (int k = 0; k < 4; k++)
      quads[k] = (byte) ((broadcast >> k * 8) & 0xFF);

    InetAddress addr = InetAddress.getByName(broadcastIP);
    return addr;
  }

  /**
   * Listen on socket for responses, timing out after TIMEOUT_MS
   * 
   * @param socket
   *          socket on which the announcement request was sent
   * @throws IOException
   */
  private void listenForResponses(DatagramSocket socket) throws IOException {
    byte[] buf = new byte[1024];
    try {
      while (true) {
        DatagramPacket packet = new DatagramPacket(buf, buf.length);
        socket.receive(packet);
        String s = new String(packet.getData(), 0, packet.getLength());
        Log.d(TAG, "Received response " + s);
        if(!s.equals(broadcastMessage))
        {
            if(DiscoveryPlugin.DEBUG)
        	    dp.sendValue("Received response " + s  + " from " + packet.getAddress());
        }else
	        return;
        try {
        	 JSONObject jsonbject = new JSONObject(s);
             dp.foundDevice(s);
        } catch (JSONException ex) {
        	dp.sendValue("No valid json: " + ex.toString());
        }
      }
    } catch (SocketTimeoutException e) {
      if(DiscoveryPlugin.DEBUG)
      {
          Log.d(TAG, "Receive timed out");
          dp.sendValue("Receive time out " + e.toString());
      }
    } catch (Exception ex) {
    	if(!(ex instanceof  SocketTimeoutException))
    	{
    		dp.sendValue("Socket Exception " + ex.toString());
    	}
    }
  }
}
