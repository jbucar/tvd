'use strict';

var NetworkModule = (function() {
	var mixins = [APIBindingSupport, DISupport, enyo.ComputedSupport, enyo.BindingSupport, enyo.EventEmitter];

	// Active Wireless connection
	enyo.kind({
		name: 'shell.network.WirelessConnection',
		mixins: mixins,
		// Attrs
		connName: undefined,
		connections: undefined,
		computed: [
			{ method: 'isConnected', path: ['connName'] },
		],
		isConnected: function() {
			return this.get('connName') !== undefined;
		},
		create: enyo.inherit(function(sup) {
			return function() {
				var sync = (function (conn) {
					this.set('isConnecting', conn.get('state') === 'connecting');

					if (conn.get('state') === 'connected') {
						this.set('connName', conn.get('name'));
					}
					else if (conn.get('state') === 'disconnected') {
						this.set('connName', undefined);
					}
				}).bind(this);

				this.connections.on('change', function(oSender, oEventName, oEvt) {
					var conn = oEvt.model;
					if (conn.get('type') === 'wireless') {
						sync(conn);
					}
				});

				this.connections.on('add', function(oSender, oEventName, oEvt) {
					var conn = oEvt.models.find(function(c) {
						return c.get('type') === 'wireless';
					});

					if (conn) {
						sync(conn);
					}
				});

				sup.apply(this, arguments);
			}
		}),
		disconnect: function() {
			network.disconnectWifi(this.get('connName'));
		}
	});

	enyo.kind({
		name: "shell.network.WirelessAP",
		kind: 'enyo.Model',
		primaryKey: 'name',
		attributes: {
			state: 'disconnected',
			name: undefined,
			isSecured: undefined,
			signal: undefined,
		},
		computed: [
			{ method: 'isConnected', path: ['state'] },
			{ method: 'isConnecting', path: ['state'] }
		],
		isConnected: function() {
			return this.get("state") === 'connected';
		},
		isConnecting: function() {
			return this.get("state") === 'connecting';
		}
	});

	enyo.kind({
		name: 'shell.network.WirelessAPs',
		kind: 'enyo.Collection',
		options: {
			merge: true,
			// Sort by signal
			sort: function(a, b) {
				return !a.get('signal')? -1 : b.get('signal') - a.get('signal');
			}
		},
		model: 'shell.network.WirelessAP',
		sync: function(silence) {
			this.emit('fetch', {status: 'begin'}, this);
			if (!silence) {
				notification.emit('InternetConnection', {'name': 'fetch', status: 'begin'});
			}
			this.empty();
			network.getWifiList(function(err, list) {
				if (!err) {
					this.add(list.filter(function(conn) { return conn.name !== '' }));
					if (!silence) {
						notification.emit('InternetConnection', {'name': 'fetch', status: 'end', result: list.length});
					}
					this.emit('fetch', {status: 'end', networks: list.length}, this);
				}
				else {
					notification.emit('InternetConnection', {'name': 'fetch', status: 'error', result: err.message});
					this.emit('fetch', {status: 'error', msg: err.message}, this);
				}
			}.bind(this));
		}
	});

	enyo.kind({
		name: 'shell.network.Connection',
		kind: 'enyo.Model',
		mixins: mixins,
		attributes: {
			state: undefined,
		},
		dependencies: {
			network: network
		},
		stateChanged: function(was, is) {
			if (is === 'connecting' || is === 'connected' || (is === 'disconnected' && this.get('name')) || is === 'error') {
				notification.emit('InternetConnection', {name: is});
			}
			if (is === 'connecting') {
				this.getAPI('network').getConnection(this.get('device'), util.catchError(this.set.bind(this)));
			}
			else if (is === 'disconnected') {
				this.set('name', undefined, {silence: true});
			}
		}
	});

	enyo.kind({
		name: 'shell.network.Connections',
		kind: 'enyo.Collection',
		mixins: [DISupport],
		model: 'shell.network.Connection',
		options: {
			purge: true
		},
		dependencies: {
			network: network,
			forwarder : util.proxy(network).forward('connection') // Use for on/off
		},
		create: enyo.inherit(function(sup) {
			return function() {
				var find = function(name) {
					return this.find(function(m) {
						return m.get('device') === name;
					});
				}.bind(this);

				this.listener = function(evt) {
					var model = find(evt.name);
					if (evt.type === 'remove') {
						if (model) {
							this.remove(model);
						}
					}
					else if (evt.type === 'state') {
						var model = find(evt.name);
						if (evt.state === 'error') {
							this.emit('connFail', {connection: model || {name: evt.name} });
						}

						if (model) {
							model.set('state', evt.state);
						}
						else {
							this.add({device: evt.name});
						}
					}
				}.bind(this);
				sup.apply(this, arguments);
			}
		}),
		listen: function(on) {
			if (on) {
				this.getAPI('forwarder').on('connection', this.listener);
			}
			else {
				this.getAPI('forwarder').off('connection', this.listener);
			}
		},
		sync: function() {
			this.getAPI('network').getConnections(util.catchError(function(conns) {
				if (conns.length > 0) {
					this.add(conns);
				}
				else {
					this.empty();
				}
			}.bind(this)));
		}
	});

	// Active Global connection
	enyo.kind({
		name: 'shell.network.ActiveConnection',
		kind: 'enyo.Model',
		mixins: mixins,
		// Attrs
		attributes: {
			connName: undefined,
			connID: undefined,
			type: undefined,
			device: undefined,
			ip: undefined,
			mac: undefined,
			gw: undefined,
			dns: undefined,
			state: undefined,
		},
		dependencies: {
			network: network,
			forwarder : util.proxy(network).forward('active') // Use for on/off
		},
		apiBindings: [
			{ api: 'forwarder', from: 'active', to: 'device' }
		],
		deviceChanged: function(was, is) {
			this.getAPI('network').getConnection(is, util.catchError(this._setData.bind(this)));
		},
		sync: function() {
			this.getAPI('network').activeConnection(util.catchError(this._setData.bind(this)));
		},
		_setData: function( conn ) {
			if (conn) {
				this.set(util.renameKeys(conn, {name: 'connName', id: 'connID'}));
			}
			else {
				this.set({
					connName: undefined,
					connID: undefined,
					type: undefined,
					device: undefined,
					ip: undefined,
					mac: undefined,
					gw: undefined,
					dns: undefined,
					state: undefined,
				});
			}
		}
	});

	// Global connectivity state
	enyo.kind({
		name: 'shell.network.Connectivity',
		mixins: mixins,
		// Attrs
		state: undefined,
		dependencies: {
			network: network,
			notification: NotificationAPI,
		},
		apiBindings: [
			{ api: 'network', attribute: 'state', sync: 'isConnected', autoConnect: true }
		],
		stateChanged: function(was, is) {
			if (!is) {
				this.getAPI('notification').emit('InternetConnection', { name: 'noConnectivity' });
			}
		}
	});

	function sync(connections, aps) {
		function syncConn(conn) {
			// Skip processing unidentified connections
			if (!conn.get('name')) {
				return;
			}

			var ap = aps.find(function(c) {
				return (c.get('name') === conn.get('name')) && (conn.get('type') === 'wireless');
			});

			if (ap) {
				ap.set('state', conn.get('state'));
			}
			else if (conn.get('type') === 'wireless') {
				aps.add({
					name: conn.get('name'),
					state: conn.get('state'),
					signal: conn.get('signal') || 100
				});
			}
		}

		connections.on('change', function(oSender, oEventName, oEvt) {
			syncConn(oEvt.model);
		});

		connections.on('add', function(oSender, oEventName, oEvt) {
			oEvt.models.forEach(syncConn);
		});

		aps.on('fetch', function(oSender, oEventName, oEvt) {
			if (oEvt.status === 'end' && oEvt.networks > 0) {
				connections.models.forEach(syncConn);
			}
		});
	}

	return {
		init: function() {
			var Connections = new shell.network.Connections();
			var Connectivity = new shell.network.Connectivity();
			var ActiveConnection = new shell.network.ActiveConnection();
			var WirelessAPs = new shell.network.WirelessAPs();
			var WirelessConnection = new shell.network.WirelessConnection({connections: Connections});
			sync(Connections, WirelessAPs);

			return {
				connections: Connections,
				connectivity: Connectivity,
				activeConnection: ActiveConnection,
				wirelessAPs: WirelessAPs,
				wirelessConnection: WirelessConnection,
			}
		}
	}
})();
