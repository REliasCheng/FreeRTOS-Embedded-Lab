# 任务管理 | Task Management

> [仓库首页](../README.md) · [内核与调度](kernel-and-scheduler.md) · [内存管理](memory-management.md)

本文将任务生命周期拆成独立步骤，便于理解创建方式、状态迁移和句柄的作用。当前默认分支不包含实现工程。

![Task state flow](../assets/images/diagram/task-state-flow.svg)

## 创建

`xTaskCreate()` 由 FreeRTOS heap 分配栈和 TCB；`xTaskCreateStatic()` 则由应用提供任务栈和 `StaticTask_t`。

两种方式产生相同的调度对象，区别在于存储由谁提供和生命周期如何管理。

## 删除、挂起与恢复

- `vTaskDelete()` 结束指定任务或当前任务。
- `vTaskSuspend()` 将任务转入 Suspended。
- `vTaskResume()` 从任务上下文恢复任务。
- `xTaskResumeFromISR()` 从中断上下文恢复任务，并返回是否需要切换到更高优先级任务。

从 ISR 恢复任务时，应检查返回值并按端口规则决定是否调用 `portYIELD_FROM_ISR()`。

## 延时与周期任务

`vTaskDelay()` 产生相对延时；固定相位的周期任务通常需要评估 `vTaskDelayUntil()` 或等价策略。

## 优先级

任务创建时需要明确优先级，并评估高优先级任务就绪后的抢占关系。当前仓库没有运行时优先级调整的实现证据。

## 相关内容

- TCB、Tick 与抢占关系：[kernel-and-scheduler.md](kernel-and-scheduler.md)
- 动态创建对应的 `heap_4.c`：[memory-management.md](memory-management.md)
- ISR 恢复任务及切换条件：[cortex-m-interrupts.md](cortex-m-interrupts.md)

