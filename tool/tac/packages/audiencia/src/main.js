'use strict';

var assert = require('assert');
var crypto = require('crypto');
var tvdutil = require('tvdutil');
var https = require('https');
var UrlParser = require('url');

//	Events
var screenEvent = 'screenActive';
var tvdEvent = 'currentChannel';

function createAudiencia( adapter ) {
	var self = {};
	var _cfg = null;
	var _currentSegment = null;
	var _segments = [];
	var _sendTimer = null;
	var _ready = {};
	var _sendDataCondition = null;

	function defaultConfig() {
		return {
			"stbID": null,
			"privateKey": null,
			"url": "https://172.16.0.210",
			"sendTimeout": 10*60*1000,
			"minElapsedTime": 1*60*1000
		};
	}

	function getSrv( srvID ) {
		assert(adapter);
		return adapter.registry().get( srvID );
	}

	function formatDate(t) {
		return Math.ceil(t.getTime() / 1000);
	}

	function startSegment() {
		stopSegment();

		_currentSegment = {
			"chID": _ready[tvdEvent],
			"startTime": new Date(),
			"endTime": null
		};
	}

	function stopSegment(restart) {
		if (_currentSegment) {
			var now = new Date();

			//	Check segment is valid
			if (now.getTime() - _currentSegment.startTime.getTime() > _cfg.minElapsedTime) {
				//	Setup segment
				_currentSegment.endTime = now;
				_currentSegment.channel = getSrv('tvd').get( _currentSegment.chID ).channel;
				log.silly( 'audiencia', 'Add segment: currentSegment=%j', _currentSegment );
				_segments.push( _currentSegment );
				_currentSegment = null;

				if (restart) {
					startSegment();
					_currentSegment.startTime = now;
				}
			}

			if (!restart) {
				//	Reset current
				_currentSegment = null;
			}
		}
	}

	function onReadyChanged( evt, st ) {
		if (_ready[evt] != st) {
			_ready[evt] = st;

			stopSegment(false);

			var canAudit = (_ready[tvdEvent] !== -1 && _ready[screenEvent] !== false);
			log.verbose( 'audiencia', 'State changed: evt=%s, st=%s, canAudit=%d', evt, st, canAudit );

			if (canAudit) {
				startSegment();
			}
		}
	}

	function onServerResponse( err, res ) {
		//	Restart timer (before dec -> onStop clear timeout!)
		_sendTimer = setTimeout( onSendData, _cfg.sendTimeout );

		//	Remove lock state
		_sendDataCondition.dec();

		if (res && res.statusCode === 200) {
			log.verbose( 'audiencia', 'Segment added!' );
			_segments = [];
		}
		else if (err) {
			log.warn( 'audiencia', 'Error trying to send data to server: url=%s, err=%s', _cfg.url, err.message );
		}
		else {
			log.warn( 'audiencia', 'Error trying to send data to server: url=%s, st=%d', _cfg.url, res.statusCode );
		}
	}

	function sendData() {
		//	Format segments
		var data = _segments.map(function(item) {
			return {
				"endTime": formatDate(item.endTime),
				"startTime": formatDate(item.startTime),
				"channel": item.channel
			};
		});

		log.verbose( 'audiencia', 'Send segments: segments=%d', _segments.length );

		//	Stringify data
		var jsonData = JSON.stringify(data);
		var curTime = formatDate(new Date());

		//	Calculate signature to data
		var sign = crypto.createSign("RSA-MD5");
		sign.write( jsonData );
		var signMsg = sign.sign( _cfg.privateKey, 'hex' );

		//	Lock stop
		_sendDataCondition.inc();

		//	Setup url and post data
		var url = _cfg.url + '/add/';
		var postData = 'stbID=' + _cfg.stbID + '&data=' + jsonData + '&signature=' + signMsg + '&currentTime=' + curTime;

		//	Send data
		var oURL = UrlParser.parse(url);
		oURL.method = 'POST';
		oURL.rejectUnauthorized = false;
		oURL.headers = {
			'Content-Type': 'text/html',
			'Content-Length': Buffer.byteLength(postData)
		};
		var req = https.request( oURL, function(res) {
			onServerResponse(undefined,res);
		});
		req.on( 'error', onServerResponse );
		req.write( postData );
		req.end();
	}

	function requestID() {
		var opts = {
			"url": _cfg.url + "/audimeter_request_id/",
			"method": "GET",
			"secure": true
		};

		log.verbose( 'audiencia', 'Request ID from server: url=%s', _cfg.url );
		tvdutil.doRequest( opts, function( err, res ) {
			if (err) {
				log.warn( 'audiencia', 'Error requesting ID from server: url=%s, err=%s', _cfg.url, err.message );
			}
			else if (res.statusCode === 200) {
				//	Validate data
				if (res.data.stbID && (typeof res.data.stbID === 'number') &&
					res.data.privateKey && (typeof res.data.privateKey === 'string'))
				{
					//	Set new config
					_cfg.stbID = res.data.stbID;
					_cfg.privateKey = res.data.privateKey;

					log.verbose( 'audiencia', 'Saving new config: _cfg=%j', _cfg );

					adapter.save( 'config.json', _cfg );

					//	Start sending data
					_sendTimer = setTimeout( onSendData, _cfg.sendTimeout );
				}
			}
		});
	}

	//	Events
	function onSendData() {
		log.silly( 'audiencia', 'On send data' );

		//	Restart segment
		stopSegment(true);

		if (_segments.length > 0) {
			sendData();
		}
		else {
			//	Register send timer
			_sendTimer = setTimeout( onSendData, _cfg.sendTimeout );
		}
	}

	function onChangeChannel( chID ) {
		log.silly( 'audiencia', 'On channel changed: chID=%d', chID );
		onReadyChanged( tvdEvent, chID );
	}

	function onScreenActiveChanged( isActive ) {
		log.silly('audiencia', 'Screen active changed: isActive=%d', isActive );
		onReadyChanged( screenEvent, isActive );
	}

	//	Service API
	self.onStart = function(cb) {
		//	Initialize
		_cfg = adapter.load( 'config.json', defaultConfig() );
		_currentSegment = null;
		_segments = [];
		_ready = {
			tvdEvent: -1,
			screenEvent: false
		};

		//	Setup exit condition
		_sendDataCondition = new tvdutil.BasicWaitCondition();

		{	//	Setup tvd notifications
			var tvd = getSrv('tvd');
			assert(tvd);
			onChangeChannel( tvd.current() );
			tvd.on( 'playerChanged', onChangeChannel );
		}

		{	//	Setup screen notifications
			var screen = getSrv('screen');
			assert(screen);
			onScreenActiveChanged( screen.isActive() );
			screen.on('changed', onScreenActiveChanged );
		}

		//	Is configurated?
		if (_cfg.stbID) {
			//	Register send timer
			_sendTimer = setTimeout( onSendData, _cfg.sendTimeout );
		}
		else {
			requestID();
		}

		//	Add basic API for testing only ....
		var api = {};
		api.segments = function() {
			return _segments;
		};
		cb( undefined, api );
	};

	self.onStop = function(cb) {
		_sendDataCondition.wait(function() {
			clearTimeout( _sendTimer );
			_sendTimer = null;

			{	//	Remove tvd listener
				var tvd = getSrv('tvd');
				assert(tvd);
				tvd.removeListener( 'playerChanged', onChangeChannel );
			}

			{	//	Remove listener for screen active changed event
				var screen = getSrv('screen');
				assert(screen);
				screen.removeListener('changed', onScreenActiveChanged );
			}

			cb();
		});
	};

	return Object.freeze(self);
}

// Export module hooks
module.exports = createAudiencia;

