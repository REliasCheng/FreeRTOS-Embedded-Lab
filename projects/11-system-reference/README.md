# 多任务综合参考工程 | Integrated Reference Project

> [仓库首页](../../README.md) · 上一模块：[ISR-to-Task](../10-isr-task-communication/)

[`FreeRTOS_GD32_BalanceBall`](course/FreeRTOS_GD32_BalanceBall/) 是配套的平衡球参考工程，用于阅读多任务、多外设和中断协作的系统组织方式。

## 任务拆分

`main.c` 创建：

- input task；
- balance control task；
- OLED task；
- protocol send task；
- protocol PID task；
- protocol receive task。

## 通信关系

```text
UART ISR → receive queue → protocol receive task
protocol task → send queue → communication output
timer ISR → binary semaphore → OLED refresh task
input task / control task → application state and peripherals
```

工程使用 Queue、Binary Semaphore、UART ISR 和多个应用任务形成协作。仓库将其作为 Integrated Reference Project，分析重点是任务拆分、通信关系和中断协作。

