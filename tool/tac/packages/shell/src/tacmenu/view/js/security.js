var security = (function(enyo) {
	'use strict';

	var login = util.promisify(session.checkPass);
	var changePass = util.promisify(session.changePass);

	function _openSession(force) {
		var shouldRequest = session.shouldRequestPass();
		if (shouldRequest || force) {
			return keyboard.show({title: (force && !shouldRequest)? 'INGRESE CONTRASEÑA' : 'INICIO DE SESIÓN'}).then(function(data) {
				if (data.status === 'accepted') {
					return login(data.input).then(function(ok) {
						if (!ok) {
							enyo.Spotlight.States.push('focus');
							return dialogs.error({desc: 'Contraseña incorrecta'}).then(util.identityFn(ok));
						}
						return ok;
					});
				}
				return false;
			})
			.catch(log.error.bind(log, 'Security', '_openSession'));
		}
		else {
			// Session already open or inexistent.
			return Promise.resolve(true);
		}
	}

	function _openCreateSession(opts) {
		if (!session.get('hasPassword')) {
			enyo.Spotlight.States.push('focus');
			return dialogs.confirm(opts).then(function(ok) {
				if (ok) {
					return keyboard.show({title: 'CREAR CONTRASEÑA'}).then(function(data) {
						if (data.status === 'accepted') {
							return changePass(data.input).then(util.true);
						}
						return false;
					})
					.catch(log.error.bind(log, 'Security', '_openCreateSession'));
				}
				return false;
			}.bind(this));
		}
		else {
			return _openSession();
		}
	}

	return { openSession: _openSession, openCreateSession: _openCreateSession };

})(enyo);
