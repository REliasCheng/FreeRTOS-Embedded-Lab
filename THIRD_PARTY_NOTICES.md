# Third-Party Notices

## Current Default Branch

The repository includes the official FreeRTOS Kernel as a Git submodule at
`third_party/FreeRTOS-Kernel`, pinned to V11.3.2 commit
`8a59a4b19bf72977819feddef87d41163ced7004`. Its upstream URL is
https://github.com/FreeRTOS/FreeRTOS-Kernel and its own MIT license is at
[`third_party/FreeRTOS-Kernel/LICENSE.md`](third_party/FreeRTOS-Kernel/LICENSE.md).
This repository's root MIT license does not reassign Kernel authorship.

The current branch distributes no CMSIS copy, GigaDevice vendor library,
Keil/RTE project, course application, system-reference project, external
image, font, or binary component. The `app/`, `include/`, `tests/`,
`CMakeLists.txt`, and CI integration files are newly authored for this
repository; no course application code was copied.

## Referenced Technologies

Documentation also refers to ARM Cortex-M, CMSIS, GD32/STM32, and Keil
concepts. Those references do not include or relicense ARM software, vendor
SDKs, or toolchain components.

## Excluded Source Material

Training projects, the historical system-reference project, vendor components, and files without established redistribution permission are excluded from the current default branch. Earlier commits remain reachable because repository history was not rewritten.
