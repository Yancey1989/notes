# 大规模分布式深度学习训练

## 背景

随着大数据以及 AI 技术的发展，AI 业务中需要处理处理的数据越来越庞大，并且随着 Deep Learning 算法的演进，需要的计算能力也越来越高，这需要企业拥有一套完整的 AI 计算平台并且能够大规模的调度训练进程，为了支持多种 AI 芯片(CPU, GPU 以及 ARM 等)
以及对应的不同计算库(Nvidia CUDA, Intel MKLDNN 以及 cuBlas 等)，训练框架除了需要能够支持大规模的训练以外还要能够兼容不同的计算设备。

## 通用计算机群 Kubernetes

为了能够将分布式的作业调度起来，我们首先需要一个机群并且有一套软件能够很好的将机群资源管理起来，传统的计算机群如 MPI、Slurm、Hadoop
等属于专用机群，如果我们希望在上面调度起来一个应用，我们需要根据机群管理软件提供的 API 来实现我们的调度逻辑，例如 MapReduce 作业
就有一个 Master 的进程，通过调用 Hadoop Yarn 提供的 API 将 Map 和 Reduce 的进程调度起来。

与专用机群相对应的是通用计算机群，Kubernetes 是 Google Borg 的开源版本，它可以使用户可以像管理操作系统中的进程一样管理集群中的
进程，例如 Kubernetes 提供的 ReplicaSet 对象，用户可以通过一个 YAML 文件来描述一个 ReplicaSet 对象，在其中指定需要运行的
Docker Image 以及副本数，通过 `kubectl` 命令行工具将 YAML 文件提交到 Kubernetes 集群，集群则会随机的在节点上启动对应数量的
“进程”（Kubernetes Pod），并且提供 Fault-tolerant 机制，一旦发现某些进程意外退出了，则会在其他节点上将进程重启，维持 `replicas` 个副本。

## 弹性深度学习系统 Elastic Deep Learning System (EDL)

传统 AI 计算机群如 MPI，用户提交训练任务时需要指定一个所需的计算资源，当机群中没有足够的资源时，所提交的作业将会一直等待，一直
到集群中有足够多的计算资源。举例来说，假设我们拥有一个 10 个计算节点的 GPU 集群，其中 5 个节点正在被占用，这时用户向集群
中提交一个训练任务， 并且指定需要的计算资源为6个节点，这时用户的任务由于没有足够的资源将无法被立即调度起来，
需要等待其余 5 个节点的作业退出后才能运行。这会产生两个问题：

- 用户体验差，并且无法快速的验证任务是否正确：用户提交任务后，需要等待一个很不确定的时间后才可以被调度起来；
- 集群资源利用率低：例子中的集群利用率只有 50%， 无法高效的利用率集群资源。

### Kubernetes TrainingJob Controller

EDL 实现了一个自定义的 Kubernetes Controller (TrainingJob Controller) 组件，使得用户可以使用一个 YAML 文件来描述
TrainingJob 的 属性，包括但不限于作业中 trainer 进程，parameter Server进程以及 master 进程
所需要的资源 (spec.resources)、指定 Runtime Docker image (spec.image) 等。用户通过命令行工具 `kubectl` 将
TrainingJob 的 YAML 文件提交到 Kubernetes 集群，TrainingJob Controller 将会将用户的配置解析成 Kubernetes 
原生的对象，例如使用 Job 启动 trainer 进程，使用 ReplicSet 启动 parameter Server 进程。

### Auto-scaller Component

为了根据集群中不同的进程的优先级调整每个训练作业中训练进程的数量，在 Training Job Controller 启动了一个

### PaddlePaddle Fault-tolerant Architecture

目前开源的深度学习框架中，还没有能够实现可容错的训练(fault-tolernat training), 当集群中一个训练节点失败后整个任务也将会失败，
为了使训练作业能够自由的增加/减少进程数量，我们在 PaddlePaddle 分布式训练框架中实现了一种新的 fault-tolerant 架构. 在
fault-tolernat 训练架构中我们引入了 master 进程，把训练进程转换成多个 RecordIO 格式的 Block 进行管理。
为了保存训练数据的处理情况，我们使用三个Queue 来管理这些Block:

