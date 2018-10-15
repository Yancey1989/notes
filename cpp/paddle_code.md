# PaddlePaddle 部分源码学习笔记

人们通常通过叠加 Layer 的方式来描述一个模型，在 PaddlePaddle 中一个 Layer 由 多个 Operator 和与其相关的 Variable 组成，当用户通过 Layer 的 API 组成一个网络结构之后，会通过顺序执行引擎 `Executor` 或者带有依赖
分析的并行执行引擎 `ParallelExecutor` 来执行这些 Operators.

## Tensor

1. 什么是 Tensor ?

在神经网络中，我们可以用一个“向量”来描述和定义一个参数，例如 data layer 是一个第0维为 `batch_size` 的向量，在 PaddlePaddle 中使用 `Tensor` 结构来描述和定义一个向量，它有以下的特性：

    1. `DDim`: 维度信息，用来表示向量的维度。
    1. `Place`: 一个深度学习框架应该可以运行在多种设备中，至少是 CPU 和 GPU 设备，所以 Tensor 需要提供统一的接口使得计算逻辑不需要为不同的设备编写独立的代码。
    1. `Type`: Tensor 的结构需要支持多种不同的数据类型，例如 int，float32 以及 float16 等等。

1. 表示维度的结构: `DDim`

    Tensor 中提供了维度相关的接口：

    ``` c++
    const DDim& dims() const;
    Tensor& Resize(const DDim& dims);
    ```

    1. `DDim` 用来表示一个维度集合，它可以用 `auto my_dims = make_ddim({1,2,3,4})` 的方式进行初始化，也可以使用重载过的运算符`[]` 来获取某个 index 的维度： `EXPECT_EQ(my_dims[0] == 1)`.

    1. `Dim<i>` 表示维度为 `i`，并且把0-9的维度进行特例化并放在 `boost::variant` 中。

        ``` c++
        typedef boost::variant<Dim<0>, Dim<1>, Dim<2>, Dim<3>, Dim<4>, Dim<5>, Dim<6>,
                            Dim<7>, Dim<8>, Dim<9>>
          DDimVar;
        ```

        `Dim` 是一个模板类:

        ``` c++
        template<typename i>
        struct Dim {
          ...
          int64_t head;
          Dim<i - 1> tail;
        };
        ```
        其成员 `tail` 的类型也是一个特例化的 `Dim<i - 1>`, 可见这是一个递归的操作，所以我们对 `Dim` 模板类进行
        特例化，以保证递归是可以结束的, 以下是简化版的代码：

        ``` c++
        template <int i>
        class Dim {
        public:
            template <typename... Args>
            Dim(int64_t head, Args... tail): head_(head), tail_(tail...) {}

            Dim(): head_(0), tail_() {}
            int64_t head_;
            Dim<i - 1> tail_;
        };

        template <>
        class Dim<0> {
        public:
            Dim(int64_t head) {}
            Dim() {}
        };
        ```

        我们可以定义一个 `indexer` 函数来获取指定的维度值, 同样我们需要特例化 `D=0`的情况，以便结束递归：

        ``` c++
        template<int D>
        int64_t indexer(Dim<D>& dim, int idx) {
            if (idx == 0)
                return dim.head_;
            return indexer(dim.tail_, idx - 1);
        }

        template<>
        int64_t indexer<0>(Dim<0>& dim, int idx) {
            return 0;
        }
        ```
1. Place

TBD