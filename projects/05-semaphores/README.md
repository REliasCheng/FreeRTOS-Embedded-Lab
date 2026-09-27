# 信号量 | Semaphores

该组工程使用信号量连接任务事件、ISR 事件和可累计资源。

## 工程

- [`03_FreeRTOS_Binary_Semaphore`](course/03_FreeRTOS_Binary_Semaphore/)：二值信号量基本同步。
- [`04_FreeRTOS_Binary_Semaphore2`](course/04_FreeRTOS_Binary_Semaphore2/)：扩展二值信号量事件路径。
- [`05_FreeRTOS_Counting_Semaphore`](course/05_FreeRTOS_Counting_Semaphore/)：最大计数 5、初始计数 3 的计数信号量。

## 事件路径

```text
task / ISR → give semaphore → waiting task unblocks → process event
```

信号量传递事件或资源计数，不承载一般消息内容。详见 [Synchronization](../../docs/synchronization.md)。

