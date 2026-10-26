# 开发环境 | Development Environment

> [仓库首页](../README.md) · [FreeRTOSConfig](freertos-configuration.md)

## 当前可复现 Host 工程

当前仓库使用固定的官方 FreeRTOS V11.3.2 Kernel。Linux 构建选官方 `GCC_POSIX` Port，Windows/MinGW 构建选官方 `MSVC_MINGW` Port；CMake、Demo、测试和限制见[运行工程说明](rtos-runtime-foundation.md)。这些 Host 路径不需要 GD32/STM32 厂商库，也不产生目标固件。

## 历史 Cortex-M 参考平台

- MCU：GD32F407VE（ARM Cortex-M4）
- 通知工程：GD32F470ZG（ARM Cortex-M4）
- RTOS：FreeRTOS V10.5.1
- IDE：Keil MDK-ARM
- Device Family Pack：GigaDevice GD32F4xx DFP
- Peripheral library：GD32 Standard Peripheral Library

当前默认分支通过 Git 子模块引入官方 FreeRTOS Kernel，但不包含厂商库、启动文件或 Keil 工程。以下步骤仅适用于未来从官方渠道取得目标依赖后创建的个人工程。

## 建立个人工程

1. 从 FreeRTOS、ARM 与芯片厂商的官方渠道取得适用版本；
2. 使用 Keil MDK-ARM 新建 `.uvprojx`；
3. 按工程中记录的 device pack 版本安装 GD32F4xx 支持包；
4. 检查 Target Device、晶振/系统时钟和 Debug Adapter；
5. 构建后再根据实际开发板选择下载与调试方式。

## 工程文件边界

未来目标工程需要自行配置 `.uvprojx` 或其他构建入口、启动文件、链接脚本、BSP、vendor library、相应 Cortex-M FreeRTOS Port 与目标专用 `FreeRTOSConfig.h`。当前 Host 配置不能直接充当目标配置。

## 工程检查状态

当前默认分支已有可执行的 Host Kernel 工程，但没有可执行的 Keil 或其他 Cortex-M 目标构建入口。目标构建与板端运行记录需要在未来匹配的工具链和硬件环境中产生。

## 相关内容

- 配置项与内核行为：[freertos-configuration.md](freertos-configuration.md)

