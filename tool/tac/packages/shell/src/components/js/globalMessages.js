
/* Messages */

var msg = {};
msg.popup = {};
msg.tooltip = {};
msg.about = {};
msg.channels = {};
msg.apps = {};
msg.network = {};
msg.restore = {};
msg.system = {};
msg.wizard = {};
msg.tvlayer = {};
msg.guide = {};

msg.popup.err = {};
msg.popup.err.wrongPasswd = 'Contraseña incorrecta.';
msg.popup.err.connectionFail = 'Fallo de conexión';
msg.popup.warn = {};
msg.popup.warn.removePasswd = 'Al eliminar la contraseña perderá sus restricciones. ¿Desea continuar?';
msg.popup.warn.scanChannels = 'Los canales y sus respectivas configuraciones se perderán. ¿Desea continuar?';
msg.popup.warn.refactoryReset = 'Se eliminarán todas las configuraciones realizadas. ¿Desea continuar?';
msg.popup.warn.installFirmware = 'Esta instalación eliminará todas las configuraciones actuales. ¿Desea continuar?';

msg.popup.warn.resetCongigService = '¿Está seguro que desea borrar los datos del servicio?';
msg.popup.warn.uninstallService = '¿Está seguro que desea desinstalar el servicio?';
msg.popup.warn.uninstallApp = '¿Está seguro que desea desinstalar esta aplicación?';
msg.popup.warn.blockWithoutPass = 'Para bloquear necesita crear una contraseña. ¿Desea crearla?';
msg.popup.warn.wizard = 'Los canales y sus respectivas configuraciones se perderán. ¿Desea continuar?';

msg.tooltip.home = msg.tooltip.config = 'Presione "OK" para ingresar a la categoría \
elegida o puede continuar navegando a derecha o izquierda con las flechas.';

msg.tooltip.menu = 'Presione "OK" para regresar al menú';

// Tooltip Images

msg.tooltip.images = {};
msg.tooltip.images.emptySection = 'Conecte un dispositivo para visualizar imágenes o puede continuar navegando a derecha e izquierda con las flechas';


/* About System */

msg.about.title = 'VERSIÓN ACTUAL DE TAC TV';
msg.about.description = 'La Terminal de Acceso al Conocimiento, es el último avance tecnológico de la Televisión Digital Abierta, \
la politica pública que impulsa el Ministerio de Planificación Federal, Infraestructura y Servicios para garantizar el acceso universal a los mejores contenidos audiovisuales en Alta Definición. \
Con la TAC TV, además de recibir la señal gratuita de la Television Digital Abierta, podes acceder a Internet, navegar por la web, ingresar a redes sociales, ver videos, series y películas a demanda,\
descargar aplicaciones, mirar fotos, escuchar música, y muchas más.';
msg.about.infoNotFound = 'No se ha podido recuperar la información de su TAC TV';

/* Internet */

msg.network.description = 'Seleccione una red WiFi de la lista de redes disponibles y presione OK. \
Para actualizar la lista presione RENOVAR LISTA. \
Si no posee una conexión WiFi conecte su TAC TV mediante un cable de red.';

msg.network.connected =  'Para conectarse a otra red, selecciónela y presione OK. \
Si desea desconectarse presione el botón DESCONECTAR RED.';


/* Restore */

msg.restore = 'Por medio de la opción RESTABLECER usted podrá volver el equipo a su configuración de fábrica.';


/* System */

msg.system.update = "Para poder disfrutar de su TAC TV al máximo es necesario mantenerla actualizada. \
Puede buscar las actualizaciones disponibles manualmente, mediante el botón BUSCAR, que está localizado a la izquierda de la pantalla. \
Recuerde que debe estar conectado a internet para poder actualizar su TAC TV.";

msg.system.download = 'Para poder comenzar a disfrutar de la nueva actualización debe descargarla presionando el botón DESCARGAR.';

msg.system.install = 'Presione INSTALAR para instalar la actualización del sistema.\nLa TAC TV se reiniciará de manera automática.';


/* Block_program */

msg.channels.contentBlocked = 'Este programa tiene un contenido inapropiado.';
msg.channels.channelBlocked = 'Este canal ha sido bloqueado.';

