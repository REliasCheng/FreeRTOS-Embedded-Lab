# FreeRTOS 实时系统实验室
## FreeRTOS Embedded Lab

基于 **GD32F407VE / ARM Cortex-M4** 与 **FreeRTOS V10.5.1**，围绕任务调度、通信同步、中断协作与实时系统结构构建的嵌入式 RTOS 工程仓库。

![FreeRTOS system stack](assets/images/architecture/freertos-system-stack.svg)

**Platform** `GD32F407VE / GD32F470ZG` · **Kernel** `FreeRTOS V10.5.1` · **Toolchain** `Keil MDK-ARM` · **Projects** `17 mainline + 1 reference`

## 👋 项目简介 | Overview

17 个主线 Keil 工程从 Task 创建与调度出发，逐步进入 Software Timer、Semaphore、Mutex、Queue、Event Group、Task Notification 和 ISR-to-Task。工程按 RTOS 机制组织，原始文件保存在各模块的 `course/` 目录；平衡球工程作为多任务综合参考，用于阅读任务拆分、通信和外设协作结构。

这里关注 **Cortex-M 如何支撑 FreeRTOS 调度，以及任务如何通过 IPC 与同步机制组成嵌入式系统**。GPIO、UART、SPI、ADC 等外设基础由前一阶段的 ARM 固件仓库承担。

## ⚙ 技术范围 | Technical Scope

### 🧠 Kernel & Scheduling

`Task` · `Priority` · `Ready / Running / Blocked / Suspended` · `Preemption` · `Tick` · `Context Switch`

### 🔄 Communication

`Queue` 传递数据，`Event Group` 组合事件状态，`Task Notification` 提供面向指定任务的轻量通知通道。

### 🔒 Synchronization

`Binary Semaphore` · `Counting Semaphore` · `Mutex` · `Recursive Mutex` · `Critical Section`

### ⚡ Interrupt Collaboration

`SysTick` · `PendSV` · `SVC` · `NVIC Priority` · `FromISR API` · `ISR-to-Task`

### 🧩 Memory

主线工程实际采用 `heap_4.c`：支持 Allocation、Free 与相邻空闲块合并。其他 heap 方案未作为独立实践扩展。

## 🧠 内核与调度 | Kernel & Scheduling

![Task state and scheduler flow](assets/images/diagram/task-state-flow.svg)

`SysTick` 提供 1 kHz 内核时基；就绪任务按优先级参与抢占式调度；`PendSV` 在 Cortex-M 端口中完成上下文切换。延时或等待事件的任务进入 Blocked，挂起的任务进入 Suspended，直到对应条件使其重新进入 Ready。

[内核与调度](docs/kernel-and-scheduler.md) · [任务管理](docs/task-management.md) · [FreeRTOSConfig](docs/freertos-configuration.md)

## ⚡ 中断与任务协作 | ISR-to-Task

![ISR-to-task paths](assets/images/diagram/isr-to-task-flow.svg)

保留工程包含三类真实路径：

- `xTaskResumeFromISR()` 恢复指定任务，并按返回值决定是否 `portYIELD_FROM_ISR()`；
- `xSemaphoreGiveFromISR()` 将硬件事件交给等待任务；
- `xQueueSendFromISR()` 将 UART 接收数据送入任务侧队列。

Software Timer 工程还通过 `xTimerStartFromISR()` / `xTimerStopFromISR()` 向 Timer Service Task 提交命令。各工程是否请求即时切换以源码实际传入的 `pxHigherPriorityTaskWoken` 参数为准。

[Cortex-M 中断边界](docs/cortex-m-interrupts.md) · [ISR-to-Task 工程索引](projects/10-isr-task-communication/)

## 🚀 核心工程 | Featured Projects

### [Task Management](projects/01-task-basics/)

动态/静态任务创建及任务存储方式。

**Keywords:** `Task` / `TCB` / `Stack` / `heap_4`

### [Scheduler & Task State](projects/02-task-scheduling/)

