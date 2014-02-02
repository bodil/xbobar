/*global __bobar */

(function(exports) {

  function evalModule(__code, __filename, __dirname) {
    return eval("(function() {\n\"use strict\";\n" +
                "var module = {};\n" +
                "var exports = module.exports = {};\n" +
                "var process = { env: {} };\n" +
                "var require = function(requirePath) {\n" +
                "  var path = __bobar.searchRequirePath(__dirname, requirePath);" +
                "  if (path) { return window.requireFile(path); }\n" +
                "  else { throw new Error(\"module not found: \" + requirePath); }\n" +
                "};\n" +
                __code + "\n;\nreturn module.exports;})()");
  }

  exports.requireFile = function require(path) {
    var op = __bobar.readFileSync(path);
    if (op.error) throw new Error(op.error);
    return evalModule(op.result, op.path, op.dir);
  };

})(window);
