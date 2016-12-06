enyo.kind({
	name: 'ServiceModel',
	kind: 'enyo.Model',
	attributes: {
		id: 0,
		name: 'Nombre del servicio no disponible.',
		pkgID: 0,
		version: '0.1',
		vendor: '',
		description: 'Descripción no disponible.',
		canConfigure: false,
		canUninstall: false,
		status: 'unknown',
		running: undefined
	},
	fetch: function() {
		srvmgr.get(this.get('id'), util.catchError(function getServiceInfo(srv) {
			this.set(srv);
			this.fetchState();
		}.bind(this)));
	},
	fetchState: function() {
		var current = this.get('status');
		if (current !== 'enabling' && current !== 'disabling') {
			log.info('fetchState current=%s id=%s', current, this.get('id'));
			srvmgr.isEnabled(this.get('id'), util.catchError(function srvIsEnabled(enabled) {
				this.set('status', enabled? 'enabled' : 'disabled');
			}.bind(this)));
		}
	},
	toggleMode: function() {
		var currentSt = this.get('status');
		var newSt = (currentSt === 'enabled')? 'disabled' : 'enabled';

		this.set('status', (newSt === 'enabled')? 'enabling' : 'disabling');
		srvmgr.enable(this.get('id'), newSt === 'enabled', util.ensureDelay(function enableSrv(err) {
			this.set('status', err? currentSt : newSt);
		}.bind(this), 2000));
	},
	statusChanged: function() {
		this.isRunning();
		log.info('Services', 'statusChanged', 'Change status service id=%s status=%s running=%s', this.get('id'), this.get('status'), this.get('running'));
	},
	config: function () {
		var id = this.get('id');
		log.trace('Services', 'config', 'Configure services id=%s', id);
		appmgr.configureService(id, util.catchError(function(result) {
			var st = result ? ' sucess' : ' fails';
			log.debug('Services', 'config', 'Configure services id=%s status=%s', id, st);
		}));
	},
	resetConfig: function() {
		this.set('status', 'resetting');
		srvmgr.resetConfig(this.get('id'), this.fetchState.bind(this));
	},
	uninstall: function() {
		pkgmgr.uninstall({'id': this.get('pkgID')}, function onUninstallService(err) {
			if (!err) {
				this.destroy();
			}
		}.bind(this));
	},
	isRunning: function() {
		srvmgr.isRunning( this.get('id'), util.catchError(function srvIsRunning(result) {
			this.set('running', result);
		}.bind(this)) );
	}
});

enyo.kind({
	name: 'ServicesCollection',
	kind: 'enyo.Collection',
	model: 'ServiceModel',
	fetch: function() {
		srvmgr.getAll(util.catchError(function servicesFetch(apps) {
			var models = this.add(apps);
			this.emit('loaded', {});
			models.forEach(function(m) {
				m.fetch();
			});
		}.bind(this)));
	}
});