优先级、Tick 延时、删除、挂起、恢复与中断恢复任务。

**Keywords:** `Scheduler` / `Priority` / `Preemption` / `State`

### [Interrupt Priority](projects/03-interrupt-priority/)

NVIC 优先级与 FreeRTOS 系统调用边界。

**Keywords:** `NVIC` / `configMAX_SYSCALL_INTERRUPT_PRIORITY` / `FromISR`

### [Software Timer](projects/04-software-timers/)

Timer Service Task、回调与 ISR 控制路径。

**Keywords:** `Software Timer` / `Command Queue` / `FromISR`

### [Semaphore & Mutex](projects/05-semaphores/)

事件同步、资源计数和共享资源互斥。

**Keywords:** `Semaphore` / [`Mutex`](projects/06-mutex/) / `Priority Inheritance`

### [Queue, Event & Notification](projects/07-queue/)

任务间数据、事件位与直接通知。

**Keywords:** `Queue` / [`Event Group`](projects/08-event-groups/) / [`Task Notification`](projects/09-task-notifications/)

### [Integrated Reference Project](projects/11-system-reference/)

多任务、Queue、Semaphore、UART ISR 与多外设协作的综合参考结构。

**Keywords:** `Multi-task` / `IPC` / `ISR` / `Driver Coordination`

## 📂 工程结构 | Repository Structure

```text
Task Basics
    ↓
Scheduling & Task State
    ↓
Interrupt Priority & Software Timer
    ↓
Semaphore / Mutex
    ↓
Queue / Event Group / Notification
    ↓
ISR-to-Task
    ↓
Integrated Reference System
```

```text
FreeRTOS-Embedded-Lab/
├── assets/images/              # 自绘架构图、机制图与硬件参考图
├── docs/                       # 调度、通信、同步、中断和配置说明
├── projects/                   # 17 个主线工程 + 1 个综合参考工程
│   ├── 01-task-basics/
│   ├── 02-task-scheduling/
│   ├── ...
│   ├── 10-isr-task-communication/
│   └── 11-system-reference/
├── SOURCE_SELECTION_MANIFEST.csv
└── MIGRATION_HASH_VERIFICATION.csv
```

完整入口见 [`projects/`](projects/)。每个技术模块通过 README 解释机制和工程关系，`course/` 保留迁移工程本体。

## 🛠 开发环境 | Development Environment

- MCU：GD32F407VE / GD32F470ZG（ARM Cortex-M4）
- IDE：Keil MDK-ARM
- Device support：GigaDevice GD32F4xx DFP
- Peripheral library：GD32 Standard Peripheral Library
- RTOS：FreeRTOS V10.5.1

当前环境未提供 `UV4.exe`，18 个工程尚未执行自动化 Keil 构建。工程入口、迁移哈希和 Markdown 资源已经检查；构建与板端运行结果需在对应工具链和硬件环境中单独记录。

详见 [开发环境](docs/development-environment.md)。

## 📖 技术文档 | Documentation

- [Kernel and Scheduler](docs/kernel-and-scheduler.md) — Task 状态、Tick 与上下文切换
- [Task Management](docs/task-management.md) — 创建、删除、挂起与恢复
- [Cortex-M Interrupts](docs/cortex-m-interrupts.md) — 异常、NVIC 与 `FromISR` 边界
- [Task Communication](docs/task-communication.md) — Queue、Event Group 与 Notification
- [Synchronization](docs/synchronization.md) — Semaphore、Mutex 与 Critical Section
- [Memory Management](docs/memory-management.md) — `heap_4.c` 与静态任务资源
- [FreeRTOS Configuration](docs/freertos-configuration.md) — 配置项、内核行为与工程影响
- [Development Environment](docs/development-environment.md) — 平台、工程入口与验证状态

## 📜 来源与许可 | License

来源与第三方组件说明见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。仓库新增的原创文档和图示采用 [MIT License](LICENSE)；`projects/**/course/` 中的工程及第三方组件遵循其原有声明。

