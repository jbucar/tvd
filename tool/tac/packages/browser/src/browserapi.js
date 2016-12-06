'use strict';

module.exports = function( browserSrv ) {
	/**
	 * Events
	 *   'browserClosed'
	 *     Se emite cuando un browser fue cerrado exitosamente.
	 *     @param id El id del browser cerrado
	 *     @param error Error en caso de que el browser se cierre con una condicion de error
	 *
	 *   'browserLaunched'
	 *     Se emite cuando un browser fue lanzado exitosamente.
	 *     @param id El id del browser lanzado
	 *
	 *   'ready'
	 *     Se emite cuando el browser service cambia de estado.
	 *     @param isReady true si se pueden utilizar la api, false en caso contrario.
	 */
	this.on = function( name, callback ) {
		browserSrv.on(name, callback);
	};

	this.once = function( name, callback ) {
		browserSrv.once(name, callback);
	};

	this.removeListener = function( name, callback ) {
		browserSrv.removeListener(name, callback);
	};

	/**
	 * Returna si el browser esta listo
	 */
	this.isReady = function() {
		return browserSrv.isReady();
	};

	/**
	 * Lanza una nueva instancia de browser cargando la url indicada, ejecutando el codigo javascript
	 * al crearse el contexto y previamente a ejecutar cualquier codigo javascript referenciado por la url.
	 * Una vez que la url es efectivamente cargada, se notifica mediante el callback.
	 * @param url la página html a cargar.
	 * @param options objeto con datos extras
	 *      {
	 *        bounds: ubicación y tamaño del browser {x:<int>,y:<int>,w:<int>,h:<int>}
	 *        apis: Arreglo de las apis que seran exportadas para el browser (api={name:<string>, id:<string>, exports:[<string>]}).
	 *        bgTransparent: <bool> indica si el browser debe tener un fondo transparente u opaco
	 *        focus: <bool> indica si el browser debe capturar el foco al lanzarse
	 *        visible: <bool> indica si el browser sera visible al lanzarse
	 *        zIndex: <int> indica la capa de visibilidad en la que se lanzara el browser
	 *        plugins: [<string>] Nombre de los plugins permitidos
	 *        enableMouse: <bool> indica si se debe habilitar el mouse
	 *        enableGeolocation: <bool> indica si se debe habilitar la geolocalizacion
	 *        hwCodecsOnly: <bool> indica si se permite la reproduccion de codecs no soportados por el HW
	 *        extraUA: <string> Extra user agent string
	 *      }
	 * @param onEvent funcion que sera llamada cuando se dispara algun evento
	 * @return el id correspondiente al nuevo browser creado.
	 */
	this.launchBrowser = function( url, options, onEvent ) {
		log.info('BrowserService', '[BrowserApi] Launch browser: url=%s', url);
		return browserSrv.launchBrowser(url, options, onEvent);
	};

	/**
	 * Cierra una instancia de browser creada previamente mediante 'launchBrowser'
	 * @param id el id retornado por la llamada a  'launchBrowser'
	 * @return true si se cerro el browser correctamente, false en caso contrario.
	 */
	this.closeBrowser = function( id ) {
		log.info('BrowserService', '[BrowserApi] Close browser: id=%d', id );
		return browserSrv.closeBrowser(id);
	};

	/**
	 * Muestra u oculta una instancia de browser
	 * @param id el id retornado por la llamada a 'launchBrowser'
	 * @param show<bool> indica si se debe mostrar u ocultar la ventana correspondiente del browser
	 * @return true si se realizo la operacion correctamente, false en caso contrario
	 */
	this.showBrowser = function( id, show, focus ) {
		return browserSrv.showBrowser(id, show, focus);
	};

	/**
	 * Retorna informacion del estado acutal del servicio (ver system.sendError())
	 */
	this.dump = function() {
		return browserSrv.dumpState();
	};
};
