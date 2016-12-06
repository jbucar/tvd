/* globals InfoBox, FFRWDialog */
/* exported VideoPlayer */

function VideoPlayer(res) {

	const ACTIONS = {
		'play': play,
		'pause': pause,
		'forward': forward,
		'rewind': rewind,
		'stop': () => ui.hideView(),
		'subt': subtitle,
	};

	function ctrlInfoBox( ractive, cmd, focusBtn ) {
		if (cmd === 'toggle' || cmd === 'show') {
			let info = {
				name: ractive.videoInfo.title,
				logo: ractive.videoInfo.logo,
				video: ractive.nodes.Video,
			};

			if (cmd === 'toggle') {
				ractive.infoBox.toggle( info );
			}
			else {
				ractive.infoBox.show( info, false, focusBtn );
			}
		}
		else {
			ractive.infoBox.hide();
		}
	}

	function resetPlay( ractive, video ) {
		assert(ractive);
		assert(video);
		video.playbackRate = 1;
		clearInterval(ractive.seekInterval);
		ractive.seekInterval = undefined;
		ractive.sRewind = -1;
		ractive.sForward = 1;
	}

	function pause(ractive,video) {
		ctrlInfoBox(ractive, 'show');
		resetPlay(ractive,video);
		video.pause();
	}

	function play(ractive, video) {
		ctrlInfoBox(ractive, 'hide');
		ractive.ffrwDialog.hide();
		resetPlay(ractive,video);
		video.play();
	}

	function rewind(ractive, video, fromInfoBox) {
		seek(ractive, video, 'sRewind', play, t => t > 0.0, fromInfoBox);
	}

	function forward(ractive, video, fromInfoBox) {
		if (video.seekable.length > 0) {
			seek(ractive, video, 'sForward', resetPlay, t => t < video.seekable.end(0), fromInfoBox);
		} else {
			let playRate = video.playbackRate * 2;
			resetPlay(ractive, video);
			video.playbackRate = playRate;
			video.play();
			ractive.ffrwDialog.show(playRate, !fromInfoBox);
		}
	}

	function seek(ractive, video, speed, action, condition, fromInfoBox) {
		let newSpeed = ractive[speed] * 2;
		resetPlay(ractive, video);
		ractive[speed] = newSpeed;
		video.pause();
		ractive.ffrwDialog.show(newSpeed, !fromInfoBox);
		ractive.seekInterval = setInterval( function() {
			if (condition(video.currentTime)) {
				video.currentTime += 0.250 * newSpeed;
			} else {
				action(ractive, video);
			}
		}, 250 );
	}

	function subtitle(/*ractive, video*/) {
		log.warn( 'VideoPlayer', 'TODO: implement next subtitle');
		// TODO:
	}

	function onKeyDown(vk, ractive, video) {
		log.verbose( 'VideoPlayer', 'onKeyDown: vk=%d', vk );

		let handled = true;
		switch (vk) {
			case tacKeyboardLayout.VK_REWIND:
			case tacKeyboardLayout.VK_LEFT:
				if (ractive.sForward > 1) {
					play(ractive, video);
				} else {
					rewind(ractive, video, false);
				}
				break;
			case tacKeyboardLayout.VK_FAST_FWD:
			case tacKeyboardLayout.VK_RIGHT:
				if (ractive.sRewind < -1) {
					play(ractive, video);
				} else {
					forward(ractive, video, false);
				}
				break;
			case tacKeyboardLayout.VK_SUBTITLE:
				subtitle(ractive, video);
				break;
			case tacKeyboardLayout.VK_PAUSE:
			case tacKeyboardLayout.VK_ENTER:
				pause(ractive, video);
				break;
			case tacKeyboardLayout.VK_PLAY:
				play(ractive, video);
				break;
			case tacKeyboardLayout.VK_INFO:
				ctrlInfoBox(ractive, 'toggle');
				break;
			default:
				handled = false;
		}

		return !handled;
	}

	let prot = {
		template: res.tpl('videoplayer'),
		css: res.style('videoplayer'),
		components: ui.getComponents( InfoBox, FFRWDialog )
	};

	prot.focus = function() {
		log.verbose( 'VideoPlayer', 'focus' );
		this.nodes.VideoPlayer.focus();
	};

	prot.oninit = function() {
		log.verbose( 'VideoPlayer', 'oninit' );
		this.on({
			keyDownHandler: event => onKeyDown(event.original.which, this, this.nodes.Video),
			infoBtnHandler: cmd => ACTIONS[cmd](this, this.nodes.Video, true),
		});
	};

	prot.onrender = function() {
		log.verbose( 'VideoPlayer', 'onrender' );

		this.videoInfo = this.get('viewParam');
		assert(this.videoInfo);

		log.info( 'VideoPlayer', 'Play video: videoInfo=%j', this.videoInfo );
		this.set({
			'videoURL': this.videoInfo.src.uri,
			'subtitles': this.videoInfo.subtitles
		});
	};

	prot.oncomplete = function() {
		log.verbose( 'VideoPlayer', 'oncomplete' );

		let videoElem = this.nodes.Video;
		assert(videoElem,'Video element not found');
		resetPlay(this, videoElem);
		videoElem.currentTime = this.videoInfo.currentTime || 0.0;
		videoElem.onended = () => {
			ctrlInfoBox(this, 'show', 'stop');
			this.ffrwDialog.hide();
		};

		this.infoBox = this.findComponent('InfoBox');
		assert(this.infoBox);

		this.ffrwDialog = this.findComponent('FFRWDialog');
		assert(this.ffrwDialog);

		this.focus();
	};

	prot.onunrender = function() {
		log.verbose( 'VideoPlayer', 'onunrender' );

		this.videoInfo.currentTime = this.nodes.Video.currentTime;
		ctrlInfoBox(this, 'hide');
	};

	return {
		proto: prot
	};
}

