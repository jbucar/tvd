var net = require("net"),
    bun = require("bun");

var SCGIRequest = require("./lib/request"),
    SCGIResponse = require("./lib/response");

module.exports.SCGIRequest = SCGIRequest;
module.exports.SCGIResponse = SCGIResponse;

var request = function request(options) {
  var stream = options.stream || net.connect(options.port, options.host),
      req = new SCGIRequest(options),
      res = new SCGIResponse(options);

  req.pipe(stream);

  req.on("end", function() {
    req.emit("response", res);
    stream.pipe(res);
  });

  return req;
};

var duplex = function duplex(options) {
  var stream = options.stream || net.connect(options.port, options.host),
      req = new SCGIRequest(options),
      res = new SCGIResponse(options);

  var s = bun([req, stream, res]);

  res.on("headers", function(headers) {
    s.emit("headers", headers);
  });

  return s;
};

module.exports.request = request;
module.exports.duplex = duplex;
