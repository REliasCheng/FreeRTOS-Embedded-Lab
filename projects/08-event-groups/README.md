# 事件组 | Event Groups

> [仓库首页](../../README.md) · 上一模块：[Queue](../07-queue/) · 下一模块：[Task Notifications](../09-task-notifications/)

[`09_FreeRTOS_EventGroup`](course/09_FreeRTOS_EventGroup/) 使用事件位表达并组合多个任务条件。

## 数据关系

```text
event source A ─┐
event source B ─┼→ event bits → wait any / wait all → task
event source C ─┘
```

Event Group 适合组合状态，不携带消息载荷。等待任务可以选择任意位或全部位，并控制退出等待时是否清除目标位。

详见 [Task Communication](../../docs/task-communication.md)。

