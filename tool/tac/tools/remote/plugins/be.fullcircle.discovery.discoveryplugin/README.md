DiscoveryPlugin
======

> The DiscoveryPlugin adds support to send broadcast messages and receive a response.

PhoneGap Build
--------------
To include the StatusBar plugin in your PhoneGap Build application, add this to your config.xml:

    <gap:plugin name="be.fullcircle.discovery.discoveryplugin" />
 
Methods
-------

#### discovery.js

- startDiscovery : Sets broadcastIP, port and message to be used to communicate. Broadcasts the message over the network every three seconds.
- stopDiscovery : Stops scanning for devices.


Permissions
-----------

#### Following permissions will be automatically added to the AndroidManifest

<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" />
<uses-permission android:name="android.permission.ACCESS_WIFI_STATE" />


Description
-----------
Sends broadcast messages using IP, PORT and BROADCAST MESSAGE. Receiving devices should reply to ip where broadcast is send from.






Supported Platforms
-------------------

- iOS