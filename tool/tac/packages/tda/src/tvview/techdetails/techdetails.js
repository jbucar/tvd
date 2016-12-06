/* globals Dialog */
/* exported TechnicalDetails */
function TechnicalDetails(res) {

	function get( val ) {
		return (val === -1) ? '-' : val;
	}

	function hide(ractive) {
		log.verbose( 'TechnicalDetails', 'hide' );
		ractive.set('show', false);
		if (ractive.timeout) {
			clearTimeout( ractive.timeout );
		}
	}

	function show( ractive, forceShow ) {
		log.verbose( 'TechnicalDetails', 'show' );

		apiCallAsync( tvd.player.current )
			.then(function(chID) {
				if (chID === -1) {
					return;
				}

				return Promise.all([
					apiCallAsync( tvd.channel.get, chID ),
					apiCallAsync( tvd.player.audioInfo ),
					apiCallAsync( tvd.player.videoInfo ),
					apiCallAsync( tvd.player.status )
				]).spread(function( chInfo, audioInfo, videoInfo, status ) {
					assert(chInfo);
					assert(audioInfo);
					assert(videoInfo);
					assert(status);

					let opts = {
						techAudioPID: get(audioInfo.pid),
						techChannels: get(audioInfo.channels),
						techAudioRate: get(audioInfo.rate),
						techVideoPID: get(videoInfo.pid),
						techVideoResolution: (videoInfo.height !== -1 && videoInfo.width !== -1) ? videoInfo.width + ' * ' + videoInfo.height : '-',
						techVideoRate: get(videoInfo.fps),
						techSignal: status.signal,
						techSignalQuality: status.quality,
						techFrequency: chInfo.info.frecuency,
						techChannelID: chInfo.info.nitID.toString(16) + '.' + chInfo.info.tsID.toString(16) + '.' + chInfo.info.srvID.toString(16),
						techChannelNumber: chInfo.channel
					};

					if (forceShow) {
						opts.show = true;
					}

					ractive.set(opts);

					if (ractive.timeout) {
						clearTimeout( ractive.timeout );
					}
					ractive.timeout = setTimeout( $(show,ractive,false), 3000 );
				});
		});
	}

	let prot = {
		template: res.tpl('techdetails'),
		css: res.style('techdetails'),
		components: ui.getComponents(Dialog),
		isolated: true,
	};

	prot.onconstruct = function(opts) {
		log.info( 'TechnicalDetails', 'onconstruct: opts=%j', opts );
		opts.data.show = false;
		opts.data.theme = ui.getTheme();
		opts.data.theme.bgColor = 'white';
	};

	prot.oninit = function() {
		log.info( 'TechnicalDetails', 'oninit' );
		let opts = {
			techTitle: res.txt('title'),
			techAudio: res.txt('audio'),
			techPIDLabel: res.txt('pid'),
			techChannelsLabel: res.txt('channels'),
			techRateLabel: res.txt('rate'),
			techVideo: res.txt('video'),
			techResolutionLabel: res.txt('resolution'),
			techSignalLabel: res.txt('signal'),
			techSignalLevel: res.txt('signalLevel'),
			techSignalQualityLabel: res.txt('signalQuality'),
			techFrequencyLabel: res.txt('frequency'),
			techChannelLabel: res.txt('channel'),
			techChannelIDLabel: res.txt('channelID'),
			techChannelNumberLabel: res.txt('channelNumber')
		};

		this.set(opts);
	};

	prot.toggle = function() {
		log.info( 'TechnicalDetails', 'toggle' );
		let st = this.get('show');
		if (st) {
			hide(this);
		}
		else {
			show(this,true);
		}
	};

	prot.onunrender = function() {
		log.info( 'TechnicalDetails', 'toggle' );
		hide(this);
	};

	return {
		proto: prot
	};
}

