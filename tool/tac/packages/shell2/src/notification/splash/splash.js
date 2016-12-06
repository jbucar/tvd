/* globals registerNotification */
/* exported Splash */
function Splash(res) {
	
	const ANIM_DURATION = 1000;
	const NUM_BALLS = 8;
	const ANIM_DELAY = ANIM_DURATION / NUM_BALLS;

	function showSplash( ractive, show ) {
		log.verbose( 'notification', 'showSplash: wasVisible=%s, show=%s', ractive.splashVisible, show );
		if (ractive.splashVisible !== show) {
			ractive.splashVisible = show;
			ractive.set( 'show', show ).then( () => ractive.parent.showLayer(show) );
		}
	}

	let prot = {
		template: res.tpl('splash'),
		css: res.style('splash'),
		isolated: true,
	};

	prot.onconstruct = function(opts) {
		log.info('Splash', 'onconstruct');

		opts.data.bgImage = res.img('splash');
		opts.data.delayPoints = Array.apply(null, Array(NUM_BALLS)).map( (_,i) => i*ANIM_DELAY );
		opts.data.delay = ANIM_DURATION;
	};
	
	prot.oninit = function() {
		this.splashVisible = false;
		registerNotification( 'ar.edu.unlp.info.lifia.tvd.appmgr', 'application', 'onApplicationStateChanged', evt => {
			log.verbose( 'notification', 'onApplicationStateChanged: evt=%j', evt );
			switch (evt.state) {
				case 'starting':
					showSplash( this, true );
					break;
				case 'loaded':
				case 'stopped':
					showSplash( this, false );
					break;
			}
		});
	};

	return {
		proto: prot,
	};
}