<img src="images/fault-tolerant-queue.png" weight="500" />

1. 在每个 epoch 开始时将所有的 blocks 放在 TODO Queue 中
1. trainer 进程开始训练前会向 master 进程请求一个 block 数据开始训练，master 会将这个 block 放入到 PENDING Queue 中。
1. 在 DONE Queue 中保存已经训练完毕的 block，并且当全部的 block 被移动到 DONE Queue 时意味着当前 epoch 结束，master 重新
将 DONE Queue 中的 block 移动到 TODO Queue 中准备进行下一轮训练。
1. 特别的为了处理某个 trainer 在训练过程中失败，我们在 Pending Queue 中的 block 设定了一个 timeout threshold, 一旦有某个 block 超过了这个 threshold 就会被移动到 TODO queue 被重新训练。

## 并行训练框架

对于 AI 开发者来说，一个框架是否好用通常意味着：

- 是否拥有清晰、明确、灵活以及符合逻辑的 API 设计；
- 是否能够稳定、高性能的支持大规模并行训练；
- Operator 是否足够丰富以便能够配置出复杂的算法模型；
- 是否可以支持多种 AI 计算芯片，例如 CPU、GPU以及ARM芯片等；

其中是否能够稳定、高性能的支持大规模的并行训练是衡量一个框架是否工业可用的一个重要标准。

### 数据并行 VS. 模型并行

TODO: 图示

并行训练主要分为两种模式：模型并行以及数据并行。

- 所谓模型并行就是将一个模型中的 Layers 分布在多个计算设备/节点上进行训练，因为每个设备/节点是负责计算模型中的一个或多个
Layers，所以并行并行的模式可以训练超大参数的模型；
- 而所谓数据并行是每个 trainer 进程保存有相同的模型副本，每个训练节点负责计算一部分训练数据，通过 parameter server 或者
All-Reduce 的方式将参数同步更新, 因为每个 trainer 进程负责训练一部分数据，所以数据并行的模式可以训练超大规模训练数据的模型
训练任务。

### 基于 RPC 的 Parameter Server 分布式训练架构

parameter server 是一种比较通用的分布式训练架构，它的主要做法是将训练作业中的进程分为两种角色：parameter server 和 trainer

- trainer 负责计算所有参数的梯度，并且将参数的梯度发送到 parameter server 上进行更新，最后用 parameter server 更新后的
    参数替换本地的参数以备下一轮迭代使用。
- parameter server 进程中分布式地存储需要更新的参数，并且在满足一定条件时对参数进行更新，这里的特定条件是指定：
    - 同步更新：parameter server 会等所有 trainer 将梯度全部上传后进行更新，更新后所有 trainer 进程拿到的参数都是一样的。
    - 异步更新：parameter server 会在有梯度上传后就执行参数更新操作，一个 trainer 进程上传梯度后可以立即拿到更新的参数而无需
    等待其他 trainer 进程上传完梯度。相比同步更新，异步更新会带来更多的不可预见性，但却可以规避大规模训练中慢节点带来的影响。
    实际实现时，异步更新的策略也有所不同，一种折中的方案是 parameter server 上等待一定比例的 trainer 完成上传后再执行更新
    操作，这需要在实际模型中验证不同异步模式带来的影响。

### 基于 Collective API 的 Ring-base All-Reduce 架构

在 Ring-base All-Reduce 架构中没有额外存储参数的 parameter server 进程，而是使用 Nvidia NCCL2 All-Reduce
来聚合参数。因为没有独立的 parameter server 进程，所以训练过程中需要传输的数据相对要少，性能更好，缺点是灵活性不足，无法在
Parameter Server 上实现一些定制化需求：

对比维度 |Ring-base All-Reduce | Parameter Server
-- | -- | --
实现复杂度 | 低| 高
CPU 训练| 不支持 | 支持
异步训练 | 不支持 | 支持
容错训练 | 不支持 | 支持
性能 | 好 | 相对低
GDR | 支持 | 不支持

## 高性能并行训练技术

### Remote Directed Memory Access (RDMA)

### RPC Zero Copy

### Overlapping RPC and Computation Operators

### Fused Small Tensors

### Remote Sparse Update
