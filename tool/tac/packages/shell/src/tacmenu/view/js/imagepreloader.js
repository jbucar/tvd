var ImageLoader = (function() {

	function loadImg(img, src) {
		return new Promise(function(resolve, reject) {
			img.onload = resolve;
			img.onerror = reject;
			img.src = src;
		});
	}

	return function constructor(collection) {
		var loaded;
		var cache = [];
		return function load(idx, size) {
			var mdls = collection.models.slice(idx, idx + size);

			if (loaded) {
				var unload = loaded.then(function() {
					// Drop the references to cached img.
					cache.length = 0;
				});
			}

			loaded = util.chain(mdls.map(function(mdl) {
				log.debug('ImagePreloader', 'Loading id=%s', mdl.get('id'));
				var img = new Image();
				cache.push(img);
				return loadImg(img, mdl.get('url'));
			}), unload || loaded);
		}
	}

})();