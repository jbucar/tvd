'use strict';
(function() {
	//TODO: remove this deps from scan
	var session = { close: testutil.noop };
	var channels = { populate: testutil.noop, empty: testutil.noop };

	var tvd = new EventEmitter();
	tvd.tuner = {};
	tvd.tuner.isScanning = testutil.noop;
	tvd.tuner.cancelScan = sinon.spy();
	tvd.tuner.startScan = sinon.spy();

	describe("Scan", function() {
		var model;

		beforeEach(function() {
			model = new ScanModel().addAPI('tvd', tvd).initialize();
		});

		afterEach(function() {
			tvd.tuner.cancelScan.reset();
			tvd.tuner.startScan.reset();
		});

		describe('start', function() {
			describe('when is not already scanning', function() {
				it('should start scan', function() {
					model.start();
					tvd.tuner.startScan.calledOnce.should.be.true;
				});
			});

			describe('when is scanning', function() {
				it('should not start scan', function() {
					model.set('isScanning', true);
					model.start();
					tvd.tuner.startScan.calledOnce.should.be.false;
				});
			});
		});

		describe('stop', function() {
			describe('when is pending', function() {
				it('should not call the api', function() {
					model.start();
					model.stop();
					tvd.tuner.cancelScan.called.should.be.false;
				});

				it('should cancel when the scan begins', function() {
					model.start();
					model.stop();
					tvd.emit('scanChanged', {state: 'begin'}); // Scan begins
					tvd.tuner.cancelScan.called.should.be.true;
				});
			});

			describe('when is not started', function() {
				it('should not call the api', function() {
					model.stop();
					tvd.tuner.cancelScan.called.should.be.false;
				});

				it('should emit the onEndScan event', function() {
					var spy = sinon.spy(model, 'emit');
					model.stop();
					spy.withArgs('onEndScan', {status: 'nonStarted'}).calledOnce.should.be.true;
				});
			});
		});
	});
})();
