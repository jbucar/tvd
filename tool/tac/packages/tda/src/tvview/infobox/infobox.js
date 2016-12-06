/* exported InfoBox */
function InfoBox(res) {

	function formatTime( tt ) {
		if (tt === '') {
			return '-';
		}
		let fmt = n => ('0'+n).slice(-2);
		let d = new Date( tt );
		return fmt(d.getHours()) + ':' + fmt(d.getMinutes()) + 'HS';
	}

	function hideBox() {
		log.verbose( 'InfoBox', 'hideBox' );
		this.set('show', false);
	}

	function formatLanguage(lan) {
		if (lan !== 'Unknown') {
			try {
				return res.txt(lan);
			}
			catch(err) {
				log.warn( 'InfoBox', 'Unknown language: %s', lan );
			}
		}
		return '-';
	}

	function showInfo( ractive, updateData, autohide ) {
		let chID = ractive.currentChannel;
		log.verbose( 'InfoBox', 'Show info: chID=%d, updateData=%s', chID, updateData );

		if (chID === undefined || chID === -1) {
			return;
		}

		Promise.all([apiCallAsync( tvd.channel.getShow, chID ), apiCallAsync( tvd.player.audioInfo )])
			.spread(function(sInfo,aInfo) {
				assert(sInfo);
				assert(aInfo);

				let chInfo = model.get('channels').get(chID);
				assert(chInfo);

				let opts = {
					icon: (chInfo.logo && chInfo.logo.length) ? chInfo.logo : res.img('../../menu/default_channel'),
					channel: chInfo.channel + '-' + chInfo.name,
					sections: {
						PROGRAMA: sInfo.name,
						INICIO: formatTime(sInfo.startTime),
						TERMINA: formatTime(sInfo.endTime),
						IDIOMA: formatLanguage(aInfo.lang),
						EDAD: sInfo.parentalAge ? sInfo.parentalAge : '-'
					},
				};

				if (!updateData) {
					opts.show = true;
					if (ractive.timeout) {
						clearTimeout( ractive.timeout );
					}
					if (autohide) {
						ractive.timeout = setTimeout(hideBox.bind(ractive), 4000);
					}
				}

				ractive.set( opts );
			});
	}

	function onChannelChanged( ractive, chID ) {
		log.verbose( 'InfoBox', 'onChannelChanged: chID=%d', chID );
		ractive.currentChannel = chID;
		showInfo( ractive, false, true );
	}

	function onCurrentShowChanged( ractive ) {
		if (ractive.get('show')) {
			showInfo( ractive, true, true );
		}
	}

	let prot = {
		template: res.tpl('infobox'),
		css: res.style('infobox'),
		isolated: true,
	};

	prot.onconstruct = function(opts) {
		log.info( 'Infobox', 'onconstruct: opts=%j', opts );
		opts.data.show = false;
	};

	prot.oninit = function() {
		log.info( 'Infobox', 'oninit' );
		tvd.on('playerChanged', $(onChannelChanged,this) );
		tvd.on('currentShowChanged', $(onCurrentShowChanged,this) );
	};

	prot.toggle = function(autohide) {
		let st = this.get('show');
		if (st) {
			hideBox.call(this);
		}
		else {
			showInfo( this, false, autohide );
		}
	};

	return {
		proto: prot,
	};
}
