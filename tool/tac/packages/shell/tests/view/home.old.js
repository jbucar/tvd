var assert = chai.assert;
var eLeft = {down: newKeyboardEvent('keydown',37), up: newKeyboardEvent('keyup',37)};
var eUp = {down: newKeyboardEvent('keydown',38), up: newKeyboardEvent('keyup',38)};
var eRight = {down: newKeyboardEvent('keydown',39), up: newKeyboardEvent('keyup',39)};
var eDown = {down: newKeyboardEvent('keydown',40), up: newKeyboardEvent('keyup',40)};
var eEnter = {down: newKeyboardEvent('keydown',13), up: newKeyboardEvent('keyup',13)};

function newKeyboardEvent(type, keyCode) {
	var event = document.createEvent("KeyboardEvent");
	Object.defineProperty(event, 'keyCode', {
			get : function() {
				return this.keyCodeVal;
			}
	});

	Object.defineProperty(event, 'which', {
		returnget : function() {
			return this.keyCodeVal;
		}
	});

	// event.initKeyboardEvent( type, true, true, document.defaultView, false, false, false, false );
	event.initKeyboardEvent( type );
	event.keyCodeVal = keyCode;
	return event;
};

function dispatchKeyEvent(target, event) {
	target.dispatchEvent(event.down);
	target.dispatchEvent(event.up);
}



/* Chequear que los componentes existan */

describe("Test de integridad", function () {
	it("Vista superior", function () {
		assert.isDefined( app.$.upperComponent, "Vista superior" )
	});
	it("Vista superior(Botones)", function () {
		assert.isDefined( app.$.userBar.$.controlParental, "Btn Control Parental" )
		assert.isDefined( app.$.userBar.$.time, "Btn Hora" )
		assert.isDefined( app.$.userBar.$.logo, "Btn Logo" )
	});
	it("Vista del medio", function () {
		assert.isDefined( app.$.middleComponent, "Vista del medio" )
	});
	it("Vista inferior", function () {
		assert.isDefined( app.$.bottomComponent, "Vista Inferior" )
	});
	it("Vista inferior(Botones)", function () {
		assert.isDefined( app.$.bottomComponent.$.home, "Btn Inicio" )
		assert.isDefined( app.$.bottomComponent.$.tv, "Btn Mirar Tv" )
		assert.isDefined( app.$.bottomComponent.$.guide, "Btn Programa" )
		assert.isDefined( app.$.bottomComponent.$.apps, "Btn Apps" )
		assert.isDefined( app.$.bottomComponent.$.about, "Btn Info" )
		assert.isDefined( app.$.bottomComponent.$.config, "Btn Config" )
	});
});

/* Chequear la navegacion con los botones del control remoto sobre el componente btnInicio */
describe("Should check go to btnInicio", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Initial (btnInicio)", function() {
		assert.equal( app.$.bottomComponent.$.home, enyo.Spotlight.getCurrent(), "Initial position" )
	});	
});

describe("Should check key navigation component (Btn Inicio)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Press Ok (btnInicio)", function() {
		dispatchKeyEvent(document,eEnter);
		assert.equal( app.$.bottomComponent.$.home, enyo.Spotlight.getCurrent(), "Press Ok" )
	});
	it("Press Down (btnInicio)", function() {
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.bottomComponent.$.home, enyo.Spotlight.getCurrent(), "Press Down" )
	});
	it("Press Left (btnInicio)", function() {
		dispatchKeyEvent(document,eLeft);
		assert.notEqual( app.$.bottomComponent.$.home, enyo.Spotlight.getCurrent(), "Press Left" )
	});
	it("Press Right (btnInicio)", function() {
		dispatchKeyEvent(document,eRight);
		assert.notEqual( app.$.bottomComponent.$.home, enyo.Spotlight.getCurrent(), "Press Right" )
	});
	it("Press Up (btnInicio)", function() {
		dispatchKeyEvent(document,eUp);
		assert.equal( app.$.bottomComponent.$.home, enyo.Spotlight.getCurrent(), "Press Up" )
	});
});

describe("Should check go to btnMirarTv", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Initial", function() {
		assert.notEqual( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Initial position" )
	});
	it("Selection", function() {
		dispatchKeyEvent(document,eRight);
		assert.equal( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Selection" )
	});
});

