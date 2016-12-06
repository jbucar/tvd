var stream = require("stream");

var SCGIRequest = module.exports = function SCGIRequest(options) {
  stream.Transform.call(this, options);

  this.method = options.method;
  this.path = options.path;
  this.headers = options.headers;

  this._buffer = [];
};
SCGIRequest.prototype = Object.create(stream.Transform.prototype, {properties: {constructor: SCGIRequest}});

SCGIRequest.prototype._transform = function _transform(input, encoding, done) {
  if (!Buffer.isBuffer(input)) {
    input = Buffer(input, encoding);
  }

  this._buffer.push(input);

  return done();
};

SCGIRequest.prototype._flush = function _flush(done) {
  var data = Buffer.concat(this._buffer);
  this._buffer = null;

  var headers = {};

  headers.CONTENT_LENGTH = data.length;
  headers.SCGI = 1;
  headers.REQUEST_METHOD = this.method || "GET";
  headers.REQUEST_URI = this.path;

  if (this.headers) {
    for (var k in this.headers) {
      headers[k.toUpperCase().replace(/-/g, "_")] = this.headers[k];
    }
  }

  var head = [];
  for (var k in headers) {
    head.push(Buffer(k));
    head.push(Buffer([0]));
    head.push(Buffer(headers[k].toString()));
    head.push(Buffer([0]));
  }
  head = Buffer.concat(head);

  this.push(head.length.toString(10));
  this.push(":");
  this.push(head);
  this.push(",");
  this.push(data);
  this.push(null);

  return done();
};
