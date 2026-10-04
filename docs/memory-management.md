# 内存管理 | Memory Management

> [仓库首页](../README.md) · [任务管理](task-management.md) · [FreeRTOSConfig](freertos-configuration.md)

本文以 `heap_4.c` 的公开设计概念说明动态内存关系。若个人工程选择该实现，`xTaskCreate()`、Queue、Semaphore、Mutex 和 Software Timer 等动态创建接口会从配置的 FreeRTOS heap 获取内存。

## heap_4 行为

`heap_4.c` 在一个连续 heap 区域内管理空闲块：

1. `pvPortMalloc()` 查找足够大的空闲块；
2. 在空间允许时拆分空闲块；
3. `vPortFree()` 将块归还空闲链表；
4. 相邻空闲块合并，降低外部碎片。

配置中的 `configTOTAL_HEAP_SIZE` 决定这一区域的大小；具体数值必须由目标工程的对象数量、栈预算和测量结果确定。

```text
configTOTAL_HEAP_SIZE
          ↓
heap_4 free-block list
          ↓
pvPortMalloc / vPortFree
          ↓
Task / Queue / Semaphore / Timer objects
```

## 静态任务创建

静态任务创建由应用提供 TCB 和任务栈，避免任务创建时从 FreeRTOS heap 分配这两部分内存。它不改变其他内核对象的分配方式。

## 范围

本文档聚焦 `heap_4.c` 的分配、释放和相邻空闲块合并机制；当前默认分支没有分发任何 FreeRTOS heap 源码。

## 相关内容

- 动态/静态任务创建：[task-management.md](task-management.md)
- Heap 大小与内核功能开关：[freertos-configuration.md](freertos-configuration.md)

