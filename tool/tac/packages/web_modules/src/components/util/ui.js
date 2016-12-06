/* exported UI */
function UI( resources ) {
	var self = {};
	assert(resources);

	var _constructors;
	var _components;
	var _views;
	var _stack;
	var _rootNode;
	var _keyHandlers = [];
	var _theme;

	function mapParentResources( aRes, parent ) {
		Object.keys(aRes).forEach(function(key) {
			if (key === '_parent') {
				aRes._parent = parent;
			}
			else if (typeof(aRes[key]) === 'object' && key.indexOf('_') !== 0) {
				mapParentResources( aRes[key], aRes );
			}
		});
	}
	mapParentResources(resources.data,undefined);

	function defaultOpts() {
		return {
			data: {},
			partials: {},
		};
	}

	function onKeyDown() {
		let code = event.which || event.keyCode;
		log.verbose( 'ui', 'OnKeyDown: code=%d', code );

		for (let i=0; i<_keyHandlers.length; i++) {
			if (!_keyHandlers[i](code)) {
				break;
			}
		}
		if (code === tacKeyboardLayout.VK_EXIT || code === tacKeyboardLayout.VK_BACK) {
			self.hideView();
		}
	}

	function createConstructor(compCtor) {
		let resFolder = resources.functions[compCtor.name.toLowerCase()];
		assert(resFolder,'Cannot get folder name for constructor: name=' + compCtor.name);
		return compCtor(self.getResources(resFolder));
	}

	function getConstructor(compCtor) {
		let tmp = _constructors[compCtor.name];
		if (!tmp) {
			tmp = _constructors[compCtor.name] = createConstructor(compCtor);
		}
		return tmp;
	}

	function createComponent(compCtor) {
		log.verbose( 'ui', 'createComponent: name=%s', compCtor.name );
		let comp = getConstructor(compCtor);
		let parent;
		if (comp.extends) {
			parent = self.getComponent(comp.extends);
			let parentOpts = getConstructor(comp.extends);
			if (parentOpts.proto.css) {
				comp.proto.css = parentOpts.proto.css + (comp.proto.css || '');
			}
		} else {
			parent = Ractive;
		}
		// TODO: ver si con esto seteado sigue andando todo (lo deberiamos utilizar en true)
		// comp.proto.isolated = true;
		return parent.extend(comp.proto);
	}

	function getView(ctor) {
		log.verbose( 'ui', 'getView: name=%s', ctor.name );
		if (!_views[ctor.name]) {
			let compCtor = self.getComponent(ctor);
			_views[ctor.name] = new compCtor(defaultOpts());
			_views[ctor.name].name = ctor.name;
			if (!_views[ctor.name].el) {
				_views[ctor.name].el = _rootNode;
			}
		}
		return _views[ctor.name];
	}

	function currentView() {
		return _stack[_stack.length-1];
	}

	function getRes( resName, type, cur ) {
		if (cur === undefined) {
			cur = resources.data;
		}

		let dirs = resName.split('/');
		let name;
		if (type !== undefined) {
			name = dirs.pop();
		}

		for (let i=0; i<dirs.length; i++){
			let n = dirs[i];
			if (n === '..') {
				n = '_parent';
			}
			cur = cur[n];
		}
		assert(cur !== undefined, 'Resource not found: res=' + resName );

		if (type === undefined) {
			return cur;
		}

		let resTypes = cur[type];
		assert(resTypes[name] !== undefined, 'No resources for type: type=' + type + ', res=' + resName );
		return resTypes[name];
	}

	self.getResource = function(type,res) {
		return getRes( res, '_' + type );
	};

	self.absPath = function( res ) {
		let parentDir = window.location.pathname;
		return parentDir.substring(0, parentDir.lastIndexOf('/') + 1) + res;
	};

	self.getResources = function( folderName ) {
		let res = getRes( folderName );
		assert(res !== undefined, 'Cannot find resource for folder: folderName=' + folderName );

		function get(key, id, fullPath) {
			let val = (id.indexOf('/') === -1) ? res[key][id] : getRes( id, key, res );
			assert( val, 'Cannot get resource: folder=' + folderName + ', type=' + key + ', id=' + id );
			if (fullPath) {
				val = self.absPath( val );
			}
			return val;
		}

		var obj = {};
		obj.tpl = $(get,'_templates');
		obj.style = $(get,'_styles');
		obj.styles = function() {
			let content = '';
			Array.prototype.forEach.call(arguments,function(resName) {
				let style = get( '_styles', resName );
				if (content) {
					content += '\n';
				}
				content += style;
			});
			return content;
		};
		obj.txt = $(get,'_strings');
		obj.img = $(get,'_images');

		return obj;
	};

	self.getComponent = function(comp) {
		log.verbose( 'ui', 'getComponent: name=%s', comp.name );
		let tmp = _components[comp.name];
		if (!tmp) {
			tmp = _components[comp.name] = createComponent(comp);
		}
		return tmp;
	};

	self.getComponents = function() {
		return Array.prototype.reduce.call(arguments, function(result,comp) {
			result[comp.name] = self.getComponent( comp );
			return result;
		}, {});
	};

	self.getModels = function(comp) {
		log.verbose( 'ui', 'getModels: name=%s', comp.name );
		return getConstructor(comp).models || {};
	};

	self.setDefaults = function(ractive, defaultValues) {
		for (let key in defaultValues) {
			if (ractive.get(key) === undefined) {
				ractive.set(key, defaultValues[key]);
			}
		}
	};

	self.getTheme = function() {
		return _theme;
	};

	self.registerKeyHandler = function( fnc ) {
		_keyHandlers.push( fnc );
	};

	self.unregisterKeyHandler = function(fnc) {
		let index = _keyHandlers.findIndex( (f) => f === fnc );
		_keyHandlers.splice(index, 1);
	};

	self.init = function( opts ) {
		log.verbose( 'ui', 'Initialize views' );

		{
			if (!opts.el) {
				let body = document.body;
				body.innerHTML = '<main style="width: 100vw; height: 100vh; outline: none;"></main>';
				body.addEventListener('focusin', () => body.scrollTop = 0, false);
				opts.el = "main";
			}

			_rootNode = document.querySelector(opts.el);
			_rootNode.onkeydown = onKeyDown;
			_rootNode.tabIndex = "0";
		}

		Ractive.DEBUG=false;
		Ractive.defaults.magic = true;
		_constructors = {};
		_components = {};
		_views = {};
		_stack = [];
		_theme = opts.theme || {
			bgColor: "rgb(44,44,44)",
			fgColor: "rgb(65,65,65)",
			focusColor: "rgb(128,128,128)"
		};

		self.showView( opts.main );
	};

	self.fin = function() {
		log.verbose( 'ui', 'Finalize views' );

		while (_stack.length > 0) {
			_stack.pop().unrender();
		}
		return Promise.map( Object.keys(_views), (v) => _views[v].teardown() )
			.then(function() {
				_components = {};
				_constructors = {};
				_views = {};
			});
	};

	self.showView = function( ctor, param ) {
		log.verbose( 'ui', 'showView: name=%s', ctor.name );

		let view = getView(ctor);
		assert(view);

		let cur = currentView();
		if (cur) {
			cur.unrender();
		}

		_stack.push( view );

		if (param !== undefined) {
			view.set( {viewParam: param} );
		}

		view.render( view.el );
		view.el.focus();
		return view;
	};

	self.hideView = function() {
		log.verbose( 'ui', 'hideView' );
		if (_stack.length > 1) {
			_stack.pop().unrender();
			let cur = currentView();
			cur.render(cur.el);
		}
	};

	return self;
}
