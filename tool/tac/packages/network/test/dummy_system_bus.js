var EventEmitter = require('events').EventEmitter;
var tvdutil = require('tvdutil');
var _ = require("lodash");

var dummies_system_buses = {
	generic_map: {
		// interface
		'org.freedesktop.DBus.Properties': {
			// member
			'Get': {
				// interface_name
				'org.freedesktop.NetworkManager': {
					// property_name
					'PrimaryConnection': function() { return [ [ { type: 'o', child: [] } ], [ '/' ] ]; },
					'Devices': function() { return [ [ { "type":"a","child":[ { "type":"o","child": [] } ] } ],
						[ [ '/org/freedesktop/NetworkManager/Devices/0',
							'/org/freedesktop/NetworkManager/Devices/1',
							'/org/freedesktop/NetworkManager/Devices/2' ] ] ];
					},
					'State': function() { return [ [ { "type":"u","child":[] } ], [ 20 ] ]; },
					'ActiveConnections': function() { return [[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/ActiveConnection/42"]]]; }
				},
				'org.freedesktop.NetworkManager.Device': {
					'Interface': function(path) {
						var index = parseInt(path.split('/')[5]);
						var iface = 'lo';
						if( index > 0 ) {
							iface = 'wlan'+(index-1);
						}
						return [ [ { type: 's', child: [] } ], [ iface ] ];
					},
					'Managed': function(path) {
						var index = parseInt(path.split('/')[5]);
						var managed = index > 0;
						return [ [ { type: 'b', child: [] } ], [ managed ] ];
					},
					'State': function(/*path*/) {
						return [[{"type":"u","child":[]}],[100]];
					}
				},
				'org.freedesktop.NetworkManager.Device.Wireless': {
					'HwAddress': function(/*path*/) {
						return [[{"type":"s","child":[]}],["02:00:00:00:01:00"]];
					}
				},
				'org.freedesktop.NetworkManager.Connection.Active': {
					'Devices': function(/*path*/) {
						return [[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/Devices/1"]]];
					}
				}
			},
			'GetAll': {
				// interface_name
				'org.freedesktop.NetworkManager.Device.Wireless': function(path) {
					if( path !== '/org/freedesktop/NetworkManager/Devices/1' ) {
						throw new Error('Method "GetAll" with signature "s" on interface "org.freedesktop.DBus.Properties" doesn\'t exist\n');
					}
					return [["HwAddress",[[{"type":"s","child":[]}],["02:00:00:00:01:00"]]],["PermHwAddress",[[{"type":"s","child":[]}],["02:00:00:00:01:00"]]],["Mode",[[{"type":"u","child":[]}],[2]]],["Bitrate",[[{"type":"u","child":[]}],[0]]],["AccessPoints",[[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/AccessPoint/5", "/org/freedesktop/NetworkManager/AccessPoint/4","/org/freedesktop/NetworkManager/AccessPoint/3","/org/freedesktop/NetworkManager/AccessPoint/2","/org/freedesktop/NetworkManager/AccessPoint/0"]]]],["ActiveAccessPoint",[[{"type":"o","child":[]}],["/"]]],["WirelessCapabilities",[[{"type":"u","child":[]}],[2047]]]];
				},
				'org.freedesktop.NetworkManager.AccessPoint': function(path) {
					var map = {
						'/org/freedesktop/NetworkManager/AccessPoint/5': [["Flags",[[{"type":"u","child":[]}],[1]]],["WpaFlags",[[{"type":"u","child":[]}],[0]]],["RsnFlags",[[{"type":"u","child":[]}],[392]]],["Ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,87,80,65,45,72,73,68,68,69,78]}]]],["Frequency",[[{"type":"u","child":[]}],[2412]]],["HwAddress",[[{"type":"s","child":[]}],["02:00:00:00:05:00"]]],["Mode",[[{"type":"u","child":[]}],[2]]],["MaxBitrate",[[{"type":"u","child":[]}],[54000]]],["Strength",[[{"type":"y","child":[]}],[84]]],["LastSeen",[[{"type":"i","child":[]}],[24150]]]],
						'/org/freedesktop/NetworkManager/AccessPoint/4': [["Flags",[[{"type":"u","child":[]}],[1]]],["WpaFlags",[[{"type":"u","child":[]}],[0]]],["RsnFlags",[[{"type":"u","child":[]}],[392]]],["Ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[]}]]],["Frequency",[[{"type":"u","child":[]}],[2412]]],["HwAddress",[[{"type":"s","child":[]}],["02:00:00:00:05:00"]]],["Mode",[[{"type":"u","child":[]}],[2]]],["MaxBitrate",[[{"type":"u","child":[]}],[54000]]],["Strength",[[{"type":"y","child":[]}],[84]]],["LastSeen",[[{"type":"i","child":[]}],[24150]]]],
						'/org/freedesktop/NetworkManager/AccessPoint/3': [["Flags",[[{"type":"u","child":[]}],[0]]],["WpaFlags",[[{"type":"u","child":[]}],[0]]],["RsnFlags",[[{"type":"u","child":[]}],[0]]],["Ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,79,80,78]}]]],["Frequency",[[{"type":"u","child":[]}],[2412]]],["HwAddress",[[{"type":"s","child":[]}],["02:00:00:00:04:00"]]],["Mode",[[{"type":"u","child":[]}],[2]]],["MaxBitrate",[[{"type":"u","child":[]}],[54000]]],["Strength",[[{"type":"y","child":[]}],[84]]],["LastSeen",[[{"type":"i","child":[]}],[24276]]]],
						'/org/freedesktop/NetworkManager/AccessPoint/2': [["Flags",[[{"type":"u","child":[]}],[1]]],["WpaFlags",[[{"type":"u","child":[]}],[0]]],["RsnFlags",[[{"type":"u","child":[]}],[0]]],["Ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,87,69,80]}]]],["Frequency",[[{"type":"u","child":[]}],[2412]]],["HwAddress",[[{"type":"s","child":[]}],["02:00:00:00:03:00"]]],["Mode",[[{"type":"u","child":[]}],[2]]],["MaxBitrate",[[{"type":"u","child":[]}],[54000]]],["Strength",[[{"type":"y","child":[]}],[84]]],["LastSeen",[[{"type":"i","child":[]}],[24276]]]],
						'/org/freedesktop/NetworkManager/AccessPoint/0': [["Flags",[[{"type":"u","child":[]}],[1]]],["WpaFlags",[[{"type":"u","child":[]}],[0]]],["RsnFlags",[[{"type":"u","child":[]}],[392]]],["Ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,87,80,65]}]]],["Frequency",[[{"type":"u","child":[]}],[2412]]],["HwAddress",[[{"type":"s","child":[]}],["02:00:00:00:02:00"]]],["Mode",[[{"type":"u","child":[]}],[2]]],["MaxBitrate",[[{"type":"u","child":[]}],[54000]]],["Strength",[[{"type":"y","child":[]}],[84]]],["LastSeen",[[{"type":"i","child":[]}],[24276]]]]

					};
					return map[path];
				},
				'org.freedesktop.NetworkManager.Connection.Active': function(/*path*/) {
					return [["Connection",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/Settings/140"]]],["SpecificObject",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/AccessPoint/3"]]],["Id",[[{"type":"s","child":[]}],["TAC_Wireless"]]],["Uuid",[[{"type":"s","child":[]}],["c3a19068-7e74-46fb-9f22-44f74dd8b1b1"]]],["Type",[[{"type":"s","child":[]}],["802-11-wireless"]]],["Devices",[[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/Devices/1"]]]],["State",[[{"type":"u","child":[]}],[2]]],["Default",[[{"type":"b","child":[]}],[true]]],["Ip4Config",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/IP4Config/50"]]],["Dhcp4Config",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/DHCP4Config/30"]]],["Default6",[[{"type":"b","child":[]}],[false]]],["Ip6Config",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/IP6Config/50"]]],["Dhcp6Config",[[{"type":"o","child":[]}],["/"]]],["Vpn",[[{"type":"b","child":[]}],[false]]],["Master",[[{"type":"o","child":[]}],["/"]]]];
				},
				'org.freedesktop.NetworkManager.IP4Config': function(/*path*/) {
					return [["Addresses",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[[3691292864,24,1678026944]]]]],["AddressData",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[[["prefix",[[{"type":"u","child":[]}],[24]]],["address",[[{"type":"s","child":[]}],["192.168.4.220"]]]]]]]],["Gateway",[[{"type":"s","child":[]}],["192.168.4.100"]]],["Routes",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["RouteData",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["Nameservers",[[{"type":"a","child":[{"type":"u","child":[]}]}],[[134744072]]]],["Domains",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["Searches",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["WinsServers",[[{"type":"a","child":[{"type":"u","child":[]}]}],[[]]]]];
				}
			}
		},
		'org.freedesktop.NetworkManager': {
			'AddAndActivateConnection': function() { return "/org/freedesktop/NetworkManager/Settings/140"; },
		},
		// interface
		'org.freedesktop.NetworkManager.Settings': {
			'ListConnections': function() {
				return ["/org/freedesktop/NetworkManager/Settings/0","/org/freedesktop/NetworkManager/Settings/1"];
			}
		},
		'org.freedesktop.NetworkManager.Settings.Connection': {
			'GetSettings': function(msg) {
				var map = {
					'/org/freedesktop/NetworkManager/Settings/0': [["connection",[["id",[[{"type":"s","child":[]}],["enp0s25"]]],["uuid",[[{"type":"s","child":[]}],["27503d54-79f2-4602-897c-5a2c857d2b76"]]],["permissions",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["timestamp",[[{"type":"t","child":[]}],[1434384537]]],["type",[[{"type":"s","child":[]}],["802-3-ethernet"]]],["secondaries",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["ipv4",[["addresses",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[[4060129472,24,16820416]]]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[["lan.tecnologia.lifia.info.unlp.edu.ar"]]]],["dns",[[{"type":"a","child":[{"type":"u","child":[]}]}],[[16820416]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["routes",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["manual"]]],["gateway",[[{"type":"s","child":[]}],["192.168.0.1"]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[[["prefix",[[{"type":"u","child":[]}],[24]]],["address",[[{"type":"s","child":[]}],["192.168.0.242"]]]]]]]]]],["802-3-ethernet",[["mac-address-blacklist",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["s390-options",[[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"s","child":[]}]}]}],[[]]]]]],["ipv6",[["routes",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"a","child":[{"type":"y","child":[]}]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]]],
					'/org/freedesktop/NetworkManager/Settings/1': [["802-11-wireless",[["mac-address-blacklist",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["security",[[{"type":"s","child":[]}],["802-11-wireless-security"]]],["ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,87,80,65]}]]],["mode",[[{"type":"s","child":[]}],["infrastructure"]]],["seen-bssids",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["connection",[["secondaries",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["id",[[{"type":"s","child":[]}],["TAC_Wireless"]]],["uuid",[[{"type":"s","child":[]}],["d836cc27-097f-4f55-b96d-c537555d9d1b"]]],["permissions",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["autoconnect",[[{"type":"b","child":[]}],[false]]],["type",[[{"type":"s","child":[]}],["802-11-wireless"]]],["timestamp",[[{"type":"t","child":[]}],[1446797885]]]]],["ipv4",[["routes",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"u","child":[]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]],["802-11-wireless-security",[["proto",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["auth-alg",[[{"type":"s","child":[]}],["open"]]],["pairwise",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["key-mgmt",[[{"type":"s","child":[]}],["wpa-psk"]]],["group",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["ipv6",[["routes",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"a","child":[{"type":"y","child":[]}]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]]],
// 						'/org/freedesktop/NetworkManager/Settings/140': [["802-11-wireless",[["mac-address-blacklist",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["security",[[{"type":"s","child":[]}],["802-11-wireless-security"]]],["ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,87,80,65]}]]],["mode",[[{"type":"s","child":[]}],["infrastructure"]]],["seen-bssids",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["connection",[["id",[[{"type":"s","child":[]}],["TAC_Wireless"]]],["uuid",[[{"type":"s","child":[]}],["4dd85d2f-b4f2-4a7c-aa67-075130141ffe"]]],["permissions",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["autoconnect",[[{"type":"b","child":[]}],[false]]],["type",[[{"type":"s","child":[]}],["802-11-wireless"]]],["secondaries",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["ipv4",[["routes",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"u","child":[]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]],["802-11-wireless-security",[["proto",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["auth-alg",[[{"type":"s","child":[]}],["open"]]],["pairwise",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["key-mgmt",[[{"type":"s","child":[]}],["wpa-psk"]]],["group",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["ipv6",[["routes",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"a","child":[{"type":"y","child":[]}]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]]]
					'/org/freedesktop/NetworkManager/Settings/140': [["802-11-wireless",[["mac-address-blacklist",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["ssid",[[{"type":"a","child":[{"type":"y","child":[]}]}],[{"type":"Buffer","data":[83,116,114,101,97,109,45,87,105,70,105,45,79,80,78]}]]],["mode",[[{"type":"s","child":[]}],["infrastructure"]]],["seen-bssids",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["connection",[["id",[[{"type":"s","child":[]}],["TAC_Wireless"]]],["uuid",[[{"type":"s","child":[]}],["6f2d630a-b77f-4bbb-b8aa-ffdd5dfa85eb"]]],["permissions",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["autoconnect",[[{"type":"b","child":[]}],[false]]],["type",[[{"type":"s","child":[]}],["802-11-wireless"]]],["secondaries",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]]]],["ipv4",[["routes",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"a","child":[{"type":"u","child":[]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"u","child":[]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]],["ipv6",[["routes",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]}]}]}],[[]]]],["route-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]],["addresses",[[{"type":"a","child":[{"type":"(","child":[{"type":"a","child":[{"type":"y","child":[]}]},{"type":"u","child":[]},{"type":"a","child":[{"type":"y","child":[]}]}]}]}],[[]]]],["dns",[[{"type":"a","child":[{"type":"a","child":[{"type":"y","child":[]}]}]}],[[]]]],["method",[[{"type":"s","child":[]}],["auto"]]],["dns-search",[[{"type":"a","child":[{"type":"s","child":[]}]}],[[]]]],["address-data",[[{"type":"a","child":[{"type":"a","child":[{"type":"{","child":[{"type":"s","child":[]},{"type":"v","child":[]}]}]}]}],[[]]]]]]]
				};
				return map[msg.path];
			},
			'Delete': function() {
				return undefined;
			}
		}
	},

	SystemBus: function ( failing_iface, map ) {
		var self = {};
		var interface_emitters = {};
		self.events = new EventEmitter();
		self.connection = {
			end: function() {}
		};
		tvdutil.forwardEventEmitter(self.connection, self.events);

		if( map === undefined ) {
			map = dummies_system_buses.generic_map;
		}
		
		self.getService = function() {
			return {
				getInterface: function(p,i,cb) {
					if( i === failing_iface ) {
						cb( new Error('pepe') );
						return;
					}
					if (! (i in interface_emitters)) {
						interface_emitters[i] = {};
					}
					if (! (p in interface_emitters[i]) ) {
						interface_emitters[i][p] = new EventEmitter();
					}
					cb( undefined, interface_emitters[i][p] );
				},
			};
		};

		self.service_emmit = function( p, i, event, params ) {
	// 		var args = Array.prototype.slice.call(arguments);
			if((i in interface_emitters) && (p in interface_emitters[i])) {
	// 			interface_emitters[i][p].emit(event, args.slice(3));
				interface_emitters[i][p].emit(event,params);
			}
		};

		self.service_delay_emmit = function( events, delay ) {
			var event = events.shift();
			if( event ) {
				self.service_emmit.apply( self, event );
				setTimeout( function() {
					self.service_delay_emmit( events, delay );
				} , delay);
			}
		};

		self.invoke = function(msg, callback) {
			var result;
			
			try {
				if( (msg.interface in map) && (msg.member in map[msg.interface])) {
					
					var func;
					if( msg.interface !== 'org.freedesktop.DBus.Properties' ) {
						func = map[msg.interface][msg.member];
						result = func(msg);
					} else {
						var body = msg.body;
						var interface_name = body[0];
						var property_name = body[1];
						var submap = map[msg.interface][msg.member];
						if( interface_name in submap ) {
							func = submap[interface_name];
							if( ( body.length > 1 ) && ( property_name in submap[interface_name] ) ) {
								func = func[property_name];
							}
							result = func(msg.path);
						}
					}
				}
				callback(undefined, result);
			} catch (err) {
				callback(err);
			}
		};

		return self;
	}
};

dummies_system_buses.connected_map = _.cloneDeep(dummies_system_buses.generic_map);
dummies_system_buses.connected_map['org.freedesktop.DBus.Properties'].Get['org.freedesktop.NetworkManager'].State = function() { 
	return [ [ { "type":"u","child":[] } ], [ 70 ] ]; 
};
dummies_system_buses.connected_map['org.freedesktop.DBus.Properties'].Get['org.freedesktop.NetworkManager'].PrimaryConnection = function() { 
	return [ [ { type: 'o', child: [] } ], [ '/org/freedesktop/NetworkManager/Settings/140' ] ]; 
};

dummies_system_buses.failed_connect = _.cloneDeep(dummies_system_buses.generic_map);
dummies_system_buses.failed_connect['org.freedesktop.NetworkManager'].AddAndActivateConnection = function () { 
	throw new Error('802-11-wireless.ssid: SSID length is out of range <1-32> bytes'); 
};

dummies_system_buses.no_ip_default = _.cloneDeep(dummies_system_buses.connected_map);
dummies_system_buses.no_ip_default['org.freedesktop.DBus.Properties'].GetAll['org.freedesktop.NetworkManager.Connection.Active'] = function(/*path*/) {
	return [["Connection",[[{"type":"o","child":[]}],["/org/freedesktop/NetworkManager/Settings/140"]]],["SpecificObject",[[{"type":"o","child":[]}],["/"]]],["Id",[[{"type":"s","child":[]}],["TAC_Wireless"]]],["Uuid",[[{"type":"s","child":[]}],["c3a19068-7e74-46fb-9f22-44f74dd8b1b1"]]],["Type",[[{"type":"s","child":[]}],["802-11-wireless"]]],["Devices",[[{"type":"a","child":[{"type":"o","child":[]}]}],[["/org/freedesktop/NetworkManager/Devices/1"]]]],["State",[[{"type":"u","child":[]}],[2]]],["Default",[[{"type":"b","child":[]}],[true]]],["Ip4Config",[[{"type":"o","child":[]}],["/"]]],["Dhcp4Config",[[{"type":"o","child":[]}],["/"]]],["Default6",[[{"type":"b","child":[]}],[false]]],["Ip6Config",[[{"type":"o","child":[]}],["/"]]],["Dhcp6Config",[[{"type":"o","child":[]}],["/"]]],["Vpn",[[{"type":"b","child":[]}],[false]]],["Master",[[{"type":"o","child":[]}],["/"]]]];
};

dummies_system_buses.missing_tac_wireless = _.cloneDeep(dummies_system_buses.generic_map);
dummies_system_buses.missing_tac_wireless['org.freedesktop.NetworkManager.Settings'].ListConnections = function() { 
	return ["/org/freedesktop/NetworkManager/Settings/0"]; 
};

module.exports = dummies_system_buses;