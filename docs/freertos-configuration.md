# FreeRTOSConfig 设计关系 | Configuration

`FreeRTOSConfig.h` 把硬件时钟、内核行为和可用 API 连接起来。下表基于仓库代表工程，不把单个工程的取值外推为所有平台的统一配置。

| 配置项 | 代表取值 | 内核行为与工程影响 |
| --- | --- | --- |
| `configCPU_CLOCK_HZ` | `SystemCoreClock` | 为内核端口提供 CPU 时钟基准 |
| `configTICK_RATE_HZ` | `1000` | 1 ms tick，影响延时分辨率与 tick 中断频率 |
| `configMAX_PRIORITIES` | 多数为 `32`；Notify 工程为 `5` | 决定任务优先级数量 |
| `configMINIMAL_STACK_SIZE` | 多数为 `130` words | Idle Task 及部分任务栈尺寸基准；RTE 配置存在不同取值 |
| `configTOTAL_HEAP_SIZE` | `75 * 1024` | `heap_4.c` 可管理的动态内存总量 |
| `configUSE_PREEMPTION` | `1` | 启用抢占式调度 |
| `configUSE_MUTEXES` | `1` | 编译 Mutex 相关接口 |
| `configUSE_COUNTING_SEMAPHORES` | `1` | 编译 Counting Semaphore 接口 |
| `configUSE_TIMERS` | `1` | 创建 Timer Service Task 与命令队列 |
| `configKERNEL_INTERRUPT_PRIORITY` | lowest priority 经位移 | 设置 SysTick/PendSV 等内核异常优先级 |
| `configMAX_SYSCALL_INTERRUPT_PRIORITY` | library priority `5` 经位移 | 划定可以调用 `FromISR` API 的中断优先级边界 |

## Time Slicing 与 Tickless

RTE 模板显式配置了部分调度选项；普通主线配置没有在每个文件中显式定义 `configUSE_TIME_SLICING`。Tickless 相关宏只在部分 RTE 配置中出现，仓库没有独立低功耗验证工程，因此不作为核心实践结论。

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

