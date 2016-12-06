/* exported InfoBox */
function InfoBox(res) {

	const buttons = ['rewind','pause','stop','forward','subt'];

	function formatTime( sec_num ) {
		var hours   = Math.floor(sec_num / 3600);
		var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
		var seconds = parseInt(sec_num - (hours * 3600) - (minutes * 60));

		if (hours   < 10) {hours   = "0"+hours;}
		if (minutes < 10) {minutes = "0"+minutes;}
		if (seconds < 10) {seconds = "0"+seconds;}
		return hours+':'+minutes+':'+seconds;
	}

	function getOptions( ractive, video ) {
		let now = currentDateTime();
		let percent = (video.currentTime * 100) / video.duration;
		return {
			timeNow: now.time,
			beginTime: formatTime(video.currentTime),
			duration: formatTime(video.duration),
			endTime: res.txt('end_time') + formatDateTime(new Date(Date.now()+((video.duration-video.currentTime)*1000))).time,
			progressPercent: percent
		};
	}

	function focusButton(ractive, cmd, inc) {
		let btnName = cmd === 'play' ? 'pause' : cmd;
		let next = (buttons.indexOf(btnName) + inc) % buttons.length;
		ractive.nodes.InfoBoxButtons.children[next].focus();
	}

	function onKeyDown(event, cmd) {
		log.info( 'Infobox', 'onKeyDown: vk=%d', event.original.which );

		let handled = true;
		switch(event.original.which) {
			case tacKeyboardLayout.VK_INFO:
				this.parent.fire('infoBtnHandler', 'play');
				break;
			case tacKeyboardLayout.VK_ENTER:
				this.parent.fire('infoBtnHandler', cmd);
				break;
			case tacKeyboardLayout.VK_RIGHT:
				focusButton(this, cmd, 1);
				break;
			case tacKeyboardLayout.VK_LEFT:
				focusButton(this, cmd, buttons.length-1);
				break;
			default:
				handled = false;
		}

		return !handled;
	}

	let prot = {
		template: res.tpl('infobox'),
		css: res.style('infobox'),
		isolated: true,
	};

	prot.onconstruct = function(opts) {
		log.info( 'Infobox', 'onconstruct: opts=%j', opts );

		opts.data.show = false;
		opts.data.clockImg = res.img('clock');
		opts.data.buttons = buttons.map(btn => ({name: btn, src: res.img(btn)}));
	};

	prot.oninit = function() {
		log.info( 'Infobox', 'oninit' );

		this.on('keyHandler', onKeyDown);
	};

	prot.hide = function() {
		log.verbose( 'InfoBox', 'hide' );
		this.set('show', false);
		clearInterval(this.refreshInterval);
		this.parent.focus();
	};

	prot.show = function( info, isPlaying, focusBtn ) {
		log.verbose( 'InfoBox', 'show' );

		let opts = getOptions( this, info.video );
		opts.show = true;
		opts.name = info.name.toUpperCase();
		opts.logo = info.logo;
		this.set( opts );

		clearInterval(this.refreshInterval);
		this.refreshInterval = setInterval( () => this.set( getOptions(this, info.video) ), 250 );

		let btnIdx = focusBtn !== undefined ? buttons.indexOf(focusBtn) : 1;
		this.nodes.InfoBoxButtons.children[btnIdx].focus();
		this.set({
			'buttons.1.src': res.img( isPlaying ? 'pause' : 'play' ),
			'buttons.1.name': isPlaying ? 'pause' : 'play',
		});
	};

	prot.toggle = function( info ) {
		if (this.get('show')) {
			this.hide();
		} else {
			this.show(info, true);
		}
	};

	return {
		proto: prot,
	};
}
