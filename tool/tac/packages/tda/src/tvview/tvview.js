/* globals InfoBox, TechnicalDetails, VTTCue */
/* exported TvView */

function TvView(res) {

	function TvModel(ractive) {
		var self = {};

		self.currentChannel = function() {
			return ractive.get('currentChannel');
		};

		self.changeChannel = function( chID ) {
			log.verbose( 'TvView', 'changeChannel: chID=%s', chID );
			apiCall( tvd.player.change, chID );
			ractive.set( 'previousChannel', ractive.get('currentChannel') );
			ractive.set( 'currentChannel', chID );
		};

		self.changeFactor = function( factor ) {
			apiCallAsync( tvd.player.nextChannel, self.currentChannel(), factor )
				.then(function(chID) {
					self.changeChannel(chID);
				});
		};

		self.channelUp = function() {
			self.changeFactor( 1 );
		};

		self.channelDown = function() {
			self.changeFactor( -1 );
		};

		self.nextFavorite = function() {
			apiCallAsync( tvd.player.nextFavorite, self.currentChannel(), 1 )
				.then(function(chID) {
					self.changeChannel(chID);
				});
		};

		self.previous = function() {
			self.changeChannel( ractive.get('currentChannel') );
		};

		return self;
	}

	function onTvMediaChanged( url, started ) {
		log.verbose( 'TvView', 'onTvMediaChanged: url=%s, started=%s', url, started );
		this.set({
			showVideo: started,
			videoURL: url
		});
	}

	function onClosedCaption( text ) {
		let video = this.nodes.tvvideo;
		assert(video);
		log.silly( 'TvView', 'onClosedCaption: text=%s', text );

		if (!video.paused) {
			var textTrack = video.textTracks[0];
			if (!textTrack) {
				textTrack = video.addTextTrack('captions', 'Closed Caption', 'es-AR');
				textTrack.mode = 'showing';
			}
			else if (textTrack.cues.length > 0) {
				for (var i=0; i<textTrack.cues.length; i++) {
					textTrack.removeCue(textTrack.cues[i]);
				}
			}
			var t = video.currentTime;
			textTrack.addCue(new VTTCue(t, t + 5.0, text));
		}
	}

	function onKeyDown( event ) {
		let vk = event.original.which;
		log.verbose( 'TvView', 'OnKeyDown: vk=%d', vk );

		let handled = true;
		switch (vk) {
		case tacKeyboardLayout.VK_CHANNEL_UP:
			this.model.channelUp();
			break;
		case tacKeyboardLayout.VK_CHANNEL_DOWN:
			this.model.channelDown();
			break;
		case tacKeyboardLayout.VK_NEXT_FAVORITE_CHANNEL:
			this.model.nextFavorite();
			break;
		case tacKeyboardLayout.VK_LAST:
			this.model.previous();
			break;
		case tacKeyboardLayout.VK_INFO:
			this.findComponent('InfoBox').toggle(false);
			break;
		case tacKeyboardLayout.VK_SUBTITLE:
			apiCall( tvd.player.nextSubtitle );
			break;
		case tacKeyboardLayout.VK_TRACK_NEXT:
			apiCall( tvd.player.nextAudio );
			break;
		case tacKeyboardLayout.VK_HELP:
			this.findComponent('TechnicalDetails').toggle();
			break;
		default:
			handled = false;
		}

		return !handled;
	}

	let prot = {
		template: res.tpl('tvview'),
		components: ui.getComponents( InfoBox, TechnicalDetails )
	};

	prot.oninit = function() {
		log.verbose( 'TvView', 'oninit' );

		this.model = new TvModel(this);

		tvd.on('playerMediaChanged', onTvMediaChanged.bind(this) );
		tvd.on('closedCaption', onClosedCaption.bind(this) );

		this.on( 'keyDownHandler', onKeyDown );
	};

	prot.onrender = function() {
		log.verbose( 'TvView', 'onrender' );
		let chInfo = this.get('viewParam');
		if (chInfo) {
			log.info( 'TvView', 'On change channel: chID=%s', chInfo.id );
			this.model.changeChannel( chInfo.id );
		}

		{
			let section = this.nodes.tvview_container;
			section.focus();
			section.addEventListener( 'blur', () => section.focus() );
		}
	};

	prot.onunrender = function() {
		log.verbose( 'TvView', 'onunrender' );
		this.model.changeChannel(-1);
	};

	return {
		proto: prot
	};
}

