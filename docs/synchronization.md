# 同步与资源管理 | Synchronization

> [仓库首页](../README.md) · [任务通信](task-communication.md) · [中断与 FreeRTOS](cortex-m-interrupts.md)

同步回答“事件是否发生”，互斥回答“共享资源当前由谁访问”。虽然 FreeRTOS 的 Semaphore 与 Mutex 共用部分队列机制，它们的工程语义不同。

```text
event synchronization → Binary / Counting Semaphore
shared resource       → Mutex / Recursive Mutex
very short atomic work → Critical Section
```

## Binary Semaphore

二值信号量只有可用/不可用两个状态。设计时需要分别处理任务间释放/获取和 ISR 使用 `FromISR` API 唤醒等待任务的路径。

## Counting Semaphore

计数信号量维护 0 到最大值之间的计数，可表示可用资源数量或累计事件次数。最大值和初始值必须由资源模型确定。

## Mutex 与 Recursive Mutex

Mutex 用于保护共享资源，并具有面向任务所有权的语义。FreeRTOS Mutex 支持 Priority Inheritance，降低低优先级任务持锁时阻塞高优先级任务造成的优先级反转影响。

Recursive Mutex 允许同一任务多次取得同一把锁，但必须匹配次数释放。当前仓库没有实现或优先级反转时序测量记录。

## Critical Section

临界区通过短时间屏蔽可受内核管理的中断来保护极短代码段。它不适合包围可能阻塞或耗时的处理。任务级资源长期互斥应优先使用 Mutex。

## 相关内容

- Queue、Event Group 与 Task Notification：[task-communication.md](task-communication.md)
- ISR 释放信号量后的任务唤醒：[cortex-m-interrupts.md](cortex-m-interrupts.md)