describe("Should check key navigation component (btnMirarTv)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.tv);
	});
	it("Press Ok", function() {
		dispatchKeyEvent(document,eEnter);
		assert.equal( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Press Ok" )
	});
	it("Press Down", function() {
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Press Down" )
	});
	it("Press Left", function() {
		dispatchKeyEvent(document,eLeft);
		assert.notEqual( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Press left" )
	});
	it("Press Right", function() {
		dispatchKeyEvent(document,eRight);
		assert.notEqual( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Press Right" )
	});
	it("Press Up", function() {
		dispatchKeyEvent(document,eUp);
		assert.notEqual( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Press Up" )
	});
});


describe("Should check go to btnMirarTv", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Initial", function() {
		assert.notEqual( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Initial position" )
	});
	it("Selection", function() {
		dispatchKeyEvent(document,eRight);
		assert.equal( app.$.bottomComponent.$.tv, enyo.Spotlight.getCurrent(), "Selection" )
	});
});

describe("Should check key navigation component (btnProgramas)", function() {

	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.guide);
	});
	it("Press Ok", function() {
		dispatchKeyEvent(document,eEnter);
		assert.equal( app.$.bottomComponent.$.guide, enyo.Spotlight.getCurrent(), "Press Ok" )
	});
	it("Press Down", function() {
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.bottomComponent.$.guide, enyo.Spotlight.getCurrent(), "Press Down" )
	});
	it("Press Left", function() {
		dispatchKeyEvent(document,eLeft);
		assert.notEqual( app.$.bottomComponent.$.guide, enyo.Spotlight.getCurrent(), "Press left" )
	});
	it("Press Right", function() {
		dispatchKeyEvent(document,eRight);
		assert.notEqual( app.$.bottomComponent.$.guide, enyo.Spotlight.getCurrent(), "Press Right" )
	});
	// it("Press Up", function() {
	// 	dispatchKeyEvent(document,eUp);
	// 	assert.equal( app.$.bottomComponent.$.guide, enyo.Spotlight.getCurrent(), "Press Up" )
	// });
});


describe("Should check go to btnApps", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Initial", function() {
		assert.notEqual( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Initial position" )
	});
	it("Selection", function() {
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		assert.equal( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Selection" )
	});
});

describe("Should check key navigation component (btnApps)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.apps);
	});
	it("Press Ok", function() {
		dispatchKeyEvent(document,eEnter);
		assert.equal( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Press Ok" )
	});
	it("Press Down", function() {
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Press Down" )
	});
	it("Press Left", function() {
		dispatchKeyEvent(document,eLeft);
		assert.notEqual( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Press left" )
	});
	it("Press Right", function() {
		dispatchKeyEvent(document,eRight);
		assert.notEqual( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Press Right" )
	});
	it("Press Up", function() {
		dispatchKeyEvent(document,eUp);
		assert.notEqual( app.$.bottomComponent.$.apps, enyo.Spotlight.getCurrent(), "Press Up" )
	});
});


describe("Should check go to btnInfo", function(){
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Initial", function() {
		assert.notEqual( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Initial position" )
	});
	it("Selection", function() {
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		assert.equal( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Selection" )
	});
});

describe("Should check key navigation component (btnInfo)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.about);
	});
	it("Press Ok", function() {
		dispatchKeyEvent(document,eEnter);
		assert.equal( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Press Ok" )
	});
	it("Press Down", function() {
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Press Down" )
	});
	it("Press Left", function() {
		dispatchKeyEvent(document,eLeft);
		assert.notEqual( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Press left" )
	});
	it("Press Right", function() {
		dispatchKeyEvent(document,eRight);
		assert.notEqual( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Press Right" )
	});
	it("Press Up", function() {
		dispatchKeyEvent(document,eUp);
		assert.notEqual( app.$.bottomComponent.$.about, enyo.Spotlight.getCurrent(), "Press Up" )
	});
});


describe("Should check go to btnConfig", function(){
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.home);
	});
	it("Initial", function() {
		assert.notEqual( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Initial position" )
	});
	it("Selection", function() {
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		dispatchKeyEvent(document,eRight);
		assert.equal( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Selection" )
	});
});

