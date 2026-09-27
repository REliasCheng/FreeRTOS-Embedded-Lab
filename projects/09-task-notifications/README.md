# 任务通知 | Task Notifications

[`10_FreeRTOS_Notify`](course/10_FreeRTOS_Notify/) 基于 GD32F470ZG 演示直接写入目标任务 TCB 的通知机制。

## 覆盖内容

- `xTaskNotifyGive()`：计数式通知；
- `xTaskNotify()` + `eSetValueWithOverwrite`：覆盖写值；
- `eSetValueWithoutOverwrite`：非覆盖写值；
- `eSetBits`：设置事件位；
- `xTaskNotifyWait()`：等待并读取通知值。

Task Notification 适合接收方固定的轻量事件或 32-bit 数据通道。详见 [Task Communication](../../docs/task-communication.md)。

