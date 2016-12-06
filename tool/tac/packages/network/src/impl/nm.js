"use strict";

var util = require('util');
var assert = require('assert');
var EventEmitter = require('events').EventEmitter;
var bPromise = require('bluebird');
var tvdutil = require('tvdutil');
var DBus = require('dbus');

var srvName = 'org.freedesktop.NetworkManager';
var settingName = 'TAC_Wireless';

//	Private constants
var NM_STATE_DISCONNECTED = 20; // There is no active network connection.
var NM_STATE_CONNECTED_LOCAL = 50; // A network device is connected, but there is only link-local connectivity.
var NM_STATE_CONNECTED_GLOBAL = 70; // A network device is connected, with global network connectivity.

// NM_802_11_AP_FLAGS
var NM_802_11_AP_FLAGS_PRIVACY = 0x1; // Access point supports privacy measures.

// NM_802_11_AP_SEC
var NM_802_11_AP_SEC_KEY_MGMT_PSK = 0x100; // Access point supports PSK key management.

var nm_device_state_reason = {};
nm_device_state_reason[0] = 'NM_DEVICE_STATE_REASON_UNKNOWN';
nm_device_state_reason[1] = 'NM_DEVICE_STATE_REASON_NONE';
nm_device_state_reason[2] = 'NM_DEVICE_STATE_REASON_NOW_MANAGED';
nm_device_state_reason[3] = 'NM_DEVICE_STATE_REASON_NOW_UNMANAGED';
nm_device_state_reason[4] = 'NM_DEVICE_STATE_REASON_CONFIG_FAILED';
nm_device_state_reason[5] = 'NM_DEVICE_STATE_REASON_CONFIG_UNAVAILABLE';
nm_device_state_reason[6] = 'NM_DEVICE_STATE_REASON_CONFIG_EXPIRED';
nm_device_state_reason[7] = 'NM_DEVICE_STATE_REASON_NO_SECRETS';
nm_device_state_reason[8] = 'NM_DEVICE_STATE_REASON_SUPPLICANT_DISCONNECT';
nm_device_state_reason[9] = 'NM_DEVICE_STATE_REASON_SUPPLICANT_CONFIG_FAILED';
nm_device_state_reason[10] = 'NM_DEVICE_STATE_REASON_SUPPLICANT_FAILED';
nm_device_state_reason[11] = 'NM_DEVICE_STATE_REASON_SUPPLICANT_TIMEOUT';
nm_device_state_reason[15] = 'NM_DEVICE_STATE_REASON_DHCP_START_FAILED';
nm_device_state_reason[16] = 'NM_DEVICE_STATE_REASON_DHCP_ERROR';
nm_device_state_reason[17] = 'NM_DEVICE_STATE_REASON_DHCP_FAILED';
nm_device_state_reason[18] = 'NM_DEVICE_STATE_REASON_SHARED_START_FAILED';
nm_device_state_reason[19] = 'NM_DEVICE_STATE_REASON_SHARED_FAILED';
nm_device_state_reason[20] = 'NM_DEVICE_STATE_REASON_AUTOIP_START_FAILED';
nm_device_state_reason[21] = 'NM_DEVICE_STATE_REASON_AUTOIP_ERROR';
nm_device_state_reason[22] = 'NM_DEVICE_STATE_REASON_AUTOIP_FAILED';
nm_device_state_reason[35] = 'NM_DEVICE_STATE_REASON_FIRMWARE_MISSING';
nm_device_state_reason[36] = 'NM_DEVICE_STATE_REASON_REMOVED';
nm_device_state_reason[37] = 'NM_DEVICE_STATE_REASON_SLEEPING';
nm_device_state_reason[38] = 'NM_DEVICE_STATE_REASON_CONNECTION_REMOVED';
nm_device_state_reason[39] = 'NM_DEVICE_STATE_REASON_USER_REQUESTED';
nm_device_state_reason[40] = 'NM_DEVICE_STATE_REASON_CARRIER';
nm_device_state_reason[41] = 'NM_DEVICE_STATE_REASON_CONNECTION_ASSUMED';
nm_device_state_reason[42] = 'NM_DEVICE_STATE_REASON_SUPPLICANT_AVAILABLE';
nm_device_state_reason[43] = 'NM_DEVICE_STATE_REASON_MODEM_NOT_FOUND';
nm_device_state_reason[44] = 'NM_DEVICE_STATE_REASON_BT_FAILED';
nm_device_state_reason[49] = 'NM_DEVICE_STATE_REASON_INFINIBAND_MODE';
nm_device_state_reason[50] = 'NM_DEVICE_STATE_REASON_DEPENDENCY_FAILED';
nm_device_state_reason[51] = 'NM_DEVICE_STATE_REASON_BR2684_FAILED';
nm_device_state_reason[52] = 'NM_DEVICE_STATE_REASON_MODEM_MANAGER_UNAVAILABLE';
nm_device_state_reason[53] = 'NM_DEVICE_STATE_REASON_SSID_NOT_FOUND';
nm_device_state_reason[54] = 'NM_DEVICE_STATE_REASON_SECONDARY_CONNECTION_FAILED';
nm_device_state_reason[55] = 'NM_DEVICE_STATE_REASON_DCB_FCOE_FAILED';
nm_device_state_reason[56] = 'NM_DEVICE_STATE_REASON_TEAMD_CONTROL_FAILED';
nm_device_state_reason[57] = 'NM_DEVICE_STATE_REASON_MODEM_FAILED';
nm_device_state_reason[58] = 'NM_DEVICE_STATE_REASON_MODEM_AVAILABLE';

