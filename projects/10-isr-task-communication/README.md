# 中断到任务协作 | ISR-to-Task Communication

> [仓库首页](../../README.md) · 上一模块：[Task Notifications](../09-task-notifications/) · 下一模块：[Integrated Reference](../11-system-reference/)

本目录是跨工程索引，不复制已有源码。相关实现分布在：

- [`07_FreeRTOS_operation2`](../02-task-scheduling/course/07_FreeRTOS_operation2/)：`xTaskResumeFromISR()` + 条件 `portYIELD_FROM_ISR()`；
- [Software Timers](../04-software-timers/)：`xTimerStartFromISR()` / `xTimerStopFromISR()`；
- [Semaphores](../05-semaphores/)：`xSemaphoreGiveFromISR()`；
- [Integrated Reference Project](../11-system-reference/)：UART ISR 使用 `xQueueSendFromISR()`，定时中断释放刷新信号量。

## 处理结构

![ISR-to-task flow](../../assets/images/diagram/isr-to-task-flow.svg)

ISR 负责确认硬件事件并提交最小消息；被唤醒的任务承担协议解析、显示刷新或其他后续工作。各保留工程对 `pxHigherPriorityTaskWoken` 的处理不同，精确差异见 [Cortex-M Interrupts](../../docs/cortex-m-interrupts.md)。

