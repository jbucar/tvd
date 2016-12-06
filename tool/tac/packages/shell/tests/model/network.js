'use strict';

// For readding porpouses;
var when = describe;
var xwhen = xdescribe;

(function() {
    var netmock = new EventEmitter();
    var notification = new EventEmitter();
    var forwarder = new EventEmitter();
    netmock.getConnections = util.nop;
    netmock.getConnection = util.nop;
    netmock.activeConnection = util.nop;
    netmock.isConnected = util.nop;

    function connectivity(connectivity, connName, api) {
        netmock.emit('state', connectivity);
        if (connectivity && connName) {
            var emitter = api ? api : netmock;
            emitter.emit('active', connName);
        }
    }

    function connModel(name, st) {
        return {
            name: name,
            get: function(attrName) {
                var attrs = {name: name, state: st};
                return attrs[attrName];
            }
        }
    }

    describe('Network', function() {
        describe('Connectivity', function() {
            var conn;
            // Mock network dependency
            enyo.kind({
                name: 'test.Connectivity',
                kind: 'network.Connectivity',
                dependencies: {
                    network: netmock,
                    notification: notification
                }
            });

            beforeEach(function() {
                conn = new test.Connectivity();
            });

            when('is not synchronized and call sync', function() {
                it('should synchronize', function() {
                    connectivity(true);
                    var c = new test.Connectivity();
                    should.not.exist(c.get('state'));
                    var stub = sinon.stub(netmock, 'isConnected');
                    stub.yields(undefined, true);
                    c.sync();
                    c.get('state').should.be.true;

                    stub.calledOnce.should.be.true;
                });
            });

            when('there is no connectivity', function() {
                it('should be false', function() {
                    connectivity(false);
                    conn.get('state').should.be.false;
                });
            });

            when('there is connectivity', function() {
                it('should be true', function() {
                    connectivity(true);
                    conn.get('state').should.be.true;
                });
            });
        });

        describe('ActiveConnection', function() {
            var activeConn;
            // Mock network dependency
            enyo.kind({
                name: 'test.ActiveConnection',
                kind: 'network.ActiveConnection',
                dependencies: {
                    network: netmock,
                    forwarder: forwarder
                }
            });

            beforeEach(function() {
                activeConn = new test.ActiveConnection();
            });

            when('there is no active connection', function() {
                it('should be undefined', function() {
                    connectivity(false, undefined, forwarder);
                    should.not.exist(activeConn.get('connName'));
                });
            });

            when('there is connectivity', function() {
                when('is listening', function() {
                    it('should be the active connection', function() {
                        activeConn.enableAPIsBinding(true);
                        connectivity(true, 'conn', forwarder);
                        activeConn.get('connName').should.be.equal('conn');
                    });
                });

                when('no listen', function() {
                    it('should not be undefined', function() {
                        connectivity(true, 'conn', forwarder);
                        should.not.exist(activeConn.get('connName'));
                    });
                });
            });

            describe('#sync', function() {
                it('should be active connection', function() {
                    var stub = sinon.stub(netmock, 'activeConnection');
                    stub.yields(undefined, connModel('conn'));
                    activeConn.sync();
                    activeConn.get('connName').should.be.equal('conn');
                });
            });
        });

        describe('Connection', function() {
            var connections;
            var getConnectionsStub;
            var getConnectionStub;
            enyo.kind({
                name: 'test.Connections',
                kind: 'network.Connections',
                dependencies: {
                    network: netmock,
                    notification: notification,
                    forwarder: forwarder
                }
            });

            function addConnection(name) {
                forwarder.emit('connection', { name: name, type: 'added', state: 'disconnected' });
            }

            function removeConnection(name) {
                forwarder.emit('connection', { name: name, type: 'remove', state: 'disconnected' });
            }

            beforeEach(function() {
                connections = new test.Connections();
                getConnectionsStub = sinon.stub(netmock, 'getConnections');
                getConnectionStub = sinon.stub(netmock, 'getConnection');
            });

            afterEach(function() {
                getConnectionsStub.restore();
                getConnectionStub.restore();
            });

            when('there is no connections', function() {
                it('should have no connections', function() {
        			getConnectionsStub.yields(undefined, []);
                    connections.sync();

                    connections.length.should.be.equal(0);
                });

                when('add a connection without listen', function() {
                    it('should have no connections', function() {
                        connections.sync();
            			addConnection();

                        connections.length.should.be.equal(0);
                    });
                });

                when('add a connection while listening', function() {
                    it('should have connections', function() {
                        getConnectionStub.yields(undefined, {name: 'two'});
                        connections.sync();
                        connections.listen(true);
            			addConnection('two');

                        connections.length.should.be.equal(1);
                        connections.listen(false);
                    });
                });

                when('remove a connection while listening', function() {
                    it('should have one connection less', function() {
                        getConnectionsStub.yields(undefined, [{name: 'one'}, {name: 'two'}]);
                        connections.sync();
                        connections.listen(true);
                        connections.length.should.be.equal(2);

            			removeConnection('one');
                        connections.length.should.be.equal(1);
                    });
                });
            });

            when('there is connections', function() {
                it('should have connections', function() {
        			getConnectionsStub.yields(undefined, [{name: 'one'}, {name: 'two'}]);
                    connections.sync();

                    connections.length.should.be.equal(2);
                });

                when('the connections no longer exists and sync', function() {
                    it('should have only the existing connections', function() {
                        getConnectionsStub.yields(undefined, [{name: 'one'}, {name: 'two'}]);
                        connections.sync();

                        connections.length.should.be.equal(2);

            			getConnectionsStub.yields(undefined, [{name: 'one'}]);
                        connections.sync();

                        connections.length.should.be.equal(1);

                        getConnectionsStub.yields(undefined, []);
                        connections.sync();

                        connections.length.should.be.equal(0);
                    });
                });

            });

            when('a connection changes his state', function() {
                it('should forward the change', function () {
                    log.setLevel('trace')
                    var spy = sinon.spy();
                    connections.on('change', spy);
                    getConnectionStub.yields(undefined, {name:'conn1'})
                    connections.listen(true);

                    addConnection('conn1');
                    forwarder.emit('connection', { name: 'conn1', type: 'state', state: 'connecting' });

                    spy.calledOnce.should.be.true;
                });
            });

            when('a connection emits an event', function() {
                it('should forward the event', function () {
                    var spy = sinon.spy();
                    connections.on('connFail', spy);
                    getConnectionsStub.yields(undefined, [{name:'conn1'}]);
                    connections.sync();
                    var conn = connections.at(0);
                    conn.emit('connFail', {});

                    spy.calledOnce.should.be.true;
                });
            });
        });

        describe('WirelessConnection', function() {
            var wlss;
            var connections;
            enyo.kind({
                name: 'test.WirelessConnection',
                kind: 'network.WirelessConnection',
            });

            beforeEach(function() {
                connections = new EventEmitter(); // Mocked connections
                wlss = new test.WirelessConnection({connections: connections});
            });

            when('a wireless connection is connecting', function() {
                it('it should have not a connection name', function () {
                    connections.emit('change', {}, 'change', {model: connModel('conn1', 'connecting')});
                    should.not.exist(wlss.get('connName'));
                });
            });

            when('a wireless connection is connected', function() {
                beforeEach(function() {
                    connections.emit('change', {}, 'change', {model: connModel('conn1', 'connected')});
                });

                it('should have the connection name', function () {
                    wlss.get('connName').should.be.equal('conn1');
                });

                when('a disconnect the wireless', function() {
                    it('should not have a connection name', function () {
                        connections.emit('change', {}, 'change', {model: connModel('conn1', 'disconnected')})
                        should.not.exist(wlss.get('connName'));
                    });
                });
            });
        });

        describe('WirelessAP', function() {
            // enyo.kind({
        	// 	name: "network.WirelessAP"
            // })
            when('a wireless ap is connecting and then disconnected', function() {
                it('should emit a failConn event', function() {
                    var ap = new network.WirelessAP();
                    var spy = sinon.spy();
                    ap.on('connFail', spy);

                    ap.set('state', 'connecting');
                    ap.set('state', 'disconnected');

                    spy.calledOnce.should.be.true;

                });
            });
        });

    });
})();
