/*global exports */

/** @module networkmanager */

var dbus = require("dbus");

exports.getDevices = function getDevices(callback) {
  dbus.call(dbus.systemBus,
            "org.freedesktop.NetworkManager",
            "/org/freedesktop/NetworkManager",
            "org.freedesktop.NetworkManager",
            "GetDevices", callback);
}