var nm2device_type = {
	'802-3-ethernet': 'wired',
	'802-11-wireless': 'wireless',
	'generic': 'generic',
};

var deviceState = {
	0: 'disconnected', // NM_DEVICE_STATE_UNKNOWN: The device is in an unknown state.
	10: 'disconnected', // NM_DEVICE_STATE_UNMANAGED: The device is recognized but not managed by NetworkManager.
	20: 'disconnected', // NM_DEVICE_STATE_UNAVAILABLE: The device cannot be used (carrier off, rfkill, etc).
	30: 'disconnected', // NM_DEVICE_STATE_DISCONNECTED: The device is not connected.
	40: 'connecting',   // NM_DEVICE_STATE_PREPARE: The device is preparing to connect.
	50: 'connecting',   // NM_DEVICE_STATE_CONFIG: The device is being configured.
	60: 'connecting',   // NM_DEVICE_STATE_NEED_AUTH: The device is awaiting secrets necessary to continue connection.
	70: 'connecting',   // NM_DEVICE_STATE_IP_CONFIG: The IP settings of the device are being requested and configured.
	80: 'connecting',   // NM_DEVICE_STATE_IP_CHECK: The device's IP connectivity ability is being determined.
	90: 'connecting',   // NM_DEVICE_STATE_SECONDARIES: The device is waiting for secondary connections to be activated.
	100: 'connected',    // NM_DEVICE_STATE_ACTIVATED: The device is active.
	110: 'disconnecting',// NM_DEVICE_STATE_DEACTIVATING: The device's network connection is being torn down.
	120: 'error',        // NM_DEVICE_STATE_FAILED: The device is in a failure state following an attempt to activate it.
};

