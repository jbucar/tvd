/* exported Log */
function Log() {
	// State
	const level = ['ERROR', 'WARN', 'INFO', 'VERBOSE'];
	let _current = 1;
	let _kindFilter;

	function doLog(lvl, logger, kind, msg, ...args) {
		if (lvl <= _current) {
			// Filtering
			if (!_kindFilter || _kindFilter === kind) {
				console[logger]( sprintf('[%s] ' + msg, kind, ...args) );
			}
		}
	}

	return {
		// filters:
		setLevel: lvl => _current = level.indexOf(lvl.toUpperCase()),
		setFilter: kind => _kindFilter = kind,

		// loggers:
		error   : $( doLog, 0, 'error' ),
		warn    : $( doLog, 1, 'warn' ),
		info    : $( doLog, 2, 'info' ),
		verbose : $( doLog, 3, 'debug' ),
	};
}
