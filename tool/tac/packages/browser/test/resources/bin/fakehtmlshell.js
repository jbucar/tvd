#!/usr/bin/env node

'use strict';

var ZmqSocket = require('../../src/util/zmqsocket');
var temp = require('temp').track();
var tvdutil = require('tvdutil');

tvdutil.setupLog( 'info' );

var realId = 10;
var htmlshell_port = 'ipc://' + temp.openSync('FakeHtmlShellPort-' + process.pid + '_').path;
var browser_port = '';
var exitAfterPing = false;
var pingCount = 0;

process.argv.slice(2).forEach( function (arg) {
	if (arg === 'send-empty-url') {
		htmlshell_port = '';
	}
	else if ((/^--server-port=/).test(arg)) {
		browser_port = arg.substr(14);
	}
	else if (arg === 'exit-after-first-ping') {
		exitAfterPing = true;
	}
});
console.log('[%d] Init! HtmlShellPort=%s, BrowserServicePort=%s', process.pid, htmlshell_port, browser_port);

var client = ZmqSocket.createClient(browser_port);

var server = ZmqSocket.createServer(htmlshell_port, function ( msg ) {
	console.log('[%d] Received request: %j', process.pid, msg);
	switch (msg.type) {
		case 'ping':
			if (exitAfterPing && pingCount >= 1) {
				exit();
			} else {
				client.send('{"type":"ping_ack"}');
				pingCount++;
			}
			break;
		case 'cmd':
			switch (msg.name) {
				case 'stop':
					exit(0);
					break;
				case 'launch_browser':
					client.send(JSON.stringify({
						type: 'cmd',
						name: 'browser_launched',
						real_id: ++realId,
						browser_id: msg.browser_id,
					}));
					break;
				case 'close_browser':
					client.send(JSON.stringify({
						type: 'cmd',
						name: 'browser_closed',
						real_id: msg.real_id,
					}));
				break;
			}
			break;
	}
});

if (client) {
	client.send('{"type":"client_up","url":"' + htmlshell_port + '"}');
	if (htmlshell_port === '') {
		exit(0);
	}
}

function exit( code ) {
	code = code | 0;
	console.log('[%d] Fin!', process.pid);
	if (client) {
		ZmqSocket.safeClose(client);
		client = undefined;
	}
	if (server) {
		ZmqSocket.safeClose(server);
		server = undefined;
	}
	temp.cleanupSync();
	process.exit(code);
}

process.on('SIGTERM', function() { exit(1); });
