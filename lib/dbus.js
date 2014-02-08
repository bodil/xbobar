/*global exports, __bobar */

/** @module dbus */

/** The standard DBus system bus.
 * @type Bus */
exports.systemBus = __bobar.DBus.systemBus;

 /** The standard DBus session bus.
  * @type Bus */
exports.sessionBus = __bobar.DBus.sessionBus;

/**
 * A callback which is invoked upon the completion of a DBus method call.
 * @callback module:dbus~dbusCallback
 * @param {Object|null} error - An object describing the error that occurred, or {@linkcode null} if the call succeeded.
 * @param {*} result - The result of the DBus method call, or {@linkcode null} if there was an error.
 */

/**
 * Invoke a DBus method.
 *
 * @param {Bus} bus - Connection object for the bus to call. Normally one of {@link module:dbus.systemBus} or {@link module:dbus.sessionBus}.
 * @param {string} service - The DBus service to call.
 * @param {string} path - The DBus object path to call.
 * @param {string} interface - The DBus interface to call.
 * @param {string} method - Name of the method to call.
 * @param {...*} args - Arguments to pass to the method.
 * @param {module:dbus~dbusCallback} callback - A callback to invoke when the call completes.
 */
exports.call = function call() {
  var callArgs = Array.prototype.slice.call(arguments, 0);
  if (callArgs.length < 6)
    throw new Error("dbus.call invoked with insufficient arguments (needs at least 6).");

  var bus = callArgs[0],
      service = callArgs[1],
      path = callArgs[2],
      iface = callArgs[3],
      method = callArgs[4],
      args = callArgs.slice(5),
      callback = args.pop();

  if (typeof bus !== "object"
      || !bus.hasOwnProperty("objectName")
      || !bus.hasOwnProperty("call"))
    throw new Error("dbus.call argument \'bus\' must be a DBus bus object.");
  if (typeof service !== "string")
    throw new Error("dbus.call argument \'service\' must be a string.");
  if (typeof path !== "string")
    throw new Error("dbus.call argument \'path\' must be a string.");
  if (typeof iface !== "string")
    throw new Error("dbus.call argument \'interface\' must be a string.");
  if (typeof method !== "string")
    throw new Error("dbus.call argument \'method\' must be a string.");
  if (typeof callback !== "function")
    throw new Error("dbus.call argument \'callback\' must be a function.");

  var response = bus.call(service, path, iface, method, args);
  response.done.connect(callback);
}

exports.getProperties = function getProperties(bus, service, path, iface, callback) {
  exports.call(bus, service, path, "org.freedesktop.DBus.Properties", "GetAll", iface,
               callback);
}
