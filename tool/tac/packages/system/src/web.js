"use strict";

var tvdutil = require('tvdutil');
var WebModule = require('./webmodule');

module.exports = tvdutil.createWebService.bind(undefined,WebModule);

