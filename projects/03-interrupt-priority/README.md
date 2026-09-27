# 中断优先级 | Interrupt Priority

> [仓库首页](../../README.md) · 上一模块：[Task Scheduling](../02-task-scheduling/) · 下一模块：[Software Timers](../04-software-timers/)

[`01_FreeRTOS_interrupt`](course/01_FreeRTOS_interrupt/) 连接 Cortex-M NVIC 优先级和 FreeRTOS 可调用中断 API 的边界。

## 关键关系

```text
NVIC priority
      ↓
configMAX_SYSCALL_INTERRUPT_PRIORITY
      ↓
FromISR API eligibility
```

工程保留 `configKERNEL_INTERRUPT_PRIORITY`、`configMAX_SYSCALL_INTERRUPT_PRIORITY` 和中断入口配置。高于 FreeRTOS 系统调用边界的中断不能调用内核 `FromISR` API。

详见 [Cortex-M Interrupts](../../docs/cortex-m-interrupts.md)。

