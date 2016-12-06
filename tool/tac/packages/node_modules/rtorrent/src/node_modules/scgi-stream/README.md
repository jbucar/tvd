scgi-stream
===========

Simple SCGI client.

Overview
--------

scgi-stream lets you talk to scgi-speaking services without having to go through
an intermediate HTTP server.

Installation
------------

Available via [npm](http://npmjs.org/):

> $ npm install scgi-stream

Or via git:

> $ git clone git://github.com/deoxxa/scgi-stream.git node_modules/scgi-stream

API
---

**request**

Makes an SCGI request. Returns an `SCGIRequest` object. Also does some wiring
for you so that the request returned will emit a `response` event when the
response begins to arrive. See the example section below for more information on
how this works.

```javascript
scgi.request(options);
```

```javascript
var scgi = require("scgi-stream");

scgi.request(options);
```

Arguments

* _options_ - an object specifying options for the request and response.
  Available options are `stream`, `host`, `port`, `path`, `method`, and
  `headers`. `stream` is optionally a stream object. If `stream` is not
  specified, `host` and `port`, which are a string and a number respectively,
  will be used to construct a TCP socket. `path` and `method` are strings, and
  `headers` is an object with a predictable structure.

**duplex**

Makes an SCGI request. Returns a duplex stream that combines both the request
and response parts of the operation. The stream returned will emit the `headers`
event like a regular `SCGIResponse` object, but will not emit a `response` event.

```javascript
scgi.duplex(options);
```

```javascript
var scgi = require("scgi-stream");

process.stdin.pipe(scgi.duplex(options)).pipe(process.stdout);
```

Arguments

* _options_ - same as the options for **request**

**SCGIRequest**

A duplex stream representing a request.

```javascript
new SCGIRequest(options);
```

```javascript
var req = new scgi.SCGIRequest({
  path: "/",
  headers: {
    "content-type": "text/plain",
  },
});

req.pipe(process.stdout);

req.end("hi there");
```

Arguments

* _options_ - an object specifying options for the request. Available options
  are `path`, `method`, and `headers`.

**SCGIResponse**

A duplex stream that parses and represents a response. Emits a `headers` event
when the headers for the stream have been parsed, then passes through all
following data.

```javascript
new SCGIResponse(options);
```

```javascript
var res = new scgi.SCGIResponse();

socket.pipe(res).pipe(process.stdout);

res.on("headers", function(headers) {
  console.log(headers);
});
```

Arguments

* _options_ - an object with the typical `stream.Transform` options such as
  `highWaterMark`, `objectMode`, etc.

Example
-------

Also see [example.js](https://github.com/deoxxa/scgi-stream/blob/master/example.js).

```javascript
var scgi = require("scgi-stream");

var req = scgi.request({
  host: "127.0.0.1",
  port: 17199,
  path: "/",
});

req.on("response", function(res) {
  res.on("headers", function(headers) {
    console.log(headers);
  });

  res.pipe(process.stdout);
});

req.end("<methodCall><methodName>download_list</methodName></methodCall>");
```

License
-------

3-clause BSD. A copy is included with the source.

Contact
-------

* GitHub ([deoxxa](http://github.com/deoxxa))
* Twitter ([@deoxxa](http://twitter.com/deoxxa))
* ADN ([@deoxxa](https://alpha.app.net/deoxxa))
* Email ([deoxxa@fknsrs.biz](mailto:deoxxa@fknsrs.biz))
