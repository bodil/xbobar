/*global exports,  */

/** @module networkmanager */

var dbus = require("dbus");
var Rx = require("rx");

exports.getDevices = function getDevices() {
  return dbus.call(dbus.systemBus,
                   "org.freedesktop.NetworkManager",
                   "/org/freedesktop/NetworkManager",
                   "org.freedesktop.NetworkManager",
                   "GetDevices")
    .flatMap(function(r) {
      return Rx.Observable.fromArray(r[0]);
    });
}

function devInfo(path, iface) {
  return dbus.getProperties(dbus.systemBus, "org.freedesktop.NetworkManager",
                            path, iface)
    .pluck(0);
}

function merge(a, b) {
  var i, out = {};
  for (i in a) {
    if (a.hasOwnProperty(i)) out[i] = a[i];
  }
  for (i in b) {
    if (b.hasOwnProperty(i)) out[i] = b[i];
  }
  return out;
}

function mergeDev(path, iface) {
  return function(dev) {
    return devInfo(path, iface).catch(Rx.Observable.return({})).map(function(newDev) {
      return merge(dev, newDev);
    });
  };
}

exports.getDeviceInfo = function getDeviceInfo(path) {
  return devInfo(path, "org.freedesktop.NetworkManager.Device")
    .map(function(dev) {
      return merge(dev, {objectPath: path});
    })
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Wireless"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Wired"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Cdma"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Gsm"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Serial"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Bluetooth"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.OlpcMesh"))
    .flatMap(mergeDev(path, "org.freedesktop.NetworkManager.Device.Modem"));
}
