# 内核与调度 | Kernel and Scheduler

本仓库的主线工程采用 FreeRTOS V10.5.1。任务不是顺序执行的函数集合，而是由内核维护状态、优先级、栈和调度关系的执行单元。

## 任务与 TCB

创建任务时，内核为任务准备栈并建立 Task Control Block（TCB）。TCB 保存任务栈位置、优先级、状态列表项和调度所需信息。动态创建工程调用 `xTaskCreate()`；静态创建工程由调用者提供 `StaticTask_t` 与栈数组，再调用 `xTaskCreateStatic()`。

## 状态与调度

![Task state flow](../assets/images/diagram/task-state-flow.svg)

- **Ready**：等待 CPU 的可运行任务。
- **Running**：当前占用 CPU 的任务。
- **Blocked**：等待延时到期、队列数据、信号量或其他事件。
- **Suspended**：由 `vTaskSuspend()` 主动移出就绪调度，直到被恢复。

主线配置启用 `configUSE_PREEMPTION = 1`。更高优先级任务进入 Ready 状态后可以抢占当前任务；同优先级轮转是否显式配置取决于具体工程，不能用一个配置文件代表所有工程。

## Tick、Idle 与上下文切换

代表工程使用 `configTICK_RATE_HZ = 1000`，即 1 ms tick。`vTaskDelay()` 将任务放入 Blocked 状态，tick 到期后重新进入 Ready。Idle Task 在没有其他可运行任务时执行，也负责回收已经删除任务的资源。

Cortex-M 端口将系统异常映射为：

```text
SysTick_Handler  → xPortSysTickHandler
PendSV_Handler   → xPortPendSVHandler
SVC_Handler      → vPortSVCHandler
```

SysTick 更新内核时基，PendSV 在低异常优先级下完成任务上下文切换，SVC 参与调度器启动。中断优先级关系见 [cortex-m-interrupts.md](cortex-m-interrupts.md)。

## 对应工程

- [Task Basics](../projects/01-task-basics/)
- [Task Scheduling](../projects/02-task-scheduling/)
- [Interrupt Priority](../projects/03-interrupt-priority/)

