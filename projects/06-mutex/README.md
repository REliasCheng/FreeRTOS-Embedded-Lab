# 互斥量 | Mutex

> [仓库首页](../../README.md) · 上一模块：[Semaphores](../05-semaphores/) · 下一模块：[Queue](../07-queue/)

该组工程用于保护任务之间共享的资源。

## 工程

- [`06_FreeRTOS_Mutex_Semaphore`](course/06_FreeRTOS_Mutex_Semaphore/)：普通 Mutex 的取得与释放。
- [`07_FreeRTOS_RecursiveMutex_Semaphore`](course/07_FreeRTOS_RecursiveMutex_Semaphore/)：同一任务可递归取得的 Mutex。

Mutex 具有任务所有权和 Priority Inheritance 语义；Recursive Mutex 要求取得与释放次数匹配。保留工程聚焦对象创建、取得与释放流程。

详见 [Synchronization](../../docs/synchronization.md)。

