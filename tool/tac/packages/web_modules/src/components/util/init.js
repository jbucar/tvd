/* globals UI, Model, Log, main */
/* exported entry_point */
function entry_point( scope, templates, styles ) {
	//	Setup log
	scope.log = new Log();
	log.info( 'ui', 'init' );

	//	Setup UI
	scope.ui = new UI(templates,styles);

	//	Setup models
	scope.model = new Model();

	//	Start when all loaded
	scope.onload = main;

	TacOnExit( function(cb) {
		scope.ui.fin()
			.then(() => scope.model.fin())
			.finally(cb);
	});
}

