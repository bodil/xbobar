/*global exports, __bobar */

var $ = require("jquery");
var fs = require("fs");

exports.resizeFrame = __bobar.resizeFrame;
exports.showFrame = __bobar.showFrame;

exports.loadCss = function loadCss(path) {
  var doc = fs.readFileSync(path);
  var css = document.createElement("style");
  $(css).attr("type", "text/css");
  $(css).text(doc);
  $(document.head).append(css);
};
