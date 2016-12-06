/* globals ui, VirtualKeyboard, Volume, NotifyBox, Splash */
/* exported Notification */
function Notification(res) {

	let focused = 0;
	let visible = 0;

	let prot = {
		template: res.tpl('notification'),
		components: ui.getComponents( NotifyBox, VirtualKeyboard, Volume, Splash ),
	};

	prot.showLayer = function(show, needFocus) {
		let wasVisible = visible > 0;
		visible += show ? 1 : -1;
		let isVisible = visible > 0;

		let wasFocused = focused > 0;
		if (needFocus !== undefined) {
			focused += needFocus ? 1 : -1;
		}
		let isFocused = focused > 0;

		log.verbose( 'notification', 'showLayer: wasVisible=%s, isVisible=%s, wasFocused=%s, isFocused=%s', wasVisible, isVisible, wasFocused, isFocused );
		if ((wasVisible !== isVisible) || (wasFocused !== isFocused) ) {
			notification.emit('ShowSystemLayer', isVisible, isFocused );
		}
	};

	return {
		proto: prot
	};
}
