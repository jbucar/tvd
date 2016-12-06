/* exported main */
/* globals model, ui, ShellMenu, ApplicationModel */

//	main entry point
function main() {
	log.verbose( 'main', 'Begin' );

	model.init({ apps: new ApplicationModel() });
	ui.init({
		main: ShellMenu,
		theme: {
			bgColor: "rgb(26,46,53)",
			fgColor: "rgb(47,78,89)",
			focusColor: "rgb(81,125,140)"
		}
	});
}
