module {
  toy.func @multiply_transpose(%arg0: tensor<*xf64> loc("tests/transpose.toy":1:1), %arg1: tensor<*xf64> loc("tests/transpose.toy":1:1)) -> tensor<*xf64> {
    %0 = toy.transpose(%arg0 : tensor<*xf64>) to tensor<*xf64> loc("tests/transpose.toy":2:12)
    %1 = toy.transpose(%arg1 : tensor<*xf64>) to tensor<*xf64> loc("tests/transpose.toy":2:27)
    %2 = toy.mul %0, %1 : tensor<*xf64> loc("tests/transpose.toy":2:27)
    toy.return %2 : tensor<*xf64> loc("tests/transpose.toy":2:5)
  } loc("tests/transpose.toy":1:1)
  toy.func @main() {
    %0 = toy.constant dense<[[1.000000e+00, 2.000000e+00], [3.000000e+00, 4.000000e+00]]> : tensor<2x2xf64> loc("tests/transpose.toy":6:19)
    %1 = toy.reshape(%0 : tensor<2x2xf64>) to tensor<2x2xf64> loc("tests/transpose.toy":6:5)
    %2 = toy.constant dense<[1.000000e+00, 2.000000e+00, 3.000000e+00, 4.000000e+00]> : tensor<4xf64> loc("tests/transpose.toy":7:19)
    %3 = toy.reshape(%2 : tensor<4xf64>) to tensor<2x2xf64> loc("tests/transpose.toy":7:5)
    %4 = toy.generic_call @multiply_transpose(%1, %3) : (tensor<2x2xf64>, tensor<2x2xf64>) -> tensor<*xf64> loc("tests/transpose.toy":8:13)
    toy.print %4 : tensor<*xf64> loc("tests/transpose.toy":9:5)
    toy.return loc("tests/transpose.toy":5:1)
  } loc("tests/transpose.toy":5:1)
} loc(unknown)
