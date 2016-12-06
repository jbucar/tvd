/* globals Dialog */
/* exported ConfirmDialog */
function ConfirmDialog(res) {

	function cancelTimer(self) {
		if (self.interval) {
			clearInterval(self.interval);
			self.interval = null;
		}
	}

	let prot = {
		css: res.style('confirm')
	};

	prot.onconstruct = function(opts) {
		log.verbose( 'ConfirmDialog', 'onconstruct: opts=%j', opts);
		this._super(opts);
		opts.data.positive = res.txt('accept') + ' [3]';
		opts.partials.content = res.tpl('confirm');
	};

	prot.oncomplete = function() {
		log.info( 'ConfirmDialog', 'oncomplete');
		this._super();

		// Start accept timeout
		let timeout = 3;
		let label = res.txt('accept');
		this.set('positive', label + ' [3]');
		this.interval = setInterval(() => (--timeout >= 0) ? this.set('positive', label + ' [' + timeout + ']') : this.onResult(true), 1000);
	};

	// override
	prot.onResult = function(result) {
		log.info( 'ConfirmDialog', 'onResult: result=%s', result);
		cancelTimer(this);
		this._super(result);
	};

	// override
	prot.focusButton = function() {
		log.verbose( 'ConfirmDialog', 'focusButton' );
		cancelTimer(this);
		this.set('positive', res.txt('accept'));
		this._super();
	};

	return {
		extends: Dialog,
		proto: prot
	};
}