msg.tvlayer.wrongPasswd = 'CONTRASEÑA INVÁLIDA';
msg.tvlayer.unlockProgram = 'DESBLOQUEAR PROGRAMA';
msg.tvlayer.unlockChannel = 'DESBLOQUEAR CANAL';
msg.tvlayer.programBlocked = 'PROGRAMA BLOQUEADO';
msg.tvlayer.channelBlocked = 'CANAL BLOQUEADO';

/* Wizard */
msg.wizard = 'Este asistente le ayudará a configurar la TAC. Para iniciarlo presione INICIAR ASISTENTE.';

/* Guide */
msg.guide.week = {};
msg.guide.week.day0 = 'DOMINGO';
msg.guide.week.day1 = 'LUNES';
msg.guide.week.day2 = 'MARTES';
msg.guide.week.day3 = 'MIÉRCOLES';
msg.guide.week.day4 = 'JUEVES';
msg.guide.week.day5 = 'VIERNES';
msg.guide.week.day6 = 'SÁBADO';


/* Contextual Help */
msg.help = {};
msg.help.error = 'No existe la ayuda para este contexto.';

msg.help.navbar = {};
msg.help.menu = {};
msg.help.submenu = {};

msg.help.carousel = {};
msg.help.details = {};
msg.help.scan = {};
msg.help.grid = {};


msg.help.navbar.home = 'Bienvenido a la TAC.';
msg.help.navbar.ch = 'En esta seccion podra buscar canales nuevos, ver todos los canales encontrados, agregarlos como favorito y bloquearlos. También podrá hacer uso de la Guia, la cual contiene todos canales y sus respectivos programas a lo largo del tiempo.';
msg.help.navbar.img = 'En esta sección podrá ver las imágenes que se encuentren en sus dispositivos conectados o en los diferentes proveedores de imágenes que tenga configurados.';
msg.help.navbar.apps = 'En esta sección podrá hacer uso de las aplicaciones presentes en la TAC y  también podrá descargar nuevas aplicaciones por medio de la Tienda TAC.';
msg.help.navbar.config = 'En esta sección podrá configurar las diferentes prestaciones de la TAC, entre las que se encuentra: configuracion de Internet, de canales y sistema.';

msg.help.menu.backToNavbar = 'El elemento con nombre “MENU” permite volver al menú inicial de la TAC.';
msg.help.menu.apps = 'En esta sección podrá hacer uso de las aplicaciones presentes en la TAC y  también podrá descargar nuevas aplicaciones por medio de la Tienda TAC.';
msg.help.menu.ch = msg.help.navbar.ch;
msg.help.menu.img = msg.help.navbar.img;
msg.help.menu.config = 'En esta sección podrá configurar las diferentes prestaciones de la TAC, entre las que se encuentra:  configuracion de Internet, de canales, sistema y servicios.';

msg.help.submenu.apps = 'En esta sección podrá recorrer todas las aplicaciones encontradas y verlas según favoritos y categorías presentes.';
msg.help.submenu.ch = 'En esta seccion podra recorrer todos los canales encontrados y verlos según favoritos o bloqueados.';
msg.help.submenu.img = msg.help.navbar.img;

msg.help.carousel.apps = 'En esta sección podrá recorrer todas las aplicaciones presentes en la TAC y ver detalles de las mismas: nombre de la aplicación, categoría y descripción.';
msg.help.carousel.ch = 'En esta sección podrá recorrer todos los canales encontrados y ver detalles de los mismos: nombre del canal, nombre del programa, horarios de inicio y fin, descripción, entre otros.';
msg.help.grid.img = 'En esta sección podrá ver las imágenes encontradas en la fuente seleccionada, recorrerlas e ingresar para verlas en pantalla completa.';
msg.help.imgViewer = 'En este visualizador podrá ver las imágenes automática o manualmente. También podrá realizar acciones como rotar y ver información de las imágenes.';

msg.help.details.apps = 'En esta sección podrá ver el detalle de una aplicación. Encontrará información como nombre de la aplicacion, descripción y categoría. Podrá agregar la aplicacion como favorito o eliminarla.';
msg.help.details.ch = 'En esta sección podrá ver el detalle de un canal y el programa que está en ese momento. Encontrará información como nombre del canal, nombre del programa, horarios de inicio y fin, descripción, entre otros.';

