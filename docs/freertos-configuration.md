# FreeRTOSConfig 设计关系 | Configuration

> [仓库首页](../README.md) · [内核与调度](kernel-and-scheduler.md) · [中断边界](cortex-m-interrupts.md) · [内存管理](memory-management.md)

`FreeRTOSConfig.h` 把硬件时钟、内核行为和可用 API 连接起来。下表给出设计审查时应确认的代表关系，不表示当前仓库包含对应配置文件。

| 配置项 | 示例取值 | 内核行为与工程影响 |
| --- | --- | --- |
| `configCPU_CLOCK_HZ` | `SystemCoreClock` | 为 Cortex-M port 的时基配置提供 CPU 时钟基准 |
| `configTICK_RATE_HZ` | `1000` | 形成 1 ms scheduler time base，影响 delay 分辨率和 Software Timer 的时间基准 |
| `configMAX_PRIORITIES` | 多数为 `32`；Notify 工程为 `5` | 限定可用任务优先级范围，任务创建参数必须落在该范围内 |
| `configMINIMAL_STACK_SIZE` | 多数为 `130` words | 作为 Idle Task 和部分任务栈尺寸的基准；RTE 配置存在不同取值 |
| `configTOTAL_HEAP_SIZE` | `75 * 1024` | 决定 `heap_4.c` 可供动态任务和内核对象分配的总空间 |
| `configUSE_PREEMPTION` | `1` | 高优先级任务进入 Ready 后可触发抢占调度 |
| `configUSE_MUTEXES` | `1` | 编译 Mutex 相关接口 |
| `configUSE_COUNTING_SEMAPHORES` | `1` | 编译 Counting Semaphore 接口 |
| `configUSE_TIMERS` | `1` | 编译 Software Timer，并创建 Timer Service Task 与命令队列 |
| `configKERNEL_INTERRUPT_PRIORITY` | lowest priority 经位移 | 设置 SysTick/PendSV 等内核异常优先级 |
| `configMAX_SYSCALL_INTERRUPT_PRIORITY` | library priority `5` 经位移 | 划定可以调用 `FromISR` API 的中断优先级边界 |

## Time Slicing 与 Tickless

RTE 模板显式配置了部分调度选项；普通主线配置并非都显式定义 `configUSE_TIME_SLICING`。Tickless 相关宏只出现在部分 RTE 配置中，本仓库的代表工程不包含独立的 Tickless 低功耗流程。

## 配置检查路径

```text
hardware clock
      ↓
configCPU_CLOCK_HZ / configTICK_RATE_HZ
      ↓
kernel timing and scheduler
      ↓
task delay and timer behavior
```

```text
NVIC priority
      ↓
configMAX_SYSCALL_INTERRUPT_PRIORITY
      ↓
whether ISR may call FreeRTOS FromISR API
```

## 配置差异边界

具体取值必须随目标 MCU、FreeRTOS 版本、端口层和应用负载重新核对，不能把本文示例外推为可构建配置或性能证明。

## 相关内容

- Tick、Priority 与 Context Switch：[kernel-and-scheduler.md](kernel-and-scheduler.md)
- NVIC Priority 与 `FromISR` API：[cortex-m-interrupts.md](cortex-m-interrupts.md)
- `heap_4.c` 的分配与释放：[memory-management.md](memory-management.md)

