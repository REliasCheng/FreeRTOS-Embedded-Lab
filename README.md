# FreeRTOS-Embedded-Lab

基于 GD32F407VE / ARM Cortex-M4 与 FreeRTOS V10.5.1 的嵌入式系统工程实践仓库，重点展示任务调度、进程间通信、同步机制和中断到任务的数据路径。

## Overview

仓库围绕 FreeRTOS 的任务生命周期、优先级调度、Software Timer、Queue、Event Group、Task Notification、Semaphore、Mutex 和 ISR-to-Task 协作组织独立工程。

这些工程用于分析 RTOS 机制如何进入嵌入式软件结构：应用被拆分为任务，任务通过 IPC 和同步原语交换状态，硬件事件通过受约束的 `FromISR` 接口交给任务处理。仓库不据此声明确定性性能、线程安全保证或生产级实时能力。

## Platform & Technology

| Field | Value |
| --- | --- |
| Language | C |
| Platform | GD32F407VE / GD32F470ZG, ARM Cortex-M4 |
| Toolchain | Keil MDK-ARM, GigaDevice GD32F4xx DFP |
| Architecture | FreeRTOS V10.5.1, task scheduling, IPC, synchronization, ISR-to-task |
| Verification | Source and configuration review; build and hardware status are listed below |

## Architecture

![FreeRTOS system stack](assets/images/architecture/freertos-system-stack.svg)

应用任务承载业务状态和外设协作；Queue、Semaphore、Mutex、Event Group、Task Notification 与 Software Timer 提供通信和同步；FreeRTOS Kernel 管理任务状态、优先级、Tick 与上下文切换；Cortex-M 的 SysTick、PendSV、SVC 和 NVIC 形成调度与中断边界。

硬件事件进入 ISR 后，只执行必要的状态确认和数据搬运，再通过 `FromISR` API 唤醒任务或提交消息。是否触发即时上下文切换，以对应源码传入的 `pxHigherPriorityTaskWoken` 和返回值为准。

## Key Features

| Capability | Implementation Entry |
| --- | --- |
| Task management | [Task Basics](projects/01-task-basics/) 与 [Task Management](docs/task-management.md) 展示动态/静态创建、删除、挂起和恢复 |
| Scheduling and timers | [Task Scheduling](projects/02-task-scheduling/) 与 [Software Timers](projects/04-software-timers/) 展示优先级、Tick、Blocked 状态和 Timer Service Task |
| Inter-task communication | [Queue](projects/07-queue/)、[Event Groups](projects/08-event-groups/) 与 [Task Notifications](projects/09-task-notifications/) 展示数据和事件传递 |
| Synchronization mechanisms | [Semaphores](projects/05-semaphores/)、[Mutex](projects/06-mutex/) 与 [Synchronization](docs/synchronization.md) 说明事件同步和共享资源保护 |
| Interrupt-driven workflow | [ISR-to-Task Communication](projects/10-isr-task-communication/) 展示 Resume、Semaphore、Queue 和 Software Timer 的中断协作路径 |
| Embedded application structure | [System Reference](projects/11-system-reference/) 作为多任务、IPC、UART ISR 和外设协作的结构参考 |

## Project Structure

```text
FreeRTOS-Embedded-Lab/
├── projects/01-task-basics/              # Task 创建与存储方式
├── projects/02-task-scheduling/          # 优先级、状态与调度
├── projects/03-interrupt-priority/       # NVIC 与系统调用边界
├── projects/04-software-timers/          # Timer Service Task
├── projects/05-semaphores/               # 二值与计数信号量
├── projects/06-mutex/                    # Mutex 与优先级继承
├── projects/07-queue/                    # Queue 数据通信
├── projects/08-event-groups/             # 事件位组合
├── projects/09-task-notifications/       # 直接任务通知
├── projects/10-isr-task-communication/   # ISR-to-Task 路径
├── projects/11-system-reference/         # 综合结构参考
├── docs/                                 # 调度、IPC、同步和中断文档
└── assets/images/                        # 已有架构与机制图
```

## Documentation

- [Kernel and Scheduler](docs/kernel-and-scheduler.md)
- [Task Management](docs/task-management.md)
- [Cortex-M Interrupts](docs/cortex-m-interrupts.md)
- [Task Communication](docs/task-communication.md)
- [Synchronization](docs/synchronization.md)
- [Memory Management](docs/memory-management.md)
- [FreeRTOS Configuration](docs/freertos-configuration.md)
- [Development Environment](docs/development-environment.md)

## Verification

### Host Test

**Status:** Not Applicable. 仓库没有独立的 Host Test 入口。

### Build Verification

**Status:** Not Provided. Keil 工程定义存在，但仓库未提供与当前公开版本对应的可复现构建记录。

### Hardware Validation

**Status:** Not Provided. 仓库未提供可复核的 GD32F407VE / GD32F470ZG 板端验证记录。

### Runtime Evidence

**Status:** Not Provided. 仓库未提供运行日志、调度时序、延迟测量或性能测试结果。

源码和工程配置存在，不等同于构建成功、硬件验证通过或获得确定性性能保证。

## License Boundary

根目录 [MIT License](LICENSE) 仅适用于仓库维护者新增并明确覆盖的文档、图示和原创内容。FreeRTOS、CMSIS、GigaDevice 组件、Keil 工程定义及参考工程继续适用各自的版权和许可声明，具体边界见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
