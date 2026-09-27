# 任务调度 | Task Scheduling

该组工程围绕任务优先级、生命周期和状态迁移组织。

## 工程

- [`05_FreeRTOS_priorities`](course/05_FreeRTOS_priorities/)：创建不同优先级任务，观察抢占调度关系。
- [`06_FreeRTOS_operation`](course/06_FreeRTOS_operation/)：任务延时、删除、挂起和恢复。
- [`07_FreeRTOS_operation2`](course/07_FreeRTOS_operation2/)：在任务操作基础上加入 `xTaskResumeFromISR()`。

## 调度关系

```text
Ready → Running → Blocked
  ↑        ↓          │
  └── preempt ────────┘

Running/Ready ↔ Suspended
```

主线使用 `vTaskDelay()` 进入 Blocked，使用任务句柄完成挂起、恢复和删除。ISR 恢复路径检查返回值并按条件调用 `portYIELD_FROM_ISR()`。

详见 [Kernel and Scheduler](../../docs/kernel-and-scheduler.md)。

