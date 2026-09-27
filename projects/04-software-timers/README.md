# 软件定时器 | Software Timers

[`02_FreeRTOS_timer`](course/02_FreeRTOS_timer/) 创建软件定时器并通过 Timer Service Task 执行回调。

## 实现

- `xTimerCreate()` 建立软件定时器；
- 回调函数在 Timer Service Task 上下文运行；
- 中断路径调用 `xTimerStartFromISR()` 与 `xTimerStopFromISR()` 发送控制命令。

```text
ISR / task → timer command queue → timer service task → callback
```

保留工程的 `FromISR` 调用传入 `NULL`，没有记录由定时器命令触发的即时任务切换测量。