describe("Should check key navigation component (btnConfig)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.config);
	});
	it("Press Ok", function() {
		dispatchKeyEvent(document,eEnter);
		assert.equal( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Press Ok" )
	});
	it("Press Down", function() {
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Press Down" )
	});
	it("Press Left", function() {
		dispatchKeyEvent(document,eLeft);
		assert.notEqual( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Press left" )
	});
	it("Press Right", function() {
		dispatchKeyEvent(document,eRight);
		assert.notEqual( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Press Right" )
	});
	it("Press Up", function() {
		dispatchKeyEvent(document,eUp);
		assert.notEqual( app.$.bottomComponent.$.config, enyo.Spotlight.getCurrent(), "Press Up" )
	});
});

/* Agregar test de  navegación en las vistas de MirarTv, Apps, Info, Config */

describe("Should check key navigation component (MirarTV)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.tv);
	});
	it("Select (Ver listado de todos los canales)", function() {
		dispatchKeyEvent(document,eUp);
		assert.equal( app.$.tvView.$.viewChannels, enyo.Spotlight.getCurrent(), "Press Selection1" )
	});
	it("Select (Ver listado de los canales favoritos)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.tvView.$.favorite, enyo.Spotlight.getCurrent(), "Press Selection2" )
	});
	it("Select (Ver listado de los canales bloqueados)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.tvView.$.blocked, enyo.Spotlight.getCurrent(), "Press Selection3" )
	});
	it("Select (Ver listado de los canales de deportes)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.tvView.$.sport, enyo.Spotlight.getCurrent(), "Press Selection4" )
	});
	it("Select (Ver listado de los canales de accion)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.tvView.$.accion, enyo.Spotlight.getCurrent(), "Press Selection5" )
	});

	it("Select (Ver listado de los canales de comedia)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.tvView.$.comedy, enyo.Spotlight.getCurrent(), "Press Selection6" )
	});

	it("Select (Ver listado de los canales de noticia)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.tvView.$.news, enyo.Spotlight.getCurrent(), "Press Selection7" )
	});

});


describe("Should check key navigation component (Aplicaciones)", function() {
	beforeEach(function() {
		// runs before all tests in this block
		enyo.Spotlight.spot(app.$.bottomComponent.$.apps);
	});
	it("Select (Recomendaciones)", function() {
		dispatchKeyEvent(document,eUp);
		assert.equal( app.$.applicationsView.$.initialView.$.allApplications, enyo.Spotlight.getCurrent(), "Selection todas las aplicaciones" )
	});
	it("Select (Favoritas)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.applicationsView.$.initialView.$.favorites, enyo.Spotlight.getCurrent(), "Selection favoritas" )
	});
	it("Select (Categoria1)", function() {
		dispatchKeyEvent(document,eUp);
		dispatchKeyEvent(document,eDown);
		dispatchKeyEvent(document,eDown);
		assert.equal( app.$.applicationsView.$.initialView.$.category1, enyo.Spotlight.getCurrent(), "Selection Categoria1" )
	});
});

/* disabled */
// describe("Should check key navigation component (Aplicaciones/Recomendaciones)", function() {
// 	beforeEach(function() {
// 		// runs before all tests in this block
// 		enyo.Spotlight.spot(app.$.bottomComponent.$.apps);
// 	});
// 	afterEach(function() {
// 		// runs before all tests in this block
// 		enyo.Spotlight.spot(app.$.bottomComponent.$.apps);
// 	});
// 	it("Changing view (Aplicaciones/Recomendaciones)", function() {
// 		dispatchKeyEvent(document,eUp);
// 		dispatchKeyEvent(document,eEnter);
// 		assert.notEqual( app.$.applicationsView.$.recommendation, enyo.Spotlight.getCurrent(), "Showing Aplicaciones/Recomendaciones" )
// 	});
// 	it("showing correct view (Aplicaciones/Recomendaciones)", function() {
// 		dispatchKeyEvent(document,eUp);
// 		dispatchKeyEvent(document,eEnter);
// 		assert.equal( app.$.applicationsView.$.recommendation.showing, true , "Showing correct Aplicaciones/Recomendaciones view" )
// 	});
// });

/* disabled */
// describe("Should check key navigation component (Info/Acerca de)", function() {
// 	beforeEach(function() {
// 		// runs before all tests in this block
// 		enyo.Spotlight.spot(app.$.bottomComponent.$.about);
// 	});
// 	it("Select (Scroll text)", function() {
// 		dispatchKeyEvent(document,eUp);
// 		assert.equal( app.$.about, enyo.Spotlight.getCurrent(), "Selection Info" )
// 	});
// });

