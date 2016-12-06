'use strict';

(function() {

describe("TvdAdapter", function() {
	var model;

	beforeEach(function() {
		model = new TvdAdapter();
	});

	afterEach(function() {
		model = undefined;
	});

	it('should have not a current channgel', function() {
		model.current(false).should.be.equal(-1);
	});

	describe('#change', function() {
		describe('when success', function() {
			beforeEach(function() {
				model.addAPI('tvd', { player: { change: function(id, cbk) { cbk(undefined, id); } } } );
			});

			it('should set the current channel ', function() {
				var chID = 10;
				model.change(chID);
				model.current(false).should.be.equal(chID);
			});

			it('should emit an event', function() {
				var stub = sinon.stub(model, 'emit');
				var chID = 10;
				model.change(chID);
				stub.withArgs('changed', {isVirtual: false, chID: chID}).calledOnce.should.be.true;
			});

		});

		describe('when fails', function() {
			beforeEach(function() {
				model.addAPI('tvd', { player: { change: function(id, cbk) { cbk(new Error(), undefined); } } } );
			});

			it('should not change the current channel', function() {
				var chID = 10;
				model.change(chID);
				model.current(false).should.be.not.equal(chID);
			});

			it('should not emit an event', function() {
				var stub = sinon.stub(model, 'emit');
				var chID = 10;
				model.change(chID);
				stub.called.should.be.false;
			});
		});
	});

	describe('#previous', function() {
		beforeEach(function() {
			model.addAPI('tvd', { player: { change: function(id, cbk) { cbk(undefined, id); } } } );
		});

		it('should do nothing when there is no previous channel', function() {
			model.previous();
			model.current(false).should.be.equal(-1);
		});

		it('should do nothing when only change a channel once', function() {
			var cur = model.current(false);
			var chID = 10;
			model.change(chID);
			model.previous();
			model.current(false).should.be.equal(chID);
		});

		it('should change when there are a previous channel', function() {
			var prevChID = 10;
			model.change(prevChID);

			model.change(14);

			model.previous();
			model.current(false).should.be.equal(prevChID);
		});

		it('should change to the previous channel that was not -1', function() {
			var prevChID = 10;
			model.change(prevChID);

			model.change(-1);
			model.change(14);

			model.previous();
			model.current(false).should.be.equal(prevChID);
		});
	});

});

})();
