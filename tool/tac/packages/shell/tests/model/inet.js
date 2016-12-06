'use strict';

describe("InternetModels", function() {
	var info = {
		general: {
			hwaddr: '09:02:HH:FF:07'
		},
		ip4: {
			address: [{
				ip: '192.168.0.1',
				gw: '192.168.0.1',
			}],
			dns: ['dns.domain.com'],
			domain: ['domain.com'],
		}
	};

	describe("WifiConnection", function() {
		var wifi;
		var notification;
		var networkmgr;
		var notifyEmitSpy;
		var connectionEvtspy;
		var connectionErrEvtspy;

		var wifiConn = {
			type: 'wireless',
			ip: '127.0.0.1',
			ssid: 'someSSID'
		};

		function wifiConnect(success, ssid) {
			var stub = sinon.stub(networkmgr, 'connectWifi');
			if (success) {
				stub.yields(undefined, true);
			}
			else {
				stub.yields(new Error());
			}
			wifi.connect(ssid || 'someSSID', '123');
			if (success) {
				networkmgr.emit('changed', {type: 'connected', state: true, connection: wifiConn});
			}

			return stub;
		}

		function wifiDisconnect(success, ssid) {
			var stub = sinon.stub(networkmgr, 'disconnectWifi');
			if (success) {
				stub.yields(undefined, true);
			}
			else {
				stub.yields(new Error());
			}
			wifi.disconnect(ssid || 'someSSID');
			if (success) {
				networkmgr.emit('changed', {type: 'connection lost', state: false, connection: wifiConn});
			}

			return stub;
		}

		beforeEach(function () {
			notification = new EventEmitter();
			notifyEmitSpy = sinon.spy(notification, 'emit');
			networkmgr = new EventEmitter();
			networkmgr.connectWifi = testutil.noop;
			networkmgr.disconnectWifi = testutil.noop;
			networkmgr.activeConnection = testutil.noop;

			connectionEvtspy = sinon.spy();
			connectionErrEvtspy = sinon.spy();

			wifi = new shell.WiFiConnection();
			wifi.on('connection', connectionEvtspy);
			wifi.on('connectionError', connectionErrEvtspy);
			wifi.addAPIs({
				'networkmgr': networkmgr,
				'notification': notification
			});
			wifi.init();
		});

		afterEach(function() {
			wifi = undefined;
			notifyEmitSpy.reset();
		});

		describe("connect", function() {
			xit("should be connecting until connection state is resolved", function() {
				// TODO
			});

			it("should notify the connection", function() {
				var stub = wifiConnect(true);
				notifyEmitSpy.withArgs('InternetConnection', {name: 'connecting'}).calledOnce.should.be.true;
				stub.restore();
			});

			describe('when success', function() {
				var stub;
				beforeEach(function() {
					stub = wifiConnect(true);
				});

				afterEach(function() {
					stub.restore();
				});

				it("should be connected", function() {
					wifi.get('state').should.be.equal('connected');
				});

				it("should have a connection", function() {
					wifi.get('connection').should.be.equal('someSSID');
				});

				it("should emit a connection event", function() {
					notifyEmitSpy.withArgs('InternetConnection', {name: 'wirelessConnected'}).calledOnce.should.be.true;
				});

				it('should emit a connection event', function() {
					connectionEvtspy.withArgs(sinon.match.any, "connection", sinon.match.string).calledOnce.should.be.true;
				});

				describe('and then connect other ssid and fails', function() {
					it("should have not a connection", function() {
						stub.restore();
						var s = wifiConnect(false, 'otherSSID');
						should.not.exist(wifi.get('connection'));
						s.restore();
					});
				});
			});

			describe('when fails', function() {
				var stub;
				beforeEach(function() {
					stub = wifiConnect(false);
				});

				afterEach(function() {
					stub.restore();
				});

				it("should be not connected", function() {
					wifi.get('state').should.equal('disconnected');
				});

				it("should has not a connection", function() {
					should.not.exist(wifi.get('connection'));
				});

				it("should notify the failure", function() {
					notifyEmitSpy.withArgs('InternetConnection', {name: 'failed'}).calledOnce.should.be.true;
				});

				it('should emit a connection error event', function() {
					connectionErrEvtspy.withArgs(sinon.match.any, "connectionError", sinon.match.instanceOf(Error)).calledOnce.should.be.true;
				});
			});
		});

		describe("disconnect", function() {
			var connStub;
			var disconnectSpy;

			beforeEach(function() {
				disconnectSpy = sinon.spy();
				wifi.on('disconnect', disconnectSpy);
				connStub = wifiConnect(true);
			});

			afterEach(function() {
				connStub.restore();
				disconnectSpy.reset();
				connStub = undefined;
				disconnectSpy = undefined;
			});

			describe('when success', function() {
				beforeEach(function() {
					wifiDisconnect(true);
				});

				it("should emit disconnect event", function() {
					disconnectSpy.withArgs(sinon.match.any, "disconnect").calledOnce.should.be.true;
				});

				it("should remove the connection", function() {
					should.not.exist(wifi.get('connection'));
				});
			});

			describe('when fails', function() {
				beforeEach(function() {
					wifiDisconnect(false);
				});

				it("should not emit disconnect", function() {
					disconnectSpy.withArgs(sinon.match.any, "disconnect").called.should.be.false;
				});

				it("should be connected", function() {
					wifi.get('state').should.be.equal('connected');
				});
			});
		});

	});

	describe("InternetConnectionModel", function() {
		var networkmgr;
		var notification;
		var model;

		var wiredConnected = false;
		var wifiConnected = false;

		function connectWifi() {
			wifiConnected = true;
			if (!wiredConnected) {
				networkmgr.emit('changed', {type: 'connected', state: wifiConnected, connection: {type: 'wireless'}});
			}
		}

		function connectWire() {
			wiredConnected = true;
			networkmgr.emit('changed', {type: 'connected', state: wiredConnected, connection: {type: 'wired'}});
		}

		function disconnectWire() {
			wiredConnected = false;
			networkmgr.emit('changed', {type: 'connection lost', state: wifiConnected, connection: {type: 'wired'}});
			// active connection changed
			if (wifiConnected) {
				connectWifi();
			}
		}

		function disconnectWifi() {
			wifiConnected = true;
			console.log('disconnectWifi: %s', wiredConnected);
			networkmgr.emit('changed', {type: 'connection lost', state: wiredConnected, connection: {type: 'wireless'}});
		}

		it("should be not initialized at creation time", function() {
			var conn = new shell.InternetConnectionModel();
			should.not.exist(model.get('connected'));
			should.not.exist(model.get('connection'));
		});

		beforeEach(function() {
			networkmgr = new EventEmitter();
			networkmgr.activeConnection = function(cbk) {
				cbk(undefined, undefined); // no connection
			};
			networkmgr.isConnected = function(cbk) {
				cbk(undefined, false); // not connected
			};
			notification = {on: testutil.noop, emit: sinon.spy(), silence: testutil.noop, unsilence: testutil.noop };
			networkmgr.getDevices = function(cbk) {
				cbk(undefined, []);
			}

			model = new shell.InternetConnectionModel().addAPIs({
				'networkmgr': networkmgr,
				'notification': notification,
			}).init();
		});

		afterEach(function() {
			model = undefined;
			notification.emit = undefined;
			wiredConnected = false;
			wifiConnected = false;
		});

		describe("#fetchStatus", function() {

			it("should emit an event", function(done) {
				model.on('fetchStatus', function() {
					notification.emit.called.should.be.true;
					done();
				});
				model.fetchStatus();
			});

			it("should not notify", function() {
				var spy = sinon.spy(notification, 'silence');
				model.fetchStatus();
				spy.calledOnce.should.be.true;
			});

			describe("when is disconnected", function() {
				beforeEach(function() {
					model.getAPI('networkmgr').isConnected = function(cbk) {
						cbk(undefined, false);
					};

					model.getAPI('networkmgr').activeConnection = function(cbk) {
						cbk(undefined, undefined);
					};
				});

				it("should not be connected", function(done) {
					model.on('fetchStatus', function() {
						model.get('connected').should.be.false;
						done();
					});
					model.fetchStatus();
				});

				it('should have not a connection', function(done) {
					model.on('fetchStatus', function() {
						should.not.exist(model.get('connection'));
						done();
					});
					model.fetchStatus();
				});

			});

			describe("when is connected", function() {
				var conn = {};
				beforeEach(function() {
					model.getAPI('networkmgr').isConnected = function(cbk) {
						cbk(undefined, true);
					};

					model.getAPI('networkmgr').activeConnection = function(cbk) {
						cbk(undefined, conn);
					};
				});

				it("should be actived", function(done) {
					model.on('fetchStatus', function() {
						model.get('connected').should.be.true;
						done();
					});
					model.fetchStatus();
				});

				it('should have a connection', function(done) {
					model.on('fetchStatus', function() {
						model.get('connection').should.be.equal(conn);
						done();
					});
					model.fetchStatus();
				});
			});
		});

		describe("when connect", function() {

			it("should be connected", function() {
				networkmgr.emit('changed', {type: 'connected', state: true, connection: {type: 'wireless'}});
				model.get('connected').should.be.true;
			});

			it("should emit a new connection notification", function() {
				networkmgr.emit('changed', {type: 'connected', state: true, connection: {type: 'wireless'}});
				notification.emit.withArgs('InternetConnection', {'name': 'active'}).calledOnce;
			});

			describe('the wire', function() {

				beforeEach(connectWire);

				it("should have a wire connection", function() {
					model.get('connection').type.should.be.equal('wired');
				});

				describe("and connect the wireless", function() {

					beforeEach(connectWifi);

					it("should be connected", function() {
						model.get('connected').should.be.true;
					});

					it("should have a wire connection", function() {
						model.get('connection').type.should.be.equal('wired');
					});

					describe("and then disconnect the wireless", function() {
						beforeEach(disconnectWifi);

						it("should be connected", function() {
							model.get('connected').should.be.true;
						});

						it("should have a wire connection", function() {
							model.get('connection').type.should.be.equal('wired');
						});
					});
				});

				describe("and then disconnect", function() {
					beforeEach(disconnectWire);

					it("should be not connected", function() {
						model.get('connected').should.be.false;
					});

					it("should emit a connection deactive notification", function() {
						notification.emit.withArgs('InternetConnection', {'name': 'deactive'}).calledOnce;
					});

					it("should have not a connection", function() {
						should.not.exist(model.get('connection'));
					});
				});
			});

			describe("the wireless", function() {
				beforeEach(connectWifi);

				it("should be connected", function() {
					model.get('connected').should.be.true;
				});

				it("should have a wireless connection", function() {
					model.get('connection').type.should.be.equal('wireless');
				});

				it("should notify the new wireless connection", function() {
					notification.emit.withArgs('InternetConnection', {'name': 'wirelessConnected'}).calledOnce;
				});

				describe("and connect the wire", function() {
					beforeEach(connectWire);

					it("should be connected", function() {
						model.get('connected').should.be.true;
					});

					it("should have a wire connection", function() {
						model.get('connection').type.should.be.equal('wired');
					});

					describe("and disconnect the wire", function() {
						beforeEach(disconnectWire);

						it("should have a wireless connection", function() {
							model.get('connection').type.should.be.equal('wireless');
						});
					});
				});

				describe("and dicconnect", function() {
					beforeEach(disconnectWifi);

					it("should be not connected", function() {
						model.get('connected').should.be.false;
					});

					it("should emit a connection deactive notification", function() {
						notification.emit.withArgs('InternetConnection', {'name': 'deactive'}).calledOnce;
					});

					it("should have not a connection", function() {
						should.not.exist(model.get('connection'));
					});
				});
			});
		});

		afterEach(function() {
			model = undefined;
		});
	});
});
