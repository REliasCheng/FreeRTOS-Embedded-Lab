# Cortex-M 中断与 FreeRTOS | Interrupts and FreeRTOS

> [仓库首页](../README.md) · [内核与调度](kernel-and-scheduler.md) · [ISR-to-Task 工程索引](../projects/10-isr-task-communication/)

FreeRTOS 的 Cortex-M 端口建立在异常、NVIC 优先级和任务栈切换机制之上。这里关注硬件机制如何为实时调度服务，不重复 GPIO、UART 等外设基础配置。

## 异常分工

```text
Cortex-M Exception
        ↓
SysTick：更新 tick，并判断是否需要调度
        ↓
PendSV：保存当前任务上下文、选择并恢复下一任务
        ↓
Task：在线程模式继续运行
```

SVC 用于从线程模式进入特权服务并启动首个任务。端口层通过 `FreeRTOSConfig.h` 中的 handler 映射接管这三个异常入口。

## NVIC 优先级边界

代表配置使用：

```c
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY     5
```

两者经过 `configPRIO_BITS` 左移形成 `configKERNEL_INTERRUPT_PRIORITY` 与 `configMAX_SYSCALL_INTERRUPT_PRIORITY`。数值更小表示更高的硬件优先级。

- 内核异常运行在最低可配置优先级。
- 调用 FreeRTOS `FromISR` API 的中断，其优先级必须处于内核允许屏蔽和管理的范围。
- 高于该边界的紧急中断不能调用这些内核 API。

## ISR-to-Task

![ISR-to-task flow](../assets/images/diagram/isr-to-task-flow.svg)

典型的即时切换模式是：

```c
BaseType_t task_woken = pdFALSE;
/* xQueueSendFromISR / xSemaphoreGiveFromISR / ... */
portYIELD_FROM_ISR(task_woken);
```

保留源码中的实现并不完全相同：

- `07_FreeRTOS_operation2` 使用 `xTaskResumeFromISR()` 并检查返回值，再调用 `portYIELD_FROM_ISR()`；
- Semaphore、Software Timer 和平衡球参考工程的若干 `FromISR` 调用传入 `NULL`，因此未请求基于 `pxHigherPriorityTaskWoken` 的即时切换；
- 平衡球的 UART 接收路径使用 `xQueueSendFromISR()` 将字节送入接收队列。

这一区别保留在源码中，便于比较“事件已提交”和“事件提交后立即让出 CPU”两种路径。

## 对应工程

- [Interrupt Priority](../projects/03-interrupt-priority/)
- [ISR-to-Task Index](../projects/10-isr-task-communication/)
- [Integrated Reference Project](../projects/11-system-reference/)

## 相关内容

- `configKERNEL_INTERRUPT_PRIORITY` 与系统调用边界：[freertos-configuration.md](freertos-configuration.md)
- Queue、Event Group 和 Task Notification：[task-communication.md](task-communication.md)

