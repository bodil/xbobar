/*global exports, __bobar */

exports.readFileSync = function readFileSync(path) {
  var op = __bobar.readFileSync(path);
  if (op.error) throw new Error(op.error);
  return op.result;
};

exports.readFile = function readFile(path, callback) {
  var op = __bobar.readFileSync(path);
  callback(op.error, op.result);
};

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
