/* globals ui, EventEmitter */
/* exported Session */

function Session() {
	var self = {};
	var _isEnabled = false;
	var _isBlocked = true;
	var _events = new EventEmitter();
	var _res;

	function onKeyDown( code ) {
		if (code === tacKeyboardLayout.VK_YELLOW && _isEnabled) {
			if (_isBlocked) {
				self.unblock();
			}
			else {
				apiCallAsync( tvd.session.expireSession );
			}
			return false;
		}
		return true;
	}

	function showNotify( msgID ) {
		notification.emit( 'ApplicationNotification', {
			msg: _res.txt(msgID),
			icon: _res.img('notification_session',true)
		});
	}

	function onSessionEnabled( isEnabled ) {
		log.verbose( 'Session', 'onSessionEnabled: isEnabled=%s', isEnabled );
		_isEnabled = isEnabled;
		showNotify( isEnabled ? 'enabled' : 'disabled' );
		_events.emit( 'changed', _isEnabled, self.isBlocked() );
	}

	function onSessionChanged( isBlocked ) {
		log.verbose( 'Session', 'onSessionChanged: isBlocked=%s', isBlocked );
		_isBlocked = !isBlocked;
		showNotify( isBlocked ? 'active' : 'blocked' );
		_events.emit( 'changed', _isEnabled, !isBlocked );
	}

	//	API
	forwardEventEmitter( self, _events );

	self.init = function() {
		_res = ui.getResources( 'session' );

		tvd.on( 'sessionEnabled', onSessionEnabled );
		tvd.on( 'sessionChanged', onSessionChanged );

		return Promise.join(
			apiCallAsync( tvd.session.isSessionEnabled ),
			apiCallAsync( tvd.session.isBlocked ),
			function( isEnabled, isBlocked ) {
				let changed = (_isBlocked != isBlocked) || (_isEnabled != isEnabled);
				_isBlocked = isBlocked;
				_isEnabled = isEnabled;
				if (changed) {
					_events.emit( 'changed', _isEnabled, _isBlocked );
				}
				ui.registerKeyHandler( onKeyDown );
			});
	};

	self.fin = function() {
		ui.unregisterKeyHandler( onKeyDown );
		//	TODO: Remove on listener
	};

	self.isEnabled = function() {
		return _isEnabled;
	};

	self.isBlocked = function() {
		return _isBlocked;
	};

	self.isActive = function() {
		return _isEnabled && !_isBlocked;
	};

	self.expire = function() {
		apiCallAsync( tvd.session.expireSession );
	};

	self.unblock = function() {
		let keyboardOpts = {
			title: ui.getResource( 'strings', 'settings/restrictions/sessionTitle' ),
			placeholder: ui.getResource( 'strings', 'settings/restrictions/pin_description' ),
			type: 'password',
			bgColor: 'rgb(44,44,44)',
			bgImage: ui.getResources('menu').img('background', true),
		};
		return apiCallAsync( appmgr.showVirtualKeyboard, keyboardOpts )
			.then( data => apiCallAsync( tvd.session.checkSession, data.input ) );
	};

	self.setPass = function( pass ) {
		log.verbose( 'Restrictions', 'setPass: pass=%j', pass );
		return apiCallAsync( tvd.session.enableSession, pass );
	};

	self.clearPass = function() {
		apiCall( tvd.session.enableSession, '' );
	};

	self.btnText = function() {
		return _res.txt('btnSession');
	};

	self.btnImage = function() {
		return _res.img('init_session');
	};

	return self;
}

