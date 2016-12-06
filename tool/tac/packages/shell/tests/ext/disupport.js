(function() {

	var dep = {a: 'b'};
	var dep2 = {b: 'a'};

	var viewDef = {
		mixins: [DISupport],
		dependencies: {'someDep': dep, 'anotherDep': dep2},
	};

	describe("DISupport", function() {
		var view;
		beforeEach(function() {
			view = app.createComponent(viewDef, {owner: app});
			view.render();
		});

		afterEach(function () {
			view.destroy();
		});

		it('should add the dependencies', function() {
			view.getAPI('someDep').should.be.equal(dep);
			view.getAPI('anotherDep').should.be.equal(dep2);
		});
	});

})();
