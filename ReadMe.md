<div align="center">
  <img src="https://github.com/fengwang/ceras/blob/main/assets/logo.png"><br><br>
</div>

----


__ceras__ is yet another tiny deep learning engine.  __ceras__ mimiks tensorflow 1.x APIs, in pure C++20 and header-only. CUDA acceleration is limited to _convolutional_ and _dense_ layers, as __ceras__ is written for ordinary devices such as a gaming laptop with a GeForce GTX 1060, in which the GPU memory is limited.

----


## Table of contents

* [Features](#features)
* [Build](#build)
* [Supported layers/operations](#supported-layers)
* [Examples](#examples)
* [License](#license)
* [Acknowledgements](#acknowledgements)
* [TODO](#todo)


----

## Features
- Fast, with/without GPU:
    - 98% accuracy on MNIST in 10 epochs in 30s (loading dataset, training and validation on a laptop with Intel(R) Core(TM) i7-7700HQ and a mobile GTX 1060)
- Portable.
    - Runs anywhere as long as you have a compiler which supports C++20;
    - A Nvidia GPU is optional for acceleration, not a must;
    - header-only.
- Simply implemented:
    - mimicking TF grammar, but in C++.


## Build
**using this library**:

copy the `include` directory to the working directory, then inclue the header file

```cpp
#include "ceras.hpp"
```

**compiliation/link**:

```bash
g++ -c -std=c++20 -Wall -Wextra -ferror-limit=1 -ftemplate-backtrace-limit=0 -funsafe-math-optimizations  -Ofast -flto -pipe -march=native -DNDEBUG -o ./obj/test_mnist.o test/mnist.cc
g++ -o ./bin/test_mnist ./obj/test_mnist.o -funsafe-math-optimizations  -Ofast -flto -pipe -march=native
```

CUDA could be optionally enabled by defining macro `CUDA`: (tested with cuda 11.2.r11.2, gcc 10.2.0, note the compile/link options)

```bash
g++ -c -std=c++20 -Wall -Wextra -fmax-errors=1 -ftemplate-backtrace-limit=0 -funsafe-math-optimizations  -Ofast -flto -pipe -march=native -DCUDA -DNDEBUG -o ./obj/test_mnist.o test/mnist.cc
g++ -funsafe-math-optimizations  -Ofast -flto -pipe -march=native -o ./bin/test_mnist ./obj/test_mnist.o -L/opt/cuda/lib64 -pthread  -lcudart -lcublas
```

Note: As [Non-Type Template Parameters](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0732r2.pdf) is not yet implemented in clang, only gcc works with this library.

## Supported layers
+ Operations:
    - `plus`, or operator `+`;
    - `multiply`, or operator `*`, note this operation enables matrix-matrix multiplication, i.e., `dot` in numpy;
    - `log`;
    - `negative`;
    - `elementwise_product`, or `hadamard_product`;
    - `sum_reduct`;
    - `mean_reduce`;
    - `minus`, or operator `-`;
    - `square`;
    - `abs`;
    - `exp`;
    - `clip`;
    - `reshape`;
    - `flatten`;
    - `identity`;
    - `transpose`;
    - `conv2d`;
    - `drop_out`;
    - `max_pooling_2d`;
    - `average_pooling_2d`;
    - `up_sampling_2d`;
    - `batch_normalization`;
    - `instance_normalization`;
    - `concatenate`, or `concat`.
+ Activations:
    - `softmax`;
    - `selu`;
    - `softplus`;
    - `softsign`;
    - `sigmoid`;
    - `tanh`;
    - `relu`;
    - `leaky_relu`;
    - `elu`;
    - `exponential`;
    - `hard_sigmoid`;
    - `gelu`.
+ Losses:
    - `mae`;
    - `mse`;
    - `cross_entropy`.
+ Optimizers:
    - `sgd`;
    - `adagrad`;
    - `rmsprop`;
    - `adadelta`;
    - `adam`;
    - `gradient_descent`.


## Examples

### [defining a 3-layered NN, 256+128 hidden units](./test/mnist.cc) for mnist

**define the model**

```cpp
// define computation graph, a 3-layered dense net with topology 784x256x128x10
using namespace ceras;
auto input = place_holder<tensor<float>>{}; // 1-D, 28x28 pixels

// 1st layer
auto w1 = variable{ randn<float>( {28*28, 256}, 0.0, 10.0/(28.0*16.0) ) };
auto b1 = variable{ zeros<float>( { 1, 256 } ) };

auto l1 = relu( input * w1 + b1 );
/* alternatively with BN
auto l1_1 = input * w1 + b1;
auto gamma = variable{ ones<float>( {1, 256} ) };
auto beta = variable{ zeros<float>( {1, 256} ) };
auto l1 = relu( batch_normalization(0.95)( l1_1, gamma, beta ) );
*/

// 2nd layer
auto w2 = variable{ randn<float>( {256, 128}, 0.0, 3.14/(16.0*11.2 )) };
auto b2 = variable{ zeros<float>( { 1, 128 } ) };
//auto l2 = relu( l1 * w2 + b2 );
auto l2 = sigmoid( l1 * w2 + b2 );

// 3rd layer
auto w3 = variable{ randn<float>( {128, 10}, 0.0, 1.0/35.8 ) };
auto b3 = variable{ zeros<float>( { 1, 10 } ) };
auto output = l2 * w3 + b3;

auto ground_truth = place_holder<tensor<float>>{}; // 1-D, 10
auto loss = cross_entropy_loss( ground_truth, output );
```

**preparing dataset**

```cpp
std::size_t const batch_size = 10;
tensor<float> input_images{ {batch_size, 28*28} };
tensor<float> output_labels{ {batch_size, 10} };

std::size_t const epoch = 1;
std::size_t const iteration_per_epoch = 60000/batch_size;
```

**prepare session**

```cpp
// creating session
session<tensor<float>> s;
s.bind( input, input_images );
s.bind( ground_truth, output_labels );
```

**define optimizer**

```cpp
float learning_rate = 1.0e-1f;
auto optimizer = gradient_descent{ loss, batch_size, learning_rate };
```


**start training**

```cpp
for ( auto e : range( epoch ) )
{
    for ( auto i : range( iteration_per_epoch ) )
    {
        s.run( loss ); //forward pass
        s.run( optimizer ); //backward pass
    }
}
```

**make prediction**

```cpp
std::size_t new_batch_size = 1;
tensor<float> new_input_images{ {new_batch_size, 28 * 28} };
s.bind( input, new_input_images );

for ( auto i : range( tests ) )
{
    //prepare new_input_images as inputs
    auto precition = s.run( output );
    //post precess prediction
}
```

### [alternative] [define a convolutional model](./test/mnist_conv2d.cc)

```cpp
using namespace ceras;
auto input = place_holder<tensor<float>>{}; // 1-D, 28x28 pixels

auto l0 = reshape( {28, 28, 1} )( input );

auto k1 = variable{ randn<float>( {32, 3, 3, 1}, 0.0, 10.0/std::sqrt(32.0*3*3*1) ) };
auto l1 = relu( conv2d(28, 28, 1, 1, 1, 1, "valid" )( l0, k1 ) ); // 26, 26, 32

auto l2 = max_pooling_2d( 2 ) ( l1 ); // 13, 13, 32

auto k2 = variable{ randn<float>( {64, 3, 3, 32}, 0.0, 10.0/std::sqrt(64.0*3*3*1) ) };
auto l3 = relu( conv2d(13, 13, 1, 1, 1, 1, "valid")( l2, k2 ) ); // 11, 11, 64

auto l4 = max_pooling_2d( 2 )( l3 ); //5, 5, 64
auto l5 = drop_out(0.5)( flatten( l4 ) );

auto w6 = variable{ randn<float>( {5*5*64, 10}, 0.0, 10.0/std::sqrt(7.0*7*64*10) ) };
auto b6 = variable{ zeros<float>( {1, 10} ) };

auto l6 = l5 * w6 + b6;
auto output = l6;

auto ground_truth = place_holder<tensor<float>>{}; // 1-D, 10
auto loss = cross_entropy_loss( ground_truth, output );
```


## License

+ AGPLv3
+ Anti-996


## Acknowledgements

+ [Tensorflow 1](https://www.tensorflow.org/)
+ [TensorSlow](https://github.com/danielsabinasz/TensorSlow)
+ [Caffe](https://github.com/BVLC/caffe)

## TODO
+ save/load model
+ mimicking Tensorflow::Keras grammar
+ recurrent operations


