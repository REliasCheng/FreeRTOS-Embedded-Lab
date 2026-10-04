# 任务通信 | Task Communication

> [仓库首页](../README.md) · [同步与互斥](synchronization.md) · [ISR-to-Task](cortex-m-interrupts.md)

不同内核对象解决不同的信息传递问题。选型的关键是：是否需要携带数据、是否需要累计事件，以及接收方是否固定。

| 机制 | 传递内容 | 典型关系 |
| --- | --- | --- | --- |
| Queue | 固定大小的数据项 | producer → buffer → consumer |
| Event Group | 多个事件位 | event sources → condition wait |
| Task Notification | 任务内 32-bit 通知值 | sender → specific task |

## Queue

Queue 复制固定大小的数据项并保持先入先出顺序，可用于分离 UART 接收、协议解析与发送任务。

适用：生产者和消费者之间需要传递数据，或需要缓冲突发输入。

## Event Group

Event Group 用多个 bit 表达独立事件或组合条件。任务可以等待任意 bit 或所有目标 bit，并选择退出等待时是否清除事件位。

适用：不需要携带数据、但需要组合多个状态条件的场景。

## Task Notification

Task Notification 将 32-bit 通知值直接存放在目标任务的 TCB 中，可表达计数、覆盖/非覆盖写值与置位等模式。

适用：接收方固定、追求低开销，并且一个通知值足以表示事件或数据的任务间通信。

## 与 Semaphore 的区别

Semaphore 的重点是同步或资源计数，不承载一般消息内容。Queue 传数据，Semaphore 表示可用事件/资源；Task Notification 则在特定任务之间提供更轻量的直接通道。

## 相关内容

- 事件同步与共享资源互斥：[synchronization.md](synchronization.md)
- UART ISR 向 Queue 传递接收字节：[cortex-m-interrupts.md](cortex-m-interrupts.md)

