/*global exports, __bobar */

/** @module dbus */

var Rx = require("rx");

/** The standard DBus system bus.
 * @type Bus */
exports.systemBus = __bobar.DBus.systemBus;

 /** The standard DBus session bus.
  * @type Bus */
exports.sessionBus = __bobar.DBus.sessionBus;

/**
 * Invoke a DBus method.
 *
 * @param {Bus} bus - Connection object for the bus to call. Normally one of {@link module:dbus.systemBus} or {@link module:dbus.sessionBus}.
 * @param {string} service - The DBus service to call.
 * @param {string} path - The DBus object path to call.
 * @param {string} interface - The DBus interface to call.
 * @param {string} method - Name of the method to call.
 * @param {...*} args - Arguments to pass to the method.
 * @returns {Rx.Observable} Observable over the call's return value.
 */
exports.call = function call() {
  var callArgs = Array.prototype.slice.call(arguments, 0);
  if (callArgs.length < 5)
    throw new Error("dbus.call invoked with insufficient arguments (needs at least 6).");

  var bus = callArgs[0],
      service = callArgs[1],
      path = callArgs[2],
      iface = callArgs[3],
      method = callArgs[4],
      args = callArgs.slice(5);

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

  var response = bus.call(service, path, iface, method, args);
  return Rx.Observable.fromNodeCallback(response.done.connect)();
}

/**
 * Get the properties for a given path and interface.
 *
 * @param {Bus} bus - Connection object for the bus to call. Normally one of {@link module:dbus.systemBus} or {@link module:dbus.sessionBus}.
 * @param {string} service - The DBus service to call.
 * @param {string} path - The DBus object path to call.
 * @param {string} interface - The DBus interface to query.
 * @returns {Rx.Observable} Observable yielding the path's properties.
 */
exports.getProperties = function getProperties(bus, service, path, iface) {
  return exports.call(bus, service, path, "org.freedesktop.DBus.Properties", "GetAll", iface);
}
