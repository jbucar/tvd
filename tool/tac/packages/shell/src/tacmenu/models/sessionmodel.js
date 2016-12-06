/*
* SessionModel
*/
enyo.kind({
	name: 'SessionModel',
	kind: 'enyo.Model',
	attributes: {
		hasPassword: undefined,
		isOpen: false,
		age: 0,
		violence: false,
		drugs: false,
		sex: false
	},
	initialize: function() {
		var self = this;
		tvd.on( 'sessionChanged', function( isActive ) {
			self.set( 'isOpen', isActive );
		});

		tvd.on( 'sessionEnabled', function( isEnabled ) {
			if (self.get('hasPassword') !== undefined) {
				self.emit('onHasPasswordChanged', { open: isEnabled });
			}
			self.set( 'hasPassword', isEnabled );
			self.set( 'isOpen', isEnabled );
		});

		tvd.session.isSessionEnabled(util.catchError(function( res ) {
			self.set( 'hasPassword', res );
			if (res) {
				tvd.session.isAgeRestricted( util.catchError(function( res ) {
					self.set( 'age', res );
				}));
				tvd.session.isSexRestricted( util.catchError(function( res ) {
					self.set( 'sex', res );
				}));
				tvd.session.isDrugsRestricted( util.catchError(function( res ) {
					self.set( 'drugs', res );
				}));
				tvd.session.isViolenceRestricted( util.catchError(function( res ) {
					self.set( 'violence', res );
				}));
			}
		}));
	},
	changePass: function( value, cbk ) {
		if (value !== '') {
			tvd.session.enableSession( value );
			notification.emit( 'session', { property: 'pass' } );
			cbk();
		}
		else {
			cbk(new Error('No se permiten contraseñas vacías'));
		}
	},
	checkPass: function( pass, cbk ) {
		tvd.session.checkSession(pass, function(err) {
			if(err) { util.informError('checkSession')(); }
			cbk.apply(undefined, arguments);
		});
	},
	close: function() {
		tvd.session.expireSession();
	},
	deletePass: function() {
		this.age( 0, false );
		this.sex( false, false );
		this.drugs( false, false );
		this.violence ( false, false );
		tvd.session.enableSession('');
		notification.emit('session', { property: 'delete' });
	},
	shouldRequestPass: function() {
		return this.get('hasPassword') && !this.get('isOpen');
	},
	age: function( restrict, notify ) {
		tvd.session.restrictAge( restrict );
		this.set( 'age', restrict );
		if( notify === undefined ) {
			notification.emit( 'session', { property: 'age', value: restrict } );
		}
	},
	sex: function( restrict, notify ) {
		tvd.session.restrictSex( restrict );
		this.set( 'sex', restrict );
		if( notify === undefined ) {
			notification.emit( 'session', { property: 'sex', value: restrict } );
		}
	},
	drugs: function( restrict, notify ) {
		tvd.session.restrictDrugs( restrict );
		this.set( 'drugs', restrict );
		if( notify === undefined ) {
			notification.emit( 'session', { property: 'drugs', value: restrict } );
		}
	},
	violence: function( restrict, notify ) {
		tvd.session.restrictViolence( restrict );
		this.set( 'violence', restrict );
		if( notify === undefined ) {
			notification.emit( 'session', { property: 'violence', value: restrict } );
		}
	},
	isOpenChanged: function(was, is) {
		notification.emit('session', { property: is ? 'sessionOpen' : 'sessionClosed' });
		this.emit('onSessionChanged', { open: is });
	}
});
