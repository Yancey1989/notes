# PaddlePaddle 部分源码学习笔记

## Tensor

### 什么是 Tensor ?

在神经网络中，我们可以用一个“向量”来描述和定义一个参数，例如 data layer 是一个第0维为 `batch_size` 的向量，在 PaddlePaddle 中使用 `Tensor` 结构来描述和定义一个向量：

1. `DDim`: 维度信息，用来表示向量的维度。
1. `Place`: 一个深度学习框架应该可以运行在多种设备中，至少是 CPU 和 GPU 设备，所以 Tensor 需要提供统一的接口使得计算逻辑不需要为不同的设备编写独立的代码。
1. `Type`: Tensor 的结构需要支持多种不同的数据类型，例如 int，float32 以及 float16 等等。

### 表示维度的结构: `DDim`

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

    我们可以声明一个 `Dim` 对象并使用 `indexer` 来获取某一个维度是值：

    ``` c++
    int main(int argc, char** argv) {
        Dim<3> d(1,2,3);
        std::cout << indexer<3>(d, 2) << std::endl;
    }
    ```

### 指向数据的指针 `std::shared_ptr<Placeholder> holder_`

在 PaddlePaddle 的 Tensor实现中，并不保存数据, 而只含有对 Tensor 的基本操作,
对数据的访问是通过对 `PlaceHolder` 的指针来操作的，对于 Tensor 的数据而言，我们需要知道：

1. 数据在哪里？CPU 还是 GPU？
1. 数据的类型是什么？int, float16 还是 float32？

为此 `Placeholder` 中有两个重要的成员: 表示所在设备的 `platform::Place place_` 以及 数据类型 `std::type_index type_`:

``` c++
struct Placeholder {
  virtual ~Placeholder() = default;
  virtual void* ptr() const = 0;
  virtual size_t size() const = 0;
  virtual std::type_index type() const = 0;
  virtual platform::Place place() const = 0;
  virtual void set_type(std::type_index type) = 0;
  virtual void set_place(platform::Place place) = 0;
};

template <typename Place>
struct PlaceholderImpl : public Placeholder {
  PlaceholderImpl(Place place, size_t size, std::type_index type)
      : ptr_(static_cast<uint8_t*>(memory::Alloc(place, size)),
            memory::PODDeleter<uint8_t, Place>(place)),
        place_(place),
        size_(size),
        type_(type) { ... }
  ...

  std::unique_ptr<uint8_t, memory::PODDeleter<uint8_t, Place>> ptr_;
}
```

1. `Placeholder` 是一个纯虚基类，定义了一些基本的操作, `PlaceholderImpl` 实现了 `Placeholder` 接口。
1. `memory::Alloc(place, size)` 和 `memory::PODDeleter<uint8_t, Place>(place)` 是 PaddlePaddle
    中实现的内存管理器，`Alloc(place,size)` 和 `PODDelete<uint8_t, Place>(place)` 分别负责申请和释放内存。