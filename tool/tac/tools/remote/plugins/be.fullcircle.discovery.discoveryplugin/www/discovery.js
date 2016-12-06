var discovery = {
		startDiscovery: function(successCallback, errorCallback) {
	        cordova.exec(
	            successCallback, // will be called if device on the network replies
	            errorCallback, // callback for errors
	            'DiscoveryPlugin', // mapped to this native class
	            'cordovaDiscoveryService', // calling this action
	            [{                  // and this array of custom arguments to create our entry
	                "broadcastIP": "255.255.255.255",   //broadcast network ip
                    	"broadcastPort": "49152",
	                "broadcastMessage": "TAC_QUERY"   //broadcast message
	            }]
	        );
	     },
		stopDiscovery: function(successCallback, errorCallback) {
		    cordova.exec(
		        successCallback, // success callback function
		        errorCallback, // error callback function
		        'DiscoveryPlugin', // mapped to this native class
		        'cordovaStopDiscoveryService', // with this action name
		        [{}] //no arguments at the moment
		    ); 
		 }
}
module.exports = discovery;
