# 任务基础 | Task Basics

四个工程建立 GD32F407VE FreeRTOS 基线，并比较动态与静态任务创建。

## 工程

- [`01_GD32_Template_FreeRTOS`](course/01_GD32_Template_FreeRTOS/)：手动组织的 FreeRTOS 工程模板。
- [`02_GD32_Template_FreeRTOS_RTE`](course/02_GD32_Template_FreeRTOS_RTE/)：Keil RTE 方式的工程模板。
- [`03_FreeRTOS_create_task`](course/03_FreeRTOS_create_task/)：`xTaskCreate()` 动态创建任务。
- [`04_FreeRTOS_create_task_static`](course/04_FreeRTOS_create_task_static/)：`xTaskCreateStatic()` 使用应用提供的 TCB 与栈。

## 数据流

```text
hardware init → create start task → create worker tasks → start scheduler
```

动态创建从 `heap_4.c` 管理的 heap 获取任务资源；静态创建由应用提供 `StaticTask_t` 和栈数组。详细关系见 [Task Management](../../docs/task-management.md) 与 [Memory Management](../../docs/memory-management.md)。

