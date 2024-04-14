module {
  toy.func @multiply_transpose(%a: tensor<*xf64> loc("tests/transpose.toy":1:24), %b: tensor<*xf64> loc("tests/transpose.toy":1:27)) -> tensor<*xf64> {
    %0 = toy.transpose(%a : tensor<*xf64>) to tensor<*xf64> loc("tests/transpose.toy":2:12)
    %1 = toy.transpose(%b : tensor<*xf64>) to tensor<*xf64> loc("tests/transpose.toy":2:27)
    %2 = toy.mul %0, %1 : tensor<*xf64> loc("tests/transpose.toy":2:25)
    toy.return %2 : tensor<*xf64> loc("tests/transpose.toy":2:5)
  } loc("tests/transpose.toy":1:1)
  toy.func @main() {
    %0 = toy.constant dense<[[1.0, 2.0], [3.0, 4.0]]> : tensor<2x2xf64> loc("tests/transpose.toy":6:19)
    %1 = toy.constant dense<[1.0, 2.0, 3.0, 4.0]> : tensor<4xf64> loc("tests/transpose.toy":7:19)
    %2 = toy.reshape(%1 : tensor<4xf64>) to tensor<2x2xf64> loc("tests/transpose.toy":7:5)
    %3 = toy.generic_call @multiply_transpose(%0, %2) : (tensor<2x2xf64>, tensor<2x2xf64>) -> tensor<*xf64> loc("tests/transpose.toy":8:13)
    toy.print %3 : tensor<*xf64> loc("tests/transpose.toy":9:5)
    toy.return loc("tests/transpose.toy":9:5)
  } loc("tests/transpose.toy":5:1)
} loc("tests/transpose.toy":1:1)
