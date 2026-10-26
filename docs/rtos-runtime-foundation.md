# FreeRTOS Host runtime foundation

This project runs the **official FreeRTOS Kernel V11.3.2** at commit
`8a59a4b19bf72977819feddef87d41163ced7004`. The Kernel is a Git submodule
from [FreeRTOS/FreeRTOS-Kernel](https://github.com/FreeRTOS/FreeRTOS-Kernel),
licensed under its own [MIT license](../third_party/FreeRTOS-Kernel/LICENSE.md).
The application, configuration, tests, and integration are repository-authored.
The historical V10.5.1 course projects are technical references only; their
application or board code was not copied.

## Actual execution path

```text
Official FreeRTOS Kernel + GCC_POSIX (Linux) / MSVC_MINGW (Windows)
  reporter task (priority 3): counted notification wait, shared statistics
  consumer task (priority 2): xQueueReceive, validation, hysteresis processing
  producer task (priority 1): xQueueSend of synthetic 12-bit samples
  Queue: four SampleInput values, FIFO, element size sizeof(SampleInput)
  Mutex: protects the statistics and result array shared by workers/reporter
  Start gate: counting semaphore blocks workers until notification timeout test
```

`main` creates the Queue, Mutex, start gate, and all three tasks with checked results,
then calls `vTaskStartScheduler()`. Failed creation deletes any resources
already created and returns a failure code. The dynamic allocations use the
official `heap_4.c` with a 64 KiB FreeRTOS heap; malloc and stack-overflow
hooks terminate the process with failure. Stack sizes are 1024 FreeRTOS stack
units per application task. These are Host test settings, not measured target
budgets.

The reporter first confirms both a zero-wait and a positive three-tick
Task Notification wait return zero while workers are held by the start gate.
It then releases two permits so the workers can run. This gate makes the
timeout assertion independent of host thread timing.

The producer sends values through the FreeRTOS Queue; the consumer processes
them in FIFO order and calls `xTaskNotifyGive()` for each valid result and
once at completion. The producer also notifies completion. The reporter starts
first, proves an initially empty notification count, then sums values returned
by `ulTaskNotifyTake(pdTRUE, bounded_wait)`. The sum checks count semantics
even if several notifications arrive before one wakeup. It takes the Mutex to
inspect a snapshot and checks every expected result and statistic. It also
holds the Mutex while attempting a zero-wait nested acquisition to verify
that an unavailable resource does not mutate statistics. Each successful
acquisition has a matching release.

Workers call `vTaskDelete(NULL)` after their completion notification. The
reporter waits for the worker task count to fall, deletes the Queue, Mutex,
and start gate, prints a diagnostic record, and exits the one-shot Host process with an
assertion-derived status. A 15-second CTest limit and a 10-second CI command
limit bound hangs if a Kernel or IPC defect prevents completion. This process
exit strategy is specific to the POSIX/MinGW simulators and is not a target
firmware shutdown pattern.

## Three executable scenarios

| Scenario | Queue and processing behavior | Required assertion |
| --- | --- | --- |
| `normal` | Producer sends 2600, 2000, 1400; consumer produces HIGH, HIGH, LOW | 3 sent, 3 received, 3 ordered results, 5 notifications |
| `backpressure` | Four entries fill the Queue before scheduling; a fifth zero-wait send fails. Entry 3 is an invalid 4096 sample. | 4 sent/received, one rejected send, one invalid value, 3 ordered results, 5 notifications; the rejected item is not counted as sent |
| `timeout` | Producer intentionally sends no data; consumer's bounded Queue wait times out. | 0 results, one receive timeout, both workers finish, 2 notifications |

The separate `sample_processing` module checks sequence and input bounds and
applies inclusive high/low thresholds. A mid-band initial sample remains
`UNKNOWN`; invalid samples consume their ordered sequence without changing
the control level or output. The seven independent tests cover those
contracts, null/configuration errors, and integer edges. ASan and UBSan in
CI apply to this **application logic only**. The GCC/Clang Kernel jobs compile
and run the actual Kernel, tasks, Queue, notifications, and Mutex.

## Reproduce

```sh
git clone https://github.com/REliasCheng/FreeRTOS-Embedded-Lab.git
cd FreeRTOS-Embedded-Lab
git submodule update --init --recursive
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/rtos_demo normal
./build/rtos_demo backpressure
./build/rtos_demo timeout
```

On Windows with MinGW, the executable is `build/rtos_demo.exe`. CMake
selects the upstream `MSVC_MINGW` Port there and `GCC_POSIX` on Linux.
No FreeRTOS-Plus-POSIX compatibility layer is used. The submodule's Gitlink
pins the complete upstream commit; it does not follow the moving branch.

## ISR and Cortex-M boundary

The three scenarios use only FreeRTOS **task-context** API calls. They do not
call a `FromISR` function from a task and do not emulate Cortex-M hardware
interrupt timing. A future target adapter would replace the synthetic input
and Host Port while preserving the task-level data contract. It must first
select an MCU/board, verify CMSIS and vendor component provenance, configure
the Cortex-M Port and FPU ABI, validate NVIC priority and `FromISR` rules,
provide startup/linker and clock/pin contracts, and produce a real target
build. No target binary, board trace, deadline, worst-case latency, or
hardware scheduling claim is provided here.
