/* globals registerNotification */
/* exported Volume */
function Volume(res) {

	function hideVolume() {
		log.verbose( 'Volume', 'hideVolume' );
		this.set('show', false).then( () => this.parent.showLayer(false) );
		this.timeout = null;
	}

	function setVolume( evt ) {
		log.verbose( 'Volume', 'Set volume: vol=%d, isMuted=%s', evt.volume, evt.isMuted );

		this.set({
			volume: evt.volume,
			isMuted: evt.isMuted,
			show: true,
			icon: (evt.isMuted || evt.volume<=0) ? res.img('mute') : res.img('vol'),
		});

		if (this.timeout) {
			clearTimeout( this.timeout );
			this.timeout = null;
		} else {
			this.parent.showLayer( true );
		}
		this.timeout = setTimeout(hideVolume.bind(this), 3000 );
	}

	let prot = {
		template: res.tpl('volume'),
		css: res.style('volume'),
		isolated: true,
	};

	prot.onconstruct = function(opts) {
		log.info( 'Volume', 'onconstruct: opts=%j', opts );
		opts.data.volume = 50;
		opts.data.isMuted = false;
		opts.data.show = false;
		opts.data.icon = res.img('vol');
	};

	prot.oninit = function() {
		log.info( 'Volume', 'oninit' );
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.audio', 'changed', 'onMasterVolumeChanged', setVolume.bind(this));
	};

	return {
		proto: prot,
	};
}
