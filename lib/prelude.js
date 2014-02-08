/*global __bobar */

(function(exports) {

  /** @module prelude */

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

  /**
   * Loads a CommonJS module from the specified path.
   * Regular module path resolution is not performed.
   *
   * You shouldn't have to call this function directly under normal circumstances.
   * Prefer to load modules using the regular {@linkcode require} mechanism
   * instead.
   *
   * @param {string} path - Complete path to a Javascript file to load.
   * @returns {Object} The resulting CommonJS module, if successful.
   * @throws {Error}
   */
  exports.requireFile = function require(path) {
    var op = __bobar.readFileSync(path);
    if (op.error) throw new Error(op.error);
    return evalModule(op.result, op.path, op.dir);
  };

})(window);
