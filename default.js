/*global React */

var dom = require("dom");
var $ = require("jquery");
var moment = require("moment");
var dbus = require("dbus");
var nm = require("networkmanager");

dom.loadCss("default.css");

var Clock = React.createClass({
  getTime: function() {
    return moment().format("ddd D MMM HH:mm");
  },
  getInitialState: function() {
    return { time: this.getTime() };
  },
  componentWillMount: function() {
    window.setInterval(function() {
      this.setState(this.getInitialState());
    }.bind(this), 1000);
  },
  render: function() {
    return React.DOM.div({className: "clock"}, this.state.time);
  }
});

var root = React.DOM.div(
  {id: "root"},
  React.DOM.div({className: "desktops"}, "desktops"),
  React.DOM.div({className: "windows"}, "windows"),
  React.DOM.div({className: "indicators"}, "indicators"),
  Clock());

React.renderComponent(root, document.body);

dom.showFrame();
dom.resizeFrame($("#root").outerHeight(), "top");

function debug(obs) {
  obs.subscribe(function(result) {
    console.log("result:", result);
  }, function(error) {
    console.error("error:", error);
  });
}

// __bobar.DBus.systemBus.call("org.freedesktop.NetworkManager",
//                             "/org/freedesktop/NetworkManager",
//                             "org.freedesktop.DBus.Properties",
//                             "GetAll", ["org.freedesktop.NetworkManager"])
//   .done.connect(function(err,data) {
//     console.log(err, data);
//   });

debug(
  nm.getDevices()
    .flatMap(function(dev) {
      console.log(dev);
      return nm.getDeviceInfo(dev);
    })
);
