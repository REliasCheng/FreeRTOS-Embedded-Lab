# 同步与资源管理 | Synchronization

> [仓库首页](../README.md) · [任务通信](task-communication.md) · [中断与 FreeRTOS](cortex-m-interrupts.md)

同步回答“事件是否发生”，互斥回答“共享资源当前由谁访问”。虽然 FreeRTOS 的 Semaphore 与 Mutex 共用部分队列机制，它们的工程语义不同。

```text
event synchronization → Binary / Counting Semaphore
shared resource       → Mutex / Recursive Mutex
very short atomic work → Critical Section
```

## Binary Semaphore

二值信号量只有可用/不可用两个状态。主线工程包含任务间释放与获取，也包含 ISR 调用 `xSemaphoreGiveFromISR()` 唤醒等待任务的路径。

## Counting Semaphore

计数信号量维护 0 到最大值之间的计数，可表示可用资源数量或累计事件次数。`05_FreeRTOS_Counting_Semaphore` 以最大值 5、初始值 3 创建对象。

## Mutex 与 Recursive Mutex

Mutex 用于保护共享资源，并具有面向任务所有权的语义。FreeRTOS Mutex 支持 Priority Inheritance，降低低优先级任务持锁时阻塞高优先级任务造成的优先级反转影响。

Recursive Mutex 允许同一任务多次取得同一把锁，但必须匹配次数释放。对应工程演示对象创建和递归访问结构；仓库没有优先级反转时序测量记录。

## Critical Section

临界区通过短时间屏蔽可受内核管理的中断来保护极短代码段。它不适合包围可能阻塞或耗时的处理。任务级资源长期互斥应优先使用 Mutex。

## 对应工程

- [Semaphores](../projects/05-semaphores/)
- [Mutex](../projects/06-mutex/)
- [Interrupt Collaboration](cortex-m-interrupts.md)

## 相关内容

- Queue、Event Group 与 Task Notification：[task-communication.md](task-communication.md)
- ISR 释放信号量后的任务唤醒：[cortex-m-interrupts.md](cortex-m-interrupts.md)

