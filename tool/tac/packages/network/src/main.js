"use strict";

var assert = require('assert');
var NetworkImpl = require('./impl/nm');

function NetworkApi(impl) {

	//	Public API
	var self = this;

	//	Setup impl
	assert(impl);

	/**
	 * Events
	 *   'changed'
	 *     Se emite cuando se conecta o desconecta de la red
	 *     @param error Error en caso de que el browser se cierre con una condicion de error
	 *     @param event
	 *                 En caso de conexion:
	 *                                      event.type == 'connected'
	 *                                      event.state == isConnected()
	 *                                      event.connection == activeConnection()
	 *                 En caso de perdida de una conexion:
	 *                                      event.type == 'connectionLost'
	 *                                      event.state == isConnected()
	 *                                      event.connection == connectionLostObject
	 *
	 *                                      Este signal se env?a ante la p?rdida de cualquier conexion,
	 *                                      para saber si se mantiene conectado globalmente, hay que chequear
	 *                                      el valor de event.state (que indica el estado de conexion global)
	 */
	self.on = function( name, callback ) {
		impl.on(name, callback);
	};
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	self.once = function( name, callback ) {
		impl.once(name, callback);
	};
	self.once.isSignal = true;

	self.removeListener = function( name, callback ) {
		impl.removeListener(name, callback);
	};

	//	Returns if is connected
	self.isConnected = function( cb ) {
		impl.isConnected( cb );
	};
	self.isConnected.isAsync = true;

	//	Returns the active connection
	self.activeConnection = function( cb ) {
		impl.activeConnection( cb );
	};
	self.activeConnection.isAsync = true;

	//	Returns the connection attached to a given device
	self.getConnection = function( device, cb ) {
		impl.getConnection( device, cb );
	};
	self.getConnection.isAsync = true;

	//	Returns a array of available connections
	self.getConnections = function( cb ) {
		impl.getConnections( cb );
	};
	self.getConnections.isAsync = true;

	//	Returns a array of available wifi networks
	self.getWifiList = function( cb ) {
		impl.getWifiList( cb );
	};
	self.getWifiList.isAsync = true;

	//	Connect
	self.connectWifi = function( opts, cb ) {
		impl.connectWifi( opts, cb );
	};
	self.connectWifi.isAsync = true;

	//  Disconnect Access Point
	self.disconnectWifi = function( ssid, callback ) {
		impl.disconnectWifi( ssid, callback );
	};
	self.disconnectWifi.isAsync = true;

        self.dump = function() {
                return impl.dumpState();
        };

	self.getWebAPI = function() {
		return {
			'name': 'network',
			'public': [
				'on',
				'once',
				'removeListener',
				'isConnected',
				'activeConnection',
				'getConnection',
				'getConnections',
				'getWifiList'
			],
			'private': [
				'connectWifi',
				'disconnectWifi'
			]
		};
	};

	return self;
}

//	Service entry point
function Network() {
	var _impl = null;
	var self = this || {};

	self.onStart = function(cb) {
		_impl = new NetworkImpl();
		_impl.init(NetworkApi, cb);
	};

	self.onStop = function(cb) {
		assert(cb);
		_impl.fin(cb);
	};

	return Object.freeze(self);
}

module.exports = Network;