function NetworkManagerImpl() {
	var _dbus = null;
	var ifaceStateChangedCBs = null;
	var self = this;
	var _primary_connection;

	//	Setup events
	EventEmitter.call(self);
	self.on.isSignal = true;
	self.on.unregisterMethod = self.removeListener;

	// Utility functions
	function getConnection() {
		return {
			name: '',
			id: '',
			type: '',
			device: '',
			ip: '',
			mac: '',
			gw: '',
			dns: '',
			state: '',
		};
	}

	function int2ip( nIp ) {
		var result = [];
		while ( nIp > 0 ) {
			result.push(nIp % 256);
			nIp = Math.floor(nIp / 256);
		}
		return result.join('.');
	}

	function capitalize( s ) {
		return s && s[0].toUpperCase() + s.slice(1);
	}

	// Device related functions
	function getDeviceMac( device, type ) {
		return _dbus.get( srvName, device, 'org.freedesktop.NetworkManager.Device.'+capitalize(nm2device_type[type]), 'HwAddress');
	}

	function getDeviceManaged( device ) {
		return _dbus.get( srvName, device, 'org.freedesktop.NetworkManager.Device', 'Managed');
	}

	function getDeviceInterface( device ) {
		return _dbus.get( srvName, device, 'org.freedesktop.NetworkManager.Device', 'Interface');
	}

	function getDeviceState( device ) {
		return _dbus.get( srvName, device, 'org.freedesktop.NetworkManager.Device', 'State')
		.then( function ( arg ) {
			return deviceState[arg];
		});
	}

	function getDeviceWirelessDetails( device ) {
		return _dbus.getAll( srvName, device, 'org.freedesktop.NetworkManager.Device.Wireless');
	}

	// IP4Config related functions
	function getIP4ConfigDetails( ip4 ) {
		var result = {};
		result.address = [];
		result.dns = [];
		if( ip4 === '/' ) {
			return new bPromise( function(resolve) {
				resolve(result);
			});
		}
		return _dbus.getAll( srvName, ip4, 'org.freedesktop.NetworkManager.IP4Config')
		.then( function( ip4Config ) {
			ip4Config.Addresses.forEach( function( ip4ConfigAddr ) {
				var ip = {
					'ip': int2ip( ip4ConfigAddr[0] ) + '/' + ip4ConfigAddr[1],
					'gw': int2ip( ip4ConfigAddr[2] )
				};
				result.address.push( ip );
			});
			ip4Config.Nameservers.forEach( function( dns ) {
				result.dns.push( int2ip(dns) );
			});
			return result;
		});
	}

	// Connection related functions
	function getConnectionDetails( conn ) {
		return _dbus.getAll( srvName, conn, 'org.freedesktop.NetworkManager.Connection.Active')
		.then( function ( connection ) {
			var path = connection.Devices[0];
			var result = getConnection();
			result.id = conn;
			result.type = nm2device_type[connection.Type];
			var proms = [];
			proms.push(getDeviceMac( path, connection.Type)
			.then( function ( mac ) {
					result.mac = mac;
			}));
			proms.push(getDeviceInterface( path )
			.then( function ( iface ) {
					result.device = iface;
			}));
			proms.push(getDeviceState( path )
			.then( function ( state ) {
					result.state = state;
			}));
			proms.push(getIP4ConfigDetails( connection.Ip4Config )
			.then( function ( ip4Config ) {
					if( ip4Config.address.length > 0 ) {
						result.ip = ip4Config.address[0].ip;
						result.gw = ip4Config.address[0].gw;
						result.dns = ip4Config.dns;
					}
			}));
			if( connection.SpecificObject.length > 1 ) {
				proms.push(getNetworkManagerAccessPointDetails( path, connection.SpecificObject )
				.then( function ( ap_detail ) {
					result.name = ap_detail.name;
				}));
			}
			return bPromise.all(proms)
			.then( function() {
				return result;
			});
		});
	}

	// Active Connection related functions
	function getActiveConnectionDevice( conn ) {
		return _dbus.get( srvName, conn, 'org.freedesktop.NetworkManager.Connection.Active', 'Devices')
		.then( function ( devices ) {
			assert( devices.length > 0);
			return devices[0];
		});
	}

	// NetworkManager related functions
	function getNetworkManagerIsConnected() {
		return _dbus.get( srvName, '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'State' )
		.then( function ( arg ) {
			return arg === NM_STATE_CONNECTED_GLOBAL;
		});
	}

	function getNetworkManagerPrimaryConnection() {
		return _dbus.get( srvName, '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'PrimaryConnection' )
		.then( function( active_connection ) {
			if( active_connection.length > 1 ) {
				return getConnectionDetails( active_connection )
				.then( function( connection_detail ) {
					return connection_detail;
				});
			} else {
				return undefined;
			}
		});
	}

	function getNetworkManagerActiveConnections() {
		return _dbus.get( srvName, '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'ActiveConnections' )
		.map(getConnectionDetails);
	}

	function getNetworkManagerDevices() {
		return _dbus.get( srvName, '/org/freedesktop/NetworkManager', 'org.freedesktop.NetworkManager', 'Devices');
	}

	function getNetworkManagerWirelessDevices() {
		var result = [];
		return getNetworkManagerDevices()
		.map( function( dev ) {
			return getDeviceManaged( dev )
			.then( function( isManaged ) {
				if( isManaged ) {
					return getDeviceWirelessDetails( dev )
					.then( function( details ) {
						log.verbose( "NetworkManager", "Found device %s with wifi support", dev );
						result.push( { device: dev, details: details } );
					}, function() {
						log.verbose( "NetworkManager", "Device %s doesn't support wifi", dev );
					});
				}
			});
		})
		.then( function() {
			return result.length ? [ result[0] ] : result;
		});
	}

	function getNetworkManagerAccessPointDetails( device, wireless_access_point ) {
		return _dbus.getAll( srvName, wireless_access_point, 'org.freedesktop.NetworkManager.AccessPoint')
		.then( function ( accessPoint ) {
			var buff = new Buffer( accessPoint.Ssid );  //TODO: check this
			var result = {
				'name': buff.toString(),
				'signal': accessPoint.Strength,
				'isSecured': accessPoint.Flags === NM_802_11_AP_FLAGS_PRIVACY,
				'device': device,
				'security_flags': accessPoint.RsnFlags | accessPoint.WpaFlags,
				'wireless_access_point': wireless_access_point,
			};
			return result;
		});
	}

	// Settings related functions
	function getSettingsConnections() {
		return _dbus.invoke(
			srvName,
			'/org/freedesktop/NetworkManager/Settings',
			'org.freedesktop.NetworkManager.Settings',
			'ListConnections'
		)
		.map(getSettingsConnectionDetails);
	}

	function getSettingsConnectionDetails( conn ) {
		return _dbus.invoke(
			srvName,
			conn,
			'org.freedesktop.NetworkManager.Settings.Connection',
			'GetSettings'
		)
		.then( function ( arg ) {
			var result = {};
			result.name = arg.connection.id;
			result.id = arg.connection.uuid;
			result.type = arg.connection.type;
			result.path = conn;
			result.ssid = '';
			if ( '802-11-wireless' in arg ) {
				var buff = new Buffer( arg['802-11-wireless'].ssid );
				result.ssid = buff.toString();
			}
			return result;
		});
	}

	function deleteSettingsConnections( connections ) {
		return bPromise
		.map( connections, function( conn ) {
			return _dbus.invoke(
				srvName,
				conn.path,
				'org.freedesktop.NetworkManager.Settings.Connection',
				'Delete'
			);
		});
	}

	// Generic methods
	function getWifiFullList() {
		var result = [];
		return getNetworkManagerWirelessDevices()
		.map( function( wifi_device ) {
			return bPromise
			.map( wifi_device.details.AccessPoints, function( wireless_access_point ) {
				return getNetworkManagerAccessPointDetails( wifi_device, wireless_access_point )
				.then( function( ap_detail ) {
					result.push( ap_detail );
				});
			});
		})
		.then( function() {
			var p = result.sort( function( a, b ) {
				return b.signal - a.signal;
			});
			return p;
		});
	}

	function getWifiAP( ssid ) {
		return getWifiFullList()
		.then ( function( wifiList ) {
			var access_point = wifiList.filter(function(wifiConn) {
				return wifiConn.name === ssid;
			});
			if (!access_point.length) {
				log.error( 'NetworkManager', 'Couldn\'t find ssid %s', ssid );
				return bPromise.reject( new Error( 'error: '+ ssid + ' not found' ) );
			}
			log.verbose( 'NetworkManager', 'Found ssid %s', ssid );
			return access_point[0];
		});
	}

	function deleteConnections( name ) {
		return getSettingsConnections()
		.then( function( settings ) {
			var valid_settings = settings.filter( function( setting ) {
				return setting.name === name;
			});
			if( valid_settings.length ) {
				log.verbose( "NetworkManager", "Deleting setting %s", name );
				return deleteSettingsConnections( valid_settings );
			}
		});
	}

	function getConnectionParams(connection_name, access_point, opts) {
		assert(opts.autoconnect !== undefined);
		var connection_params = [['connection',[
			['id', ['s',connection_name]],
			['autoconnect', ['b',opts.autoconnect]]],
		]];

		if (access_point.isSecured) {
			assert(opts.pass !== undefined);
			connection_params.push(['802-11-wireless', [
				['security', ['s','802-11-wireless-security']]
			]]);
			if ( access_point.security_flags ) {
				connection_params.push(['802-11-wireless-security', [
					['psk', ['s',opts.pass]]
				]]);
			} else {
				connection_params.push(['802-11-wireless-security', [
					['wep-key-type', ['u',1]], // TYPE_KEY
					['wep-key0', ['s',opts.pass]]
				]]);
			}
		}
		return connection_params;
	}

	function createConnection( connection_name, access_point, opts ) {
		log.verbose( "NetworkManager", "Create Connection with ssid: %s", connection_name );
		access_point.security_flags = access_point.security_flags & NM_802_11_AP_SEC_KEY_MGMT_PSK;
		var connection_params = getConnectionParams(connection_name, access_point, opts);

		return getDeviceInterface( access_point.device.device )
			.then( function ( di ) {
				return _dbus.invoke(
					srvName,
					'/org/freedesktop/NetworkManager',
					'org.freedesktop.NetworkManager',
					'AddAndActivateConnection',
					'a{sa{sv}}oo',
					connection_params,
					access_point.device.device,
					access_point.wireless_access_point
				)
				.catch(function (error) {
					log.error( "NetworkManager", "Error creating connection:", error);
					error.device = di;
					throw error;
				});
			});
	}

	function getHiddenConnectionParams(connection_name, opts) {
		assert(opts.ssid !== undefined);
		assert(opts.pass !== undefined);
		assert(opts.autoconnect !== undefined);
		assert(opts.mode === 'wpa');
		var connection_params = [
			['connection',[
				['id', ['s',connection_name]],
				['autoconnect', ['b',opts.autoconnect]],
				['type', ['s','802-11-wireless']],
				['uuid', ['s','18de14ac-1a6f-11e5-b60b-1697f925ec7b']] // TODO generate new uuid on every call
			]],
			['ipv4', [['method', ['s','auto']]]],
			['ipv6', [['method', ['s','auto']]]],
			['802-11-wireless', [
				['mode', ['s','infrastructure']],
				['security', ['s','802-11-wireless-security']],
				['ssid', ['ay', [ opts.ssid.split('').map(function(c) { return c.charCodeAt(); }) ] ]]
			]],
			['802-11-wireless-security', [
				['key-mgmt', ['s','wpa-psk']],
				['psk', ['s',opts.pass]]
			]]
		];

		return connection_params;
	}

	function createHiddenConnection( connection_name, device, opts ) {
		log.verbose( "NetworkManager", "Create Connection with ssid: %s", connection_name );
		var connection_params = getHiddenConnectionParams(connection_name, opts);

		return getDeviceInterface( device )
			.then( function ( di ) {
				return _dbus.invoke(
					srvName,
					'/org/freedesktop/NetworkManager',
					'org.freedesktop.NetworkManager',
					'AddAndActivateConnection',
					'a{sa{sv}}oo',
					connection_params,
					device,
					'/'
				)
				.catch(function (error) {
					log.error( "NetworkManager", "Error creating connection:", error);
					error.device = di;
					throw error;
				});
			});
	}

	function connectHiddenWifi( opts ) {
		return deleteConnections( settingName )
		.then( function() {
			return getNetworkManagerWirelessDevices()
			.then( function( wirelessDevices ) {
				assert( wirelessDevices.length );
				return createHiddenConnection( settingName, wirelessDevices[0].device, opts );
			});
		});
	}

	function connectVisibleWifi( opts ) {
		return getWifiAP( opts.ssid )
		.then( function( ap ) {
			return deleteConnections( settingName )
			.then( function() {
				return createConnection( settingName, ap, opts );
			});
		});
	}

	function onPropertiesChanged( args ) {
		args = _dbus.parse( args );
		if( 'PrimaryConnection' in args ) {
			var primary_connection = args.PrimaryConnection;
			if(primary_connection !== '/' ) {
				getConnectionDetails( primary_connection )
				.then( function( primary_connection_detail ) {
					_primary_connection = primary_connection_detail;
					self.emit( 'active', primary_connection_detail.device );
				});
			} else {
				_primary_connection = undefined;
				self.emit( 'active', undefined );
			}
		}
	}

	function onNetworkStateChanged( args ) {
		if( (args === NM_STATE_CONNECTED_GLOBAL) || (args === NM_STATE_DISCONNECTED) || (args === NM_STATE_CONNECTED_LOCAL) ) {
			getNetworkManagerIsConnected()
			.then( function( isConnected ) {
				self.emit( 'state', isConnected );
			});
		}
	}

	function getNetworkIface( cb ) {
		assert(_dbus);
		_dbus.getInterface(
			srvName,
			'/org/freedesktop/NetworkManager',
			'org.freedesktop.NetworkManager',
			cb
		);
	}

	function onNewConnection( args ) {
		getSettingsConnectionDetails( args )
		.then( function( connection_detail ) {
			var event = {
				name: connection_detail.path,
				type: 'added',
				state: 'disconnected',
			};
			self.emit( 'connection', event );
		});
	}

	function getSettingsIface( cb ) {
		assert(_dbus);
		_dbus.getInterface(
			srvName,
			'/org/freedesktop/NetworkManager/Settings',
			'org.freedesktop.NetworkManager.Settings',
			cb
		);
	}

	function onIfaceStateChanged( device_interface ) {
		return function( args ) {
			var event = {
				name: device_interface,
				type: 'state',
				state: deviceState[args],
			};
			self.emit( 'connection', event );
		};
	}

	function getDeviceIface( device, cb ) {
		assert(_dbus);
		_dbus.getInterface(
			srvName,
			device,
			'org.freedesktop.NetworkManager.Device',
			cb
		);
	}

	// Public API implementation

	//	Returns if is connected
	self.isConnected = function( cb ) {
		getNetworkManagerIsConnected()
		.then( function ( arg ) {
			cb( undefined, arg );
		})
		.catch( cb );
	};
	self.isConnected.isAsync = true;

	//	Returns the id of the primary device (former getPrimaryDevice)
	self.activeConnection = function( cb ) {
		getNetworkManagerPrimaryConnection()
		.then( function ( arg ) {
			cb( undefined, arg );
		})
		.catch( cb );
	};
	self.activeConnection.isAsync = true;

	//	Returns the connection attached to a given device
	self.getConnection = function( device, cb ) {
		var device_connection;
		getNetworkManagerActiveConnections()
		.map( function( active_connection ) {
			return getActiveConnectionDevice( active_connection.id )
			.then( getDeviceInterface )
			.then( function ( device_interface ) {
				if( device_interface === device ) {
					device_connection = active_connection;
				}
			});
		})
		.then( function () {
			cb(undefined, device_connection);
		})
		.catch( cb );
	};
	self.getConnection.isAsync = true;

	//	Returns a array of available connections
	self.getConnections = function( cb ) {
		getNetworkManagerActiveConnections()
		.then( function ( arg ) {
			cb( undefined, arg );
		})
		.catch( cb );
	};
	self.getConnections.isAsync = true;

	//	Returns a array of available wifi networks
	self.getWifiList = function( cb ) {
		return getWifiFullList()
		.map( function( wifi_conn ) {
			return {
				name: wifi_conn.name,
				isSecured: wifi_conn.isSecured,
				signal: wifi_conn.signal,
			};
		})
		.then( function( result ) {
			cb( undefined, result );
		})
		.catch( cb );
	};
	self.getWifiList.isAsync = true;

	//	Connect
	self.connectWifi = function( opts, cb ) {
		var result;

		// If mode present in opts -> we are dealing with a hidden AP
		if( 'mode' in opts ) {
			result = connectHiddenWifi(opts);
		} else {
			result = connectVisibleWifi(opts);
		}

		result.then( function() {
			cb();
		})
		.catch( function( error ){
			var event = {
				name: error.device,
				type: 'state',
				state: 'error',
			};
			log.error( 'NetworkManager', 'Couldn\'t connect using device: %s "%s"', error.device, error.toString() );
			self.emit( 'connection', event );
			cb( new Error(error.toString()) );
		});
	};
	self.connectWifi.isAsync = true;

	//  Disconnect Device
	self.disconnectWifi = function( ssid, callback ) {
		var connected_wifi;
		getNetworkManagerActiveConnections()
		.map( function( active_connection ) {
			if( active_connection.name === ssid ) {
				connected_wifi = active_connection;
			}
		})
		.then( function () {
			if(connected_wifi !== undefined ) {
				getActiveConnectionDevice( connected_wifi.id )
				.then( function ( connected_device ) {
					return _dbus.invoke(
						srvName,
						connected_device,
						'org.freedesktop.NetworkManager.Device',
						'Disconnect'
					)
					.then( function() {
						callback();
					});
				});
			} else {
				log.error( 'NetworkManager', 'Couldn\'t find ssid %s', ssid );
				return bPromise.reject( new Error( 'error: '+ ssid + ' not found' ) );
			}
		})
		.catch( function( error ) {
			callback( new Error(error.toString()) );
		});
	};
	self.disconnectWifi.isAsync = true;

	self.dumpState = function() {
		return _primary_connection;
	};

	self.init = function( api, cb ) {
		assert( cb );
		ifaceStateChangedCBs = [];

		//      Setup dbus
		_dbus = new DBus();
		_dbus.init(function(err) {
			if (err) {
				cb( tvdutil.warnErr( 'network', 'Error trying to setup dbus: err=%s', err.message ) );
				return;
			}
			getNetworkManagerPrimaryConnection()
			.then(function( primary_connection_detail ) {
				_primary_connection = primary_connection_detail;
			});

			//      Setup notifications
			getNetworkIface(function( err, netIface ) {
				if (err) {
					cb( tvdutil.warnErr( 'network', 'Error trying to setup network manager events: err=%s', err.message ) );
					return;
				}

				netIface.addListener( 'StateChanged', onNetworkStateChanged );
				netIface.addListener( 'PropertiesChanged', onPropertiesChanged );

				getSettingsIface(function( err, settingsIface ) {
					if (err) {
						cb( tvdutil.warnErr( 'network', 'Error trying to setup network manager settings events: err=%s', err.message ) );
						return;
					}

					settingsIface.addListener( 'NewConnection', onNewConnection );

					getNetworkManagerDevices()
					.map( function(device) {
						getDeviceInterface( device )
						.then( function ( device_interface ) {
							getDeviceIface(device, function( err, deviceIface ) {
								if (err) {
									throw err;
								}
								var ifaceStateChangedCB = onIfaceStateChanged( device_interface );
								deviceIface.addListener( 'StateChanged', ifaceStateChangedCB);
								ifaceStateChangedCBs.push({ device:device, cb: ifaceStateChangedCB });
							});
						});
					})
					.then( function () {
						//      Ok! ... return api
						cb( undefined, new api(self) );
					}).catch( function( err ) {
						cb( tvdutil.warnErr( 'network', 'Error trying to setup network manager device events: err=%s', err.message ) );
					});
				});
			});
		});
	};

	self.fin = function(cb) {
		assert(cb);
		assert(ifaceStateChangedCBs);
		assert(_dbus);
		getNetworkIface(function( err, netIface ) {
			if (!err) {
				netIface.removeListener( 'StateChanged', onNetworkStateChanged );
				netIface.removeListener( 'PropertiesChanged', onPropertiesChanged );
			}

			getSettingsIface(function( err, settingsIface ) {
				if (!err) {
					settingsIface.removeListener( 'NewConnection', onNewConnection );
				}
				bPromise.map(ifaceStateChangedCBs, function( ifaceStateChangedCB ) {
					getDeviceIface(ifaceStateChangedCB.device, function( err, deviceIface ) {
						if (!err) {
							deviceIface.removeListener( 'StateChanged', ifaceStateChangedCB.cb );
						}
					});
				})
				.then( function () {
					_dbus.fin(function() {
						cb();
					});
				});
			});
		});
	};

	return self;
}
util.inherits(NetworkManagerImpl, EventEmitter);

module.exports = NetworkManagerImpl;
