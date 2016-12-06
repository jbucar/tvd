'use strict';

var chai = require("chai");
chai.config.includeStack = true;
var assert = chai.assert;
var Mocks = require('mocks');
var dbus_native = require('dbus-native');
var Network = require('../src/main');
var dummy_system_bus = require('./dummy_system_bus');
var SystemBus = dummy_system_bus.SystemBus;

var good_ssid_wpa = 'Stream-WiFi-WPA';
var good_ssid_wpa_hidden = 'Stream-WiFi-WPA-HIDDEN';
var good_ssid_wep = 'Stream-WiFi-WEP';
var good_ssid_opn = 'Stream-WiFi-OPN';
var bad_ssid = '0f68dfae-4a94-4cf6-a29a-a4dd0779a249'; // random uuid

var bad_wifi_pass = 'stream0303458'; //an invalid pass long version
var good_wifi_pass = 'stream0303456'; //valid pass

describe('Network', function() {

	beforeEach(function() {
		Mocks.init('verbose');
	});

	afterEach(function() {
		Mocks.fin();
	});

	describe('constructor', function() {
		it('should construct a Network service', function() {
			assert.isObject(new Network());
		});
	});

	describe('constructor', function() {
		it('should construct a Network service without using new', function() {
			assert.isObject(Network());
		});
	});

	describe('onStart/onStop', function() {
		var network = null;
		var adapter = null;

		beforeEach(function() {
			adapter = new Mocks.ServiceAdapter();
			network = new Network();
		});

		afterEach( function () {
			network = null;
		});

		it('should return api on start', function(done) {
			network.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				var web = api.getWebAPI();
				assert.isObject(web);
				assert(web.name = 'networkmgr');
				assert.isArray( web['public'] );
				web['public'].forEach(function(fnc) {
					assert.isFunction( api[fnc] );
				});
				assert.isArray( web['private'] );
				web['private'].forEach(function(fnc) {
					assert.isFunction( api[fnc] );
				});
				network.onStop(done);
			});
		});

		it('should start/stop/start', function(done) {
			network.onStart(function(err,api) {
				assert.equal(err);
				assert.isObject(api);
				network.onStop(function() {
					network.onStart(function(err1,api1) {
						assert.equal(err1);
						assert.isObject(api1);
						assert( api1 !== api );
						network.onStop(done);
					});
				});
			});
		});

		function dbusInitError( cb ) {
			var sysBus = null;
			var old = dbus_native.systemBus;
			dbus_native.systemBus = function() {
				sysBus = new SystemBus();
				return sysBus;
			};

			network.onStart(function(err/*,api*/) {
				assert.equal(err,'Error: Error trying to setup dbus: err=pepe');
				dbus_native.systemBus = old;
				network.onStop(cb);
			});
			sysBus.events.emit( 'error', new Error('pepe') );
		}

		function networkIfaceError ( cb ) {
			var sysBus = null;
			var old = dbus_native.systemBus;
			dbus_native.systemBus = function() {
				sysBus = new SystemBus('org.freedesktop.NetworkManager');
				return sysBus;
			};

			network.onStart(function(err/*,api*/) {
				assert.equal(err,'Error: Error trying to setup network manager events: err=pepe');
				dbus_native.systemBus = old;
				network.onStop(cb);
			});

			sysBus.events.emit( 'connect' );
		}

		function settingsIfaceError ( cb ) {
			var sysBus = null;
			var old = dbus_native.systemBus;
			dbus_native.systemBus = function() {
				sysBus = new SystemBus('org.freedesktop.NetworkManager.Settings');
				return sysBus;
			};

			network.onStart(function(err/*,api*/) {
				assert.equal(err,'Error: Error trying to setup network manager settings events: err=pepe');
				dbus_native.systemBus = old;
				network.onStop(cb);
			});

			sysBus.events.emit( 'connect' );
		}

		function deviceIfaceError ( cb ) {
			var sysBus = null;
			var old = dbus_native.systemBus;
			dbus_native.systemBus = function() {
				sysBus = new SystemBus('org.freedesktop.NetworkManager.Device');
				return sysBus;
			};

			network.onStart(function(err/*,api*/) {
				assert.equal(err,'Error: Error trying to setup network manager device events: err=pepe');
				dbus_native.systemBus = old;
				network.onStop(cb);
			});

			sysBus.events.emit( 'connect' );
		}

		it('should handle error on NM init/fin', function(done) {
			dbusInitError( function() {
				networkIfaceError( function() {
					settingsIfaceError( done );
				});
			});
		});
	});

	describe('api', function() {
		var network = null;
		var api = null;
		var adapter = null;
		var sysBus = null;
		var old = null;

		function startSysBus() {
			sysBus.events.emit( 'connect' );
		}

		function connect_good_SysBus() {
			sysBus.service_delay_emmit( [
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 40 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 50 ],
				[ '/org/freedesktop/NetworkManager/Settings', 'org.freedesktop.NetworkManager.Settings', 'NewConnection', '/org/freedesktop/NetworkManager/Settings/140' ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 70 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 80 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 90 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 100 ],
				[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'StateChanged',  50 ],
				[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'StateChanged',  60 ],
				[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'StateChanged',  70 ],
				[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'PropertiesChanged', [["State",[[{"type":"u","child":[]}],[70]]],["PrimaryConnection",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/ActiveConnection/39"]]],["Metered",[[{"type":"u","child":[]}],[4]]],["PrimaryConnectionType",[[{"type":"s","child":[]}],["802-11-wireless"]]],["ActivatingConnection",[[{"type":"o","child":[]}],["/"]]],["Connectivity",[[{"type":"u","child":[]}],[4]]]] ],
			],200);
		}

		function connect_bad_SysBus() {
			sysBus.service_delay_emmit( [
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 40 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 50 ],
				[ '/org/freedesktop/NetworkManager/Settings', 'org.freedesktop.NetworkManager.Settings', 'NewConnection', '/org/freedesktop/NetworkManager/Settings/140' ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 60 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 120 ],
				[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'StateChanged', 20 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 30 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 40 ],
				[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 50 ],
			],200);
		}

		function connection_options(ssid, pass) {
			return {
				ssid: ssid,
				pass: pass,
				autoconnect: false
			};
		}

		function hidden_connection_options(ssid, pass) {
			var opts = connection_options(ssid, pass);
			opts.mode = 'wpa';
			return opts;
		}

		function changed_callback_connected( done ) {
			return function( event ) {
				assert.isDefined( event.name );
				assert.isDefined( event.type );
				assert.isDefined( event.state );
				if(event.state==='connected') {
					api.getConnection( event.name, function ( err, res ) {
						if(res.type === 'wireless') {
							done();
						}
					});
				}
			};
		}

		function changed_connection_callback_disconnected( done ) {
			return function( event ) {
				assert.isDefined( event.name );
				assert.isDefined( event.type );
				assert.isDefined( event.state );
				if(event.state==='disconnected') {
					done();
				}
			};
		}

		function changed_active_callback_disconnected( done )  {
			return function( event ) {
				if(event === undefined) {
					done();
				}
			};
		}

		function changed_callback_error( done ) {
			return function( event ) {
				assert.isDefined( event.type );
				assert.isDefined( event.state );
				if(event.state==='error') {
					done();
				}
			};
		}

		function init( cb, sysbus_map ) {
			old = dbus_native.systemBus;
			dbus_native.systemBus = function() {
				sysBus = new SystemBus( undefined, sysbus_map );
				return sysBus;
			};

			adapter = new Mocks.ServiceAdapter();
			network = new Network(adapter);
			network.onStart(function(err,impl) {
				assert.equal(err);
				api = impl;
				assert.isObject(api);
				cb();
			});
		}

		afterEach( function (done) {
			network.onStop(function() {
				network = null;
				api = null;
				dbus_native.systemBus = old;
				done();
			});
		});

		describe('events', function() {
			it('should register/deregister for every event notification', function(done) {
				init(function() {
					var cb = changed_callback_connected(done);
					api.on('connection', cb);
					api.removeListener('connection', cb);
					done();
				});
				startSysBus();
			});
			it('should register/deregister for only one event notification', function(done) {
				init(function() {
					var cb = changed_callback_connected(done);
					api.once('connection', cb);
					api.removeListener('connection', cb);
					done();
				});
				startSysBus();
			});
		});

		describe('no isConnected', function() {
			it('should return that the network stack is not connected', function(done) {
				init(function() {
					api.isConnected( function( err, res ) {
						assert.isUndefined( err );
						assert.isFalse( res );
						done();
					});
				});
				startSysBus();
			});
		});

		describe('no activeConnection', function() {
			it('should return undefined as the active connection', function(done) {
				init(function() {
					api.activeConnection( function( err, res ) {
						assert.isUndefined( err );
						assert.isUndefined( res );
						done();
					});
				});
				startSysBus();
			});
		});

		describe('dump no activeConnection', function() {
			it('should return undefined dump', function(done) {
				init(function() {
					var dump = api.dump();
					assert.isUndefined( dump );
					done();
				});
				startSysBus();
			});
		});

		describe('getWifiList', function() {
			it('should return wifi list', function(done) {
				init(function() {
					api.getWifiList( function( err, res ) {
						assert.isUndefined( err );
						assert.isDefined( res );
						done();
					});
				});
				startSysBus();
			});
		});

		describe('connectWifi', function() {
			this.timeout(20000);

			it('should not connect to ap (invalid password)', function(done) {
				init(function() {
					api.on('connection', changed_callback_error(done));
					api.connectWifi( connection_options(good_ssid_wpa, bad_wifi_pass), function( err ) {
						assert.isUndefined( err );
						connect_bad_SysBus();
					});
				});
				startSysBus();
			});
			it('should not connect to ap (invalid ssid)', function(done) {
				init(function() {
					api.on('connection', changed_callback_error(done));
					api.connectWifi( connection_options(bad_ssid, good_wifi_pass), function( err ) {
						assert.isDefined( err );
					});
				});
				startSysBus();
			});
			it('should not connect to ap (short ssid)', function(done) {
				init(function() {
					api.on('connection', changed_callback_error(done));
					api.connectWifi( connection_options(good_ssid_opn, good_wifi_pass), function( err ) {
						assert.isDefined( err );
					});
				}, dummy_system_bus.failed_connect);
				startSysBus();
			});
			it('should not connect to a hidden ap (invalid ssid)', function(done) {
				init(function() {
					api.on('connection', changed_callback_error(done));
					api.connectWifi( hidden_connection_options(bad_ssid, good_wifi_pass), function( err ) {
						assert.isDefined( err );
					});
				}, dummy_system_bus.failed_connect);
				startSysBus();
			});
			it('should connect to open ap and test "active" signal', function(done) {
				init(function() {
					api.on('active', function( active_connection ) {
						assert.isDefined( active_connection );
						assert.isTrue( active_connection.indexOf('wlan') > -1 );
						done();
					});
					api.connectWifi( connection_options(good_ssid_opn), function( err ) {
						assert.isUndefined( err );
						connect_good_SysBus();
					});
				});
				startSysBus();
			});
			it('should connect to open ap', function(done) {
				init(function() {
					api.on('connection', changed_callback_connected(done));
					api.connectWifi( connection_options(good_ssid_opn), function( err ) {
						assert.isUndefined( err );
						connect_good_SysBus();
					});
				});
				startSysBus();
			});
			it('should connect to open ap (no TAC_Wireless present)', function(done) {
				init(function() {
					api.on('connection', changed_callback_connected(done));
					api.connectWifi( connection_options(good_ssid_opn), function( err ) {
						assert.isUndefined( err );
						connect_good_SysBus();
					});
				}, dummy_system_bus.missing_tac_wireless);
				startSysBus();
			});
			it('should return data dump', function(done) {
				init(function() {
					api.on('active', function( connection ) {
						var dump = api.dump();
						if( connection === undefined ) {
							assert.isUndefined( dump );
						} else {
							assert.isDefined( dump );
							assert.isTrue( dump.name  === good_ssid_opn );
							assert.isDefined( dump.id );
							assert.isTrue( dump.type === 'wireless' );
							assert.isDefined( dump.device );
							assert.isDefined( dump.ip );
							assert.isDefined( dump.mac );
							assert.isDefined( dump.gw );
							assert.isDefined( dump.dns );
							assert.isTrue( dump.state === 'connected' );
							done();
						}
					});
					api.connectWifi( connection_options(good_ssid_opn), function( err ) {
						connect_good_SysBus();
						assert.isUndefined( err );
					});
				});
				startSysBus();
			});
			it('should connect to wep ap', function(done) {
				init(function() {
					api.on('connection', changed_callback_connected(done));
					api.connectWifi( connection_options(good_ssid_wep, good_wifi_pass), function( err ) {
						assert.isUndefined( err );
						api.connectWifi( connection_options(good_ssid_opn), function( err ) {
							assert.isUndefined( err );
							connect_good_SysBus();
						});

					});
				});
				startSysBus();
			});
			it('should connect to a hidden wpa ap', function(done) {
				init(function() {
					api.on('connection', changed_callback_connected(done));
					api.connectWifi( hidden_connection_options(good_ssid_wpa_hidden, good_wifi_pass), function( err ) {
						assert.isUndefined( err );
						connect_good_SysBus();
					});
				});
				startSysBus();
			});
			it('should connect to wpa ap', function(done) {
				init(function() {
					api.on('connection', changed_callback_connected(done));
					api.connectWifi( connection_options(good_ssid_wpa, good_wifi_pass), function( err ) {
						assert.isUndefined( err );
						connect_good_SysBus();
					});
				});
				startSysBus();
			});
		});

		describe('isConnected', function() {
			it('should return that the network stack is connected', function(done) {
				init(function() {
					api.isConnected( function( err, res ) {
						assert.isUndefined( err );
						assert.isTrue( res );
						done();
					});
				}, dummy_system_bus.connected_map );
				startSysBus();
			});
		});

		describe('activeConnection', function() {
			it('should return a defined active connection', function(done) {
				init(function() {
					api.activeConnection( function( err, res ) {
						assert.isUndefined( err );
						assert.isDefined( res );
						assert.isDefined( res.name );
						assert.isDefined( res.id );
						assert.isDefined( res.type );
						assert.isDefined( res.device );
						assert.isDefined( res.ip );
						assert.isDefined( res.mac );
						assert.isDefined( res.gw );
						assert.isDefined( res.dns );
						assert.isDefined( res.state );
						done();
					});
				}, dummy_system_bus.connected_map );
				startSysBus();
			});
			it('should return a defined active connection (no ip4 address)', function(done) {
				init(function() {
					api.activeConnection( function( err, res ) {
						assert.isUndefined( err );
						assert.isDefined( res );
						assert.isDefined( res.name );
						assert.isDefined( res.id );
						assert.isDefined( res.type );
						assert.isDefined( res.device );
						assert.isDefined( res.ip );
						assert.isDefined( res.mac );
						assert.isDefined( res.gw );
						assert.isDefined( res.dns );
						assert.isDefined( res.state );
						done();
					});
				}, dummy_system_bus.no_ip_default );
				startSysBus();
			});
		});

		describe('getConnections', function() {
			it('should return active connections', function(done) {
				init(function() {
					api.getConnections( function( err, res ) {
						assert.isUndefined( err );
						assert.isTrue( res.length >= 0 );
						res.forEach( function( connection ) {
							assert.isDefined( connection.name );
							assert.isDefined( connection.id );
							assert.isDefined( connection.type );
							assert.isDefined( connection.device );
							assert.isDefined( connection.ip );
							assert.isDefined( connection.mac );
							assert.isDefined( connection.gw );
							assert.isDefined( connection.dns );
							assert.isDefined( connection.state );
						});
						done();
					});
				}, dummy_system_bus.connected_map );
				startSysBus();
			});
		});

		describe('disconnectWifi', function() {
			it('should not disconnect wifi (invalid ssid)', function(done) {
				init(function() {
					api.disconnectWifi( bad_ssid, function( err, res ) {
						assert.isDefined( err );
						assert.isUndefined( res );
						done();
					});
				});
				startSysBus();
			});
			it('should disconnect wifi', function(done) {
				init(function() {
					api.on('connection', changed_connection_callback_disconnected(done));
					api.disconnectWifi( good_ssid_opn, function( err ) {
						assert.isUndefined( err );
						sysBus.service_delay_emmit( [
							[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'PropertiesChanged', [["Metered",[[{"type":"u","child":[]}],[0]]],["State",[[{"type":"u","child":[]}],[20]]],["PrimaryConnectionType",[[{"type":"s","child":[]}],[""]]],["ActiveConnections",[[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/ActiveConnection/15"]]]]] ],
							[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 20 ],
							[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'StateChanged',  20 ]
						],200);
					});
				});
				startSysBus();
			});
			it('should disconnect wifi (active signal)', function(done) {
				init(function() {
					api.on('active', changed_active_callback_disconnected(done));
					api.disconnectWifi( good_ssid_opn, function( err ) {
						assert.isUndefined( err );
						sysBus.service_delay_emmit( [
							[ '/org/freedesktop/NetworkManager/Devices/1', 'org.freedesktop.NetworkManager.Device', 'StateChanged', 20 ],
							[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'StateChanged',  20 ],
							[ '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'PropertiesChanged', [["Metered",[[{"type":"u","child":[]}],[0]]],["PrimaryConnection",[[{"type":"o","child":[]}],["/"]]],["State",[[{"type":"u","child":[]}],[20]]],["PrimaryConnectionType",[[{"type":"s","child":[]}],[""]]],["ActiveConnections",[[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/ActiveConnection/15"]]]]] ]
						],200);
					});
				});
				startSysBus();
			});
		});
	});
});