msg.help.scan.ch = 'En esta sección podrá buscar nuevos canales.';
msg.help.scan.config = 'En esta sección podrá buscar nuevos canales.';

msg.help.marketdetail = {};
msg.help.guide = {};
msg.help.tda = {};
msg.help.network = {};
msg.help.parental = {};


msg.help.marketdetail.apps = 'En esta sección podrá ver el detalle de una aplicación. Encontrará información como nombre de la aplicacion, descripción y categoría.';
msg.help.guide.ch = 'En esta seccion podra recorrer todos los canales con sus respectivos programas a lo largo del tiempo. Se encuentra información como títulos, nombres, descripciones y horarios.';

msg.help.config = { system: {}, tda: { parental: {}}, network: {}, services: {}, debug: {}};

msg.help.config.system.submenu = 'En esta sección podrá ver información sobre la TAC, actualizar a nuevas versiones y restablecer a valores de fábrica.';
msg.help.config.system.about = 'En esta sección podrá ver información sobre la TAC.';
msg.help.config.system.update = 'En esta sección podrá actualizar la TAC a su última versión.';
msg.help.config.system.restore = 'En esta sección podrá restablecer la TAC a su configuración inicial.';
msg.help.config.system.wizard = 'El asistente de configuración le permite volver a iniciar la configuración inicial de la TAC.';
msg.help.config.tda.submenu = 'En esta sección podrá configurar las diferentes prestaciones de los TDA. Buscar canales y configurar las restricciones de los canales y programas.';
msg.help.config.tda.ginga = 'En esta seccion podra habilitar y deshabilitar Ginga.ar. Este módulo es el encargado de proveer interactividad a los programas de televisión. Cuando un programa presente interactividad, y este módulo esté habilitado, se le notificará automáticamente y podrá hacer uso de la misma.';
msg.help.config.tda.parental.new = 'A partir de este momento, para iniciar o cerrar sesión, deberá hacer uso del botón amarillo del control remoto. Esta acción estará disponible en cualquier sección de la Tac. Cuando su sesión este iniciada, visualizará en la parte superior de la pantalla el icono de un candado.';
msg.help.config.tda.parental.passwd = 'En esta sección podrá configurar el control parental de la TAC. El control parental es una funcionalidad que le permitirá bloquear canales según el tipo de contenido que tenga.';
msg.help.config.tda.parental.age = 'En esta sección podrá configurar el control parental de la TAC. Si configura la opción “Por edad” automáticamente se bloquearán los programas televisivos correspondientes para esa edad.';
msg.help.config.tda.parental.content = 'En esta sección podrá configurar el control parental de la TAC. Si configura la opción “Por contenido” automáticamente se bloqueará los programas televisivos correspondientes con esa categoría.';
msg.help.config.network.submenu = 'En esta sección podrá ver información del estado actual de la red y conectarse a una red wifi.';
msg.help.config.network.status = 'En esta sección podrá ver información del estado actual de la red.';
msg.help.config.network.connect = 'En esta sección podrá buscar y conectarse a redes inalámbricas.';
msg.help.config.services.system = 'En esta sección podrá configurar los diferentes servicios presententes en la TAC.';
msg.help.config.services.details = 'En esta sección podrá configurar un servicio de la TAC. Podrá habilitar o deshabilitar, borrar datos, etc.';

msg.geolocal = 'Habilitando el servicio de geolocalización la TAC podrá acceder a su posición geográfica. Esto permite que las aplicaciones que utilicen este servicio puedan brindarte una mejor experiencia.';

/* Ginga */
msg.ginga = 'En esta sección podrá habilitar y deshabilitar Ginga.ar. Este módulo es el encargado de proveer interactividad a los programas de televisión. \
Cuando un programa presente interactividad, y este módulo esté habilitado, se le notificará automáticamente y podrá hacer uso de la misma. Mientras se encuentre deshabilitado \
no recibirá notificaciones.';

// I18 - Internazionalization

var i18 = (function() {
	var messages = {
		'spanish': 'español'
	}

	return function(txt) {
		var str = messages[txt.toLowerCase()];
		if (str) {
			return str;
		} else {
			log.warn('Lenguaje de audio fuera de norma ', txt);
			return txt;
		}
	}
})();
