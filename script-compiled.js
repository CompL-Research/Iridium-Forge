"use strict";

var Test = "outer";
function f1() {
  var a = arguments.length <= 0 || arguments[0] === undefined ? Test : arguments[0];
  var Test = arguments.length <= 1 || arguments[1] === undefined ? "args" : arguments[1];
  return (function () {
    var Test = "inner";
    return a;
  })();
}

console.log(f1());
