'use strict';

var assert = require('assert');
var dgram = require('dgram');

var HOST = '0.0.0.0';
var PORT = 49152;

function Discover( nm ) {
	var _server = null;
	var _params = null;
	var self = {};

	function saveParams( conn ) {
		if (!conn || !conn.ip) {
			log.verbose( 'remoteapi', 'Save params error; invalid connection!' );
			_params.host = null;
		}
		else {
			var pos = conn.ip.indexOf('/');
			if (pos !== -1) {
				_params.host = conn.ip.substr(0,pos);
			}
			else {
				_params.host = conn.ip;
			}

			log.verbose( 'remoteapi', 'Saving public IP: ip=%s', _params.host );
		}
	}

	function onActiveChanged() {
		nm.activeConnection( onActiveConnection );
	}

	function onActiveConnection( err, conn ) {
		if (!err) {
			saveParams( conn );
		}
	}

	function startDiscover() {
		log.verbose( 'remoteapi', 'Start Discover server' );

		_server = dgram.createSocket('udp4');

		_server.on('listening', function () {
			var address = _server.address();
			log.verbose( 'remoteapi', 'UDP server listening on: addr=%s, port=%d', address.address, address.port );
		});

		_server.on('message', function (rcvMsg, remote) {
			if (!_params.host) {
				log.warn( 'remoteapi', 'Ignoring message; configuration not ready: addr=%s, port=%d, msg=%s', remote.address, remote.port, rcvMsg);
				return;
			}

			var sndMsg = JSON.stringify(_params);

			var client = dgram.createSocket('udp4');
			client.send( sndMsg, 0, sndMsg.length, remote.port, remote.address, function(err) {
				if (err) {
					log.warn( 'remoteapi', 'Error trying to send IP to client: err=%s', err.message );
				}
				else {
					log.verbose( 'remoteapi', 'UDP message sent: addr=%s, port=%d, params=%j', remote.address, remote.port, _params );
				}
				client.close();
			});
		});

		_server.on('error', function(err) {
			log.warn( 'remoteapi', 'Error on Discover server: err=%s', err.message );
		});

		_server.bind(PORT, HOST);
	}

	self.start = function( port ) {
		assert(port);

		//	Start server
		_params = {
			port: port
		};
		startDiscover();

		//	Get active connection from network manager
		nm.on( 'active', onActiveChanged );
		nm.activeConnection( onActiveConnection );
	};

	self.stop = function(cb) {
		log.verbose( 'remoteapi', 'Stop discover server' );
		assert(cb);

		nm.removeListener( 'active', onActiveChanged );
		_server.close();
		cb();
	};

	return self;
}

// Export module hooks
module.exports = Discover;

