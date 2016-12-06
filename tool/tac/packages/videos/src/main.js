/* exported main */
/* globals SeriesModel, VideosMenu, MoviesModel */

//	main entry point
function main() {
	log.verbose( 'main', 'Begin' );
	model.init({
		Movies: new MoviesModel(),
		Series: new SeriesModel()
	});
	ui.init({
		main: VideosMenu,
		theme: {
			bgColor: "rgb(44,44,44)",
			fgColor: "rgb(65,65,65)",
			focusColor: "rgb(128,128,128)"
		}
	});
}
