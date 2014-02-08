/*global exports, __bobar */

/** @module dom */

var $ = require("jquery");
var fs = require("fs");

/**
 * The window frame is initially hidden. Call this function when it's ready to be shown.
 * This will cause the frame's DOM to update itself according to the screen's settings,
 * which means that the dimensions of DOM elements can now be trusted. It will, however,
 * still have a height of 0, so once this is done, you should calculate the window
 * dimensions you're going to need and call {@link module:dom.resizeFrame}.
 *
 * @function
 * @see module:dom.resizeFrame
 */
exports.showFrame = __bobar.showFrame;

/**
 * Resize the window frame to the given height, and position it along the specified
 * screen edge.
 *
 * @function
 * @param {number} height - The required height (or width, if attached to the screen's left or right edge)
 * @param {string} attachment - One of {@linkcode "top"}, {@linkcode "right"}, {@linkcode "bottom"} or {@linkcode "left"}, specifying which screen edge to attach the frame to.
 * @see module:dom.showFrame
 */
exports.resizeFrame = __bobar.resizeFrame;

/**
 * Read a CSS file and apply it to the current document by inserting a {@linkcode style}
 * tag with its contents into the DOM tree.
 *
 * @param {string} path - Path to the CSS file to apply.
 */
exports.loadCss = function loadCss(path) {
  var doc = fs.readFileSync(path);
  var css = document.createElement("style");
  $(css).attr("type", "text/css");
  $(css).text(doc);
  $(document.head).append(css);
};
