/* exported main */
/* globals model, Session, ChannelModel, ui, TDAMenu */

//	main entry point
function main() {
	log.verbose( 'main', 'Begin' );
	model.init({
		channels: new ChannelModel(),
		session: new Session()
	});
	ui.init({
		main: TDAMenu,
		theme: {
			bgColor: "rgb(44,44,44)",
			fgColor: "rgb(68,68,68)",
			focusColor: "rgb(128,128,128)"
		}
	});
}
