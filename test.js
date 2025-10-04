var Test = "outer"
function f1(a = Test, Test = "args" ){
  var Test = "inner" 
  return a;
}

console.log(f1());