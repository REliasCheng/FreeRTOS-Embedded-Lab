# 任务通信 | Task Communication

不同内核对象解决不同的信息传递问题。选型的关键是：是否需要携带数据、是否需要累计事件，以及接收方是否固定。

## Queue

Queue 复制固定大小的数据项并保持先入先出顺序。`08_FreeRTOS_Queue` 同时创建基础类型队列和结构体队列；平衡球参考工程用队列分离 UART 接收、协议解析与发送任务。

适用：生产者和消费者之间需要传递数据，或需要缓冲突发输入。

## Event Group

Event Group 用多个 bit 表达独立事件或组合条件。任务可以等待任意 bit 或所有目标 bit，并选择退出等待时是否清除事件位。

适用：不需要携带数据、但需要组合多个状态条件的场景。

## Task Notification

Task Notification 将 32-bit 通知值直接存放在目标任务的 TCB 中。`10_FreeRTOS_Notify` 演示通知计数、覆盖/非覆盖写值与置位方式。

适用：接收方固定、追求低开销，并且一个通知值足以表示事件或数据的任务间通信。

## 与 Semaphore 的区别

Semaphore 的重点是同步或资源计数，不承载一般消息内容。Queue 传数据，Semaphore 表示可用事件/资源；Task Notification 则在特定任务之间提供更轻量的直接通道。

## 工程入口

- [Queue](../projects/07-queue/)
- [Event Groups](../projects/08-event-groups/)
- [Task Notifications](../projects/09-task-notifications/)
- [Semaphores](../projects/05-semaphores/)

