function mockSeriesMetaData() {
	return [{
		id: 'episode1',
		currentTime: 0,
		runtime: 40,
		"subtitles": [],
		"title": "Episode 6",
		"season": "01",
		"episode": "06",
		"summary": "Can't get enough 'Game of Thrones'? Join hosts Andy Greenwald and Chris Ryan for this post-'Game' show that delves inside Season 6, Episode 6, explores the complicated politics and history of 'Thrones', and offers absurd and not-so-absurd theories about future episodes.",
		"video": {
			codecs: {
				audio: {
					name: "aac",
					profile: "LC",
					channels: 2
				},
				video: {
					name: "h264",
					profile: "High",
					width: 1920,
					height: 800,
					fps: 23.976023976023978
				}
			},
			src: {
				type: 'file'
			},
			"size": 850
		}
	}, {
		id: 'episode2',
		currentTime: 0,
		runtime: 40,
		"subtitles": [],
		"title": "Episode 7",
		"season": "01",
		"episode": "07",
		"summary": "Otra cosa.",
		"video": {
			codecs: {
				audio: {
					name: "aac",
					profile: "LC",
					channels: 2
				},
				video: {
					name: "h264",
					profile: "High",
					width: 1920,
					height: 800,
					fps: 23.976023976023978
				}
			},
			src: {
				type: 'file'
			},
			"size": 850
		}
	}];
}

function mockSeries() {
	return [{
		id: 'xxx',
		title: 'The walking dead',
		summary: "'The Walking Dead' parte de uno de los cómics más seguidos de los últimos años y cuenta con destacados profesionales al frente de su puesta en imágenes: el cineasta Frank Darabont, tres veces nominado al Oscar y autor, entre otras, de las películas 'Cadena perpetua' y 'La Milla Verde'), es responsable del guión, la producción y la realización. Junto a él, completa el equipo de producción la veterana productora Gale Anne Hurd, vinculada a películas de gran presupuesto como 'Terminator', 'Alien', 'Armageddon', 'El increíble Hulk'.'The Walking Dead' está ambientada en un futuro apocalíptico con la Tierra devastada por el efecto de un cataclismo, que ha provocado la mutación en zombies de la mayor parte de los habitantes del planeta. La trama sigue a un grupo de humanos supervivientes, comandados por el policía Rick Grimes (Andrew Lincoln – 'Love Actually'), encargado de buscar un lugar seguro en el que poder vivir.La serie, como el cómic, explora las dificultades de los protagonistas para sobrevivir en un mundo poblado por el horror, así como las relaciones personales que se establecen entre ellos, en ocasiones también una amenaza para su supervivencia. Los personajes se enfrentan a todo tipo de retos y, sobre todo, a la constante presencia de la muerte, lo que les hará aún más fuertes para sobrevivir.",
		logo: 'components/mock/imgs/series/the_walking_dead.jpg',
		seasons: ['season1', 'season2'],
		rating: 9.5,
	},{
		id: 'yyy',
		title: 'Game of Thrones',
		summary: "The Game of Thrones serie is a...",
		seasons: ['season1', 'season2'],
		rating: 6.3,
	}];
}
