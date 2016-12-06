#!/usr/bin/env node

var scgi = require("./scgi");

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
