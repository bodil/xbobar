/*global exports, __bobar */

/** @module fs */

/**
 * A callback which is invoked upon completing an asynchronous read operation.
 * @callback module:fs~readCallback
 * @param {Object|null} error - An object describing the error that occurred, or {@linkcode null} if the operation succeeded.
 * @param {*} result - The result of the read operation, or {@linkcode null} if there was an error.
 */

/**
 * Read a file synchronously.
 *
 * @param {string} path - Path to the file to read.
 * @returns {string} The contents of the file.
 * @throws {Error} Throws an exception if an I/O error occurred.
 */
exports.readFileSync = function readFileSync(path) {
  var op = __bobar.readFileSync(path);
  if (op.error) throw new Error(op.error);
  return op.result;
};

/**
 * Read the contents of a file.
 *
 * @param {string} path - Path to the file to read.
 * @param {module:fs~readCallback} callback - A callback accepting the arguments {@linkcode error} and {@linkcode result}.
 */
exports.readFile = function readFile(path, callback) {
  var op = __bobar.readFileSync(path);
  callback(op.error, op.result);
};

/**
 * Read the contents of an external URL.
 *
 * @param {string} url - The URL to read.
 * @param {module:fs~readCallback} callback - A callback accepting the arguments {@linkcode error} and {@linkcode result}.
 */
exports.readUrl = function readUrl(url, callback) {
  var uuid = __bobar.loadUrl(url);
  var listener = function urlRequestListener(incUuid, data) {
    if (uuid === incUuid) {
      __bobar.completedLoadUrl.disconnect(listener);
      callback(data.error, data.result);
    }
  };
  __bobar.completedLoadUrl.connect(listener);
}
