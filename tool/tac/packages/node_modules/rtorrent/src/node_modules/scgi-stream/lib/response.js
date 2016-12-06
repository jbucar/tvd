var stream = require("stream");

var SCGIResponse = module.exports = function SCGIResponse(options) {
  stream.Transform.call(this, options);

  this._buffer = Buffer(1024);
  this._offset = 0;
  this._doneHeaders = false;
};
SCGIResponse.prototype = Object.create(stream.Transform.prototype, {properties: {constructor: SCGIResponse}});

SCGIResponse.prototype._transform = function _transform(input, encoding, done) {
  if (this._doneHeaders) {
    this.push(input);

    return done();
  }

  var i;
  for (i=0;i<input.length;++i) {
    this._buffer[this._offset++] = input[i];

    if (this._offset === this._buffer.length) {
      this._buffer = Buffer.concat(this._buffer, Buffer(1024));
    }

    if (input[i] === 0x0a && this._offset >= 4 && this._buffer.toString("hex", this._offset - 4, this._offset) === "0d0a0d0a") {
      this._doneHeaders = true;
      i++;
      break;
    }
  }

  if (this._doneHeaders) {
    var headers = this._buffer.toString("ascii", 0, this._offset).trim().split("\r\n").map(function(e) {
      var bits = e.split(":");
      return [bits.shift().trim(), bits.join(":").trim()];
    }).reduce(function(i, v) {
      i[v[0].toLowerCase()] = v[1];
      return i;
    }, {});

    this.emit("headers", headers);
  }

  this.push(input.slice(i));

  done();
};
