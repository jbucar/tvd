'use strict';

(function() {

var viewDef = {
	name: 'viewDef',
	kind: View,
	showing: true,
	menuOptions: { classes: 'class-submenu' },
	tabBindings: {
		tab1: 'content1',
		tab2: 'content2',
	},
	menuComponents: [
		{
			name: 'tab1',
			kind: 'Shell.TabButton',
			label: 'tab1',
			icon: '',
		},
		{
			name: 'tab2',
			kind: Shell.TabButton,
			label: 'tab2',
			icon: '',
		},
	],
	components: [
		{ name: 'content1', kind: 'Control', spotlight: true, content: 'content1' },
		{ name: 'content2', kind: 'Control', spotlight: true, content: 'content2', showing: false },
	]
};

function createWindow(def) {
	var view = app.createComponent(def, {owner: app});
	view.render();
	view.adjust();
	return view;
}

describe("TabbedWindow", function() {
	var view;

	beforeEach(function() {
		view = createWindow(viewDef);
	});

	afterEach(function () {
		enyo.Spotlight.unspot();
		view.destroy();
		view = undefined;
	});

	describe("Events", function() {
		it("should call onMenuFocus when spot it", function() {
			var spy = sinon.spy(view, 'onMenuFocus');
			spotlight.spot(view);
			spy.calledTwice.should.be.true;
			spy.firstCall.args[1].originator.isDescendantOf(view.$.menu).should.be.true; // A tab button
		});

		it("should call onMenuFocused when spot it", function() {
			var spy = sinon.spy(view, 'onMenuFocused');
			spotlight.spot(view);
			spy.calledTwice.should.be.true;
			spy.firstCall.args[1].originator.isDescendantOf(view.$.menu).should.be.true; // A tab button
		});

		it("should call onMenuEnter when spot it", function() {
			var spy = sinon.spy(view, 'onMenuEnter');
			spotlight.spot(view);
			spy.calledOnce.should.be.true;
		});

		it("should call onWrapperEnter when enter to the wrapper by pressing the down key", function() {
			var spy = sinon.spy(view, 'onWrapperEnter');
			spotlight.spot(view);
			navigate.down();
			spy.calledOnce.should.be.true;
		});

		it("should call onWrapperEnter when enter to the wrapper by pressing the enter key", function() {
			var spy = sinon.spy(view, 'onWrapperEnter');
			spotlight.spot(view);
			navigate.enter();
			spy.calledOnce.should.be.true;
		});

		xit("should not call onWrapperEnter when enter to the wrapper by pressing the enter key on a none tab", function() {
			var def = _.cloneDeep(viewDef);
			def.name = 'pepe';
			def.tabBindings.tab1 = 'none';
			var v = app.createComponent(def, {owner: app});
			v.render();
			v.adjust();

			var spy = sinon.spy(v, 'onWrapperEnter');
			spotlight.spot(v);
			navigate.enter();
			spy.called.should.be.false;

			v.destroy();
		});

		it("should not call onWrapperEnter when enter to the wrapper by pressing the down key on a none tab", function() {
			var def = _.cloneDeep(viewDef);
			def.name = 'pepe';
			def.tabBindings.tab1 = 'none';
			var v = app.createComponent(def, {owner: app});
			v.render();
			v.adjust();

			var spy = sinon.spy(v, 'onWrapperEnter');
			spotlight.spot(v);
			navigate.down();
			spy.called.should.be.false;

			v.destroy();
		});

		it("should call onFirstEnter when spot it for first time", function() {
			var spy = sinon.spy(view, 'onFirstEnter');
			spotlight.spot(view);
			spy.calledOnce.should.be.true;
		});

		it("should not call onFirstEnter when spot it for second time", function() {
			var spy = sinon.spy(view, 'onFirstEnter');
			spotlight.spot(view);
			spotlight.unspot();
			spotlight.spot(view);
			spy.calledOnce.should.be.true;
		});
	});

	describe("View", function() {
		it("should exist", function() {
			should.exist(view.hasNode());
		});

		it("should have a menu", function() {
			should.exist(view.$.menu.hasNode());
		});

		it("should have not an active menu when is spotted", function() {
			view.$.menu.hasClass('active').should.be.false;
		});

		it("should have an active menu when is spotted", function() {
			spotlight.spot(view);
			view.$.menu.hasClass('active').should.be.true;
			view.$.menu.hasClass('inactive').should.be.false;
		});

		it("should have a wrapper", function() {
			should.exist(view.$.wrapper.hasNode());
		});

		it("should have a wrapper inactive", function() {
			view.$.wrapper.hasClass('inactive').should.be.true;
		});

		it("should have a menu with the configured classes", function() {
			view.$.menu.hasClass('class-submenu').should.be.true;
		});

		it("should have a menu with 2 options", function() {
			view.$.menu.getClientControls().length.should.be.equal(2);
		});

		// TODO: Move to menu test
		it("should have a menu with the first option spotted", function() {
			spotlight.spot(view);
			var opt = view.$.menu.getClientControls()[0];
			opt.hasClass('spotlight').should.be.true;
		});

		it("should have a menu with the first option underlined", function() {
			spotlight.spot(view);
			var opt = view.$.menu.getClientControls()[0];
			opt.hasClass('freezed').should.be.true;
		});

		it("should have a menu without the second option spotted", function() {
			spotlight.spot(view);
			var opt = view.$.menu.getClientControls()[1];
			opt.hasClass('spotlight').should.be.false;
		});

		it("should have a menu without the second option underlined", function() {
			spotlight.spot(view);
			var opt = view.$.menu.getClientControls()[1];
			opt.hasClass('freezed').should.be.false;
		});

		it("should have a menu with the second option spotted when navigate to the right", function() {
			spotlight.spot(view);
			navigate.right();
			var opt = view.$.menu.getClientControls()[1];
			opt.hasClass('freezed').should.be.true;
			opt.hasClass('spotlight').should.be.true;
		});

		it("should spot the wrapper when navigate down", function() {
			spotlight.spot(view);
			navigate.down();
			spotlight.getCurrent().isDescendantOf(view.$.wrapper).should.be.true;
		});

		it("should spot the wrapper when enter", function() {
			spotlight.spot(view);
			navigate.enter();
			spotlight.getCurrent().isDescendantOf(view.$.wrapper).should.be.true;
		});

		it("should add the class active to the wrapper when enter it with enter key", function() {
			spotlight.spot(view);
			navigate.enter();
			view.$.wrapper.hasClass('active').should.be.true;
		});

		it("should add the class active to the wrapper when enter it with down key", function() {
			spotlight.spot(view);
			navigate.down();
			view.$.wrapper.hasClass('active').should.be.true;
		});

		it("should add the class active to the wrapper when leave it", function() {
			spotlight.spot(view);
			navigate.enter();
			navigate.back();
			view.$.wrapper.hasClass('inactive').should.be.true;
			view.$.wrapper.hasClass('active').should.be.false;
		});

		it("should add the class active to the wrapper when leave it with up key", function() {
			spotlight.spot(view);
			navigate.enter();
			navigate.up();
			view.$.wrapper.hasClass('inactive').should.be.true;
			view.$.wrapper.hasClass('active').should.be.false;
		});

		it("should remove the class active to the menu when leave it", function() {
			spotlight.spot(view);
			navigate.enter();
			view.$.menu.hasClass('active').should.be.false;
		});

		it("should add the class inactive to the menu when enter it by pressing enter", function() {
			spotlight.spot(view);
			navigate.enter();
			view.$.menu.hasClass('inactive').should.be.true;
		});

		it("should add the class inactive to the menu when enter it by pressing down", function() {
			spotlight.spot(view);
			navigate.down();
			view.$.menu.hasClass('inactive').should.be.true;
		});

		it("should add the class active to the menu when enter it", function() {
			spotlight.spot(view);
			navigate.enter();
			navigate.back();
			view.$.menu.hasClass('active').should.be.true;
		});

		it("should add the class deactive to the layer when enter it", function() {
			spotlight.spot(view);
			navigate.enter();
			view.$.layer.hasClass('deactive').should.be.true;
		});

		it("should remove the class deactive to the layer when enter it", function() {
			spotlight.spot(view);
			navigate.enter();
			navigate.back();
			view.$.layer.hasClass('deactive').should.be.false;
		});
	});

	describe('#viewClose', function() {
		it("should spot the menu when enter", function() {
			spotlight.spot(view);
			navigate.enter();
			view.viewClose();
			spotlight.spot(view);
			spotlight.getCurrent().isDescendantOf(view.$.menu).should.be.true;
		});
	});
});

var embedDef = {
	name: 'embedDef',
	kind: View,
	showing: true,
	menuOptions: { classes: 'class-submenu' },
	tabBindings: {
		'tab': viewDef.name,
	},
	menuComponents: [
		{
			name: 'tab',
			kind: 'Shell.TabButton',
			label: viewDef.name,
			icon: '',
		}
	],
	components: [ viewDef ] // Embed Window
};

// Describe cases when we have a TabbedWindow into another TabbedWindow
describe('EmbedTabbedWindow', function() {
	var view;
	beforeEach(function() {
		view = createWindow(embedDef);
	});

	afterEach(function () {
		view.destroy();
	});

	xit("should not add the class deactive to the layer when enter it", function() {
		spotlight.spot(view);
		navigate.enter();
		view.$.layer.hasClass('deactive').should.be.false;
	});

	it("should add the class deactive to the layer when enter in the inner window", function() {
		spotlight.spot(view);
		navigate.enter(); // Enter main window
		navigate.enter(); // Enter inner window
		view.$.layer.hasClass('deactive').should.be.true;
	});
});

})();
