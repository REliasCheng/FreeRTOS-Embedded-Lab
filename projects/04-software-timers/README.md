# 软件定时器 | Software Timers

> [仓库首页](../../README.md) · 上一模块：[Interrupt Priority](../03-interrupt-priority/) · 下一模块：[Semaphores](../05-semaphores/)

[`02_FreeRTOS_timer`](course/02_FreeRTOS_timer/) 创建软件定时器并通过 Timer Service Task 执行回调。

## 实现

- `xTimerCreate()` 建立软件定时器；
- 回调函数在 Timer Service Task 上下文运行；
- 中断路径调用 `xTimerStartFromISR()` 与 `xTimerStopFromISR()` 发送控制命令。

```text
ISR / task → timer command queue → timer service task → callback
```

保留工程的 `FromISR` 调用将 `pxHigherPriorityTaskWoken` 参数设为 `NULL`，因此这条代码路径只提交 Timer Command，不请求基于该参数的即时上下文切换。

