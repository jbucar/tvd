'use strict';

var _ = require('lodash');
var os = require('os');
var uuid = require('node-uuid');
var ssdp = require('peer-ssdp');

function Dial() {
	var self = {};

	//	Implementation
	var _id = uuid.v4();
	var _ssdp = null;
	var _prefix = '/dial';
	var _ctrlURL = '/ssdp/notfound';
	var _serviceTypes = [
		"urn:dial-multiscreen-org:service:dial:1",
		"urn:dial-multiscreen-org:device:dial:1",
		"upnp:rootdevice",
		"ssdp:all",
		"uuid:" + _id
	];
	var _server = os.type() + "/" + os.release() + " UPnP/1.1 TAC/1.0.0";
	var _location = null;

	function makePath(p) {
		return _prefix + '/' + p;
	}

	function makeDeviceDescriptor( base ) {
		var xml = '<?xml version="1.0"?>';
		xml += '<root xmlns="urn:schemas-upnp-org:device-1-0">';
		xml += '  <specVersion>';
		xml += '    <major>1</major>';
		xml += '    <minor>0</minor>';
		xml += '  </specVersion>';
		xml += '  <URLBase>' + base + '</URLBase>';
		xml += '  <device>';
		xml += '    <deviceType>urn:dial-multiscreen-org:device:dial:1</deviceType>';
		xml += '    <friendlyName>TAC</friendlyName>';
		xml += '    <manufacturer>LIFIA</manufacturer>';
		xml += '    <modelName>TAC aml_isdbt_1</modelName>';
		xml += '    <UDN>uuid:' + _id + '</UDN>';
		xml += '    <iconList>';
		xml += '      <icon>';
		xml += '        <mimetype>image/png</mimetype>';
		xml += '        <width>144</width>';
		xml += '        <height>144</height>';
		xml += '        <depth>32</depth>';
		xml += '        <url>/img/icon.png</url>';
		xml += '      </icon>';
		xml += '    </iconList>';
		xml += '    <serviceList>';
		xml += '      <service>';
		xml += '        <serviceType>urn:dial-multiscreen-org:service:dial:1</serviceType>';
		xml += '        <serviceId>urn:dial-multiscreen-org:serviceId:dial</serviceId>';
		xml += '        <controlURL>' + _ctrlURL + '</controlURL>';
		xml += '        <eventSubURL>' + _ctrlURL + '</eventSubURL>';
		xml += '        <SCPDURL>' + _ctrlURL + '</SCPDURL>';
		xml += '      </service>';
		xml += '    </serviceList>';
		xml += '  </device>';
		xml += '</root>';

		return xml;
	}

	function makeDeviceRenderer( appName, state, pid ) {
		var xml = '<?xml version="1.0" encoding="UTF-8"?>';
		xml += '<service xmlns="urn:dial-multiscreen-org:schemas:dial" dialVer="1.7">';
		xml += '    <name>' + appName + '</name>';
		xml += '    <options allowStop="true"/>';
		xml += '    <state>' + state + '</state>';
		if (state === 'running') {
			xml += '    <link rel=\"' + pid + '" href=\"' + pid + '" />';
		}
		xml += '</service>';
		return xml;
	}

	function createPeer(st) {
		return {
			NT: st,
			USN: "uuid:" + _id + "::" + st,
			SERVER: _server,
			LOCATION: _location
		};
	}

	function startServer() {
		_location = "http://{{networkInterfaceAddress}}:" + self.port() + _prefix + "/ssdp/device-desc.xml";

		_ssdp = ssdp.createPeer();

		_ssdp.on( "ready", function() {
			log.silly( 'dial', 'SSDP ready' );
			for (var i=0; i<_serviceTypes.length; i++) {
				var st = _serviceTypes[i];
				_ssdp.alive( createPeer(st) );
			}
		});

		_ssdp.on( "search", function(headers, address) {
			if (_serviceTypes.indexOf(headers.ST) != -1) {
				//log.silly( 'dial', 'Send search reply: headers=%j, address=%j', headers, address );
				_ssdp.reply({
					LOCATION: _location,
					ST: headers.ST,
					"CONFIGID.UPNP.ORG": 7337,
					"BOOTID.UPNP.ORG": 7337,
					USN: "uuid:" + _id + "::" + headers.ST
				}, address);
			}
		});

		_ssdp.on( "close", function() {
			log.silly( 'dial', 'SSDP closed' );
		});

		_ssdp.start();
	}

	function closeServer() {
		for (var i=0; i<_serviceTypes.length; i++) {
			var st = _serviceTypes[i];
			_ssdp.byebye( createPeer(st) );
		}
		_ssdp.close();
		_ssdp = null;
	}

	function getAppID( appmgr, appName ) {
		var apps = appmgr.getAll();
		var appIndex = _.findIndex(apps,function(id) {
			var info = appmgr.get( id );
			return info.name === appName;
		});
		return apps[appIndex];
	}

	//	API
	self.name = 'dial';
	self.useUTF8 = true;
	self.useREST = true;
	self.hidden = true;

	self.start = function() {
		log.verbose( 'remoteapi', 'Start DIAL server' );

		//	Create ssdp server and start
		startServer();

		//	getApps and getApp(appName) methods
		self.addHandler( 'GET', makePath('apps'), function(req,res) {
			if (req.routeParams.length > 0) {
				//	Check app parameter
				var appName = req.routeParams[0];
				log.silly( 'dial', 'Handle getApp(%s)', appName );

				var appmgr = self.reg().get('appmgr');
				var appID = getAppID( appmgr, appName );
				if (appID) {
					var isRunning = appmgr.isRunning( appID );
					var state = isRunning ? "running" : "stopped";
					var xml = makeDeviceRenderer( appName, state, 'run' );
					self.sendResult( res, 200, 'application/xml', xml );
				}
				else {
					self.sendResult( res, 404 );
				}
			}
			else {
				log.silly( 'dial', 'Handle getApps' );
				self.sendResult( res, 204 );
			}

			log.silly( 'dial', 'Handle end' );
		});

		self.addHandler( 'POST', makePath('apps'), function(req,res) {
			if (req.routeParams.length === 0) {
				self.sendResult( res, 404 );
				return;
			}

			//	Check app parameter
			var appName = req.routeParams[0];
			log.silly( 'dial', 'Handle runApp(%j)', req.routeParams );

			var appmgr = self.reg().get('appmgr');
			var appID = getAppID( appmgr, appName );
			if (!appID) {
				log.verbose( 'dial', 'Application not found: appName=%s', appName );
				self.sendResult( res, 404 );
				return;
			}

			//	Check is running
			if (appmgr.isRunning( appID )) {
				log.silly( 'dial', 'Application already running: appName=%s', appName );
				self.sendResult( res, 200 );
				return;
			}

			var runOpts = {
				id: appID,
				params: req.data
			};
			appmgr.runAsync( runOpts, function(err) {
				if (err) {
					log.warn( 'dial', 'Application can\'t start: appName=%s', appName );
					self.sendResult( res, 503 );
				}
				else {
					var baseURL = self.getBaseUrl(req) + _prefix;
					res.setHeader( 'LOCATION', baseURL + '/apps/' + appName + '/run');
					self.sendResult( res, 201 );
				}
			});
		});

		self.addHandler( 'DELETE', makePath('apps'), function(req,res) {
			if (req.routeParams.length === 0) {
				self.sendResult( res, 404 );
				return;
			}

			//	Check app parameter
			var appName = req.routeParams[0];
			log.silly( 'dial', 'Handle stopApp(%s)', appName );

			//	Check exists
			var appmgr = self.reg().get('appmgr');
			var appID = getAppID( appmgr, appName );
			if (!appID) {
				log.silly( 'dial', 'Application not found: appName=%s', appName );
				self.sendResult( res, 404 );
				return;
			}

			//	Check is running
			if (!appmgr.isRunning( appID )) {
				log.silly( 'dial', 'Application not running: appName=%s', appName );
				self.sendResult( res, 400 );
				return;
			}

			appmgr.stop( appID, function(err) {
				self.sendResult( res, err ? 400 : 200 );
			});
		});

		//	Device descriptors handlers
		self.addHandler( 'GET', makePath('ssdp/device-desc.xml'), function(req,res) {
			var baseURL = self.getBaseUrl(req) + _prefix;
			var xml = makeDeviceDescriptor( baseURL );
			res.setHeader("Access-Control-Allow-Method", "GET, POST, DELETE, OPTIONS");
			res.setHeader("Access-Control-Expose-Headers", "Location");
			res.setHeader('Application-URL', baseURL + '/' + 'apps');
			self.sendResult( res, 200, 'application/xml', xml );
		});

		self.addHandler( 'GET', _ctrlURL, function(req,res) {
			self.sendResult(res,404);
		});
	};

	self.stop = function() {
		closeServer();
	};

	return self;
}

// Export module hooks
module.exports = Dial;

