# 内存管理 | Memory Management

主线工程实际选择 `heap_4.c` 为 FreeRTOS 动态内存实现。`xTaskCreate()`、Queue、Semaphore、Mutex 和 Software Timer 等动态创建接口最终从该 heap 获取内存。

## heap_4 行为

`heap_4.c` 在一个连续 heap 区域内管理空闲块：

1. `pvPortMalloc()` 查找足够大的空闲块；
2. 在空间允许时拆分空闲块；
3. `vPortFree()` 将块归还空闲链表；
4. 相邻空闲块合并，降低外部碎片。

配置中的 `configTOTAL_HEAP_SIZE` 决定这一区域的大小。多数主线配置使用 75 KiB。

## 静态任务创建

`04_FreeRTOS_create_task_static` 由应用提供 TCB 和任务栈，避免任务创建时从 FreeRTOS heap 分配这两部分内存。它不改变其他内核对象的分配方式。

## 范围

FreeRTOS 源码包包含其他 heap 实现，但主线工程没有形成 `heap_1`、`heap_2`、`heap_3` 和 `heap_5` 的独立实践与对比测试。本仓库也没有 heap 碎片率、剩余空间或任务栈高水位的实测记录。

## 代码入口

- [动态与静态任务创建](../projects/01-task-basics/)
- 各工程中的 `FreeRTOS/portable/MemMang/heap_4.c`

