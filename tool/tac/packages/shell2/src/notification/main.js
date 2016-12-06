/* exported main */
/* globals ui, Notification */

//	main entry point
function main() {
	log.verbose( 'main', 'Begin' );
	ui.init({main: Notification});
}
