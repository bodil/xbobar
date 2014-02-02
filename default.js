/*global React */

var dom = require("dom");
var $ = require("jquery");
var moment = require("moment");

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
