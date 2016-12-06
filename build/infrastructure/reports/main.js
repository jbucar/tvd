var request = require('request-promise');
var nodemailer = require('nodemailer');
var bPromise = require('bluebird');
var util = require('util');

function property(prop) {
	return function(obj) {
		return obj[prop];
	}
}

function accumulate(total, cur) {
	return total + cur;
}

function configureSender() {
	var mailer = nodemailer.createTransport({
		service: 'Gmail',
		auth: {
			user: 'denethor.tvdlifia@gmail.com',
			pass: 'Tvd03034567'
		}
	});

	var send = bPromise.promisify(mailer.sendMail.bind(mailer));

	return function(txt) {
		var opts = {
			from: 'Denethor <denethor.tvdlifia@gmail.com>',
			to: 'tvd-lifia-pieza3@googlegroups.com',
			text: txt,
			subject: 'Reporte Buildbot',
		};
		return send(opts);
	};
}

var sendMail = configureSender();

var slaves = ['WinSeven32', 'ArchI7', 'Linux32'];

var mail_title = 'Builds pendientes: ';
var treeshold = 2; // More than 2 buils pendings;
var baseURI = 'http://172.16.0.206:8009/json/';

function requestJSON(opts) {
	return request(opts).then(JSON.parse);
}

function requestBuilder(builder) {
	return requestJSON({uri: baseURI + 'builders/' + builder})
}

function getBuilders(slave) {
	return requestJSON({uri: baseURI  + 'slaves/' + slave})
	.then(property('builders'))
	.then(Object.keys);
}

function getPendings(slave) {
	return getBuilders(slave)
	.map(requestBuilder)
	.map(property('pendingBuilds'))
	.reduce(accumulate, 0)
	.then(function(pending) {
		return [slave, pending];
	});
}

function sendReport() {
	bPromise.all(slaves.map(function(slave) { return getPendings(slave); })).then(function(slavePendings) {
		var pendings = slavePendings.filter(function(sp) {
			return sp[1] > treeshold;
		});
		if (pendings.length) {
			return bPromise.reduce(pendings, function(total, slavePending) {
				return util.format('%s \n\t %s : %s builds pendientes', total, slavePending[0], slavePending[1]);
			}, mail_title)
			.then(sendMail)
			.then(function(info) {
				console.log(info.response);
			});
		}
	})
	.catch(console.error.bind(console));
}

sendReport();
