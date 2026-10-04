# 开发环境 | Development Environment

> [仓库首页](../README.md) · [FreeRTOSConfig](freertos-configuration.md)

## 主线平台

- MCU：GD32F407VE（ARM Cortex-M4）
- 通知工程：GD32F470ZG（ARM Cortex-M4）
- RTOS：FreeRTOS V10.5.1
- IDE：Keil MDK-ARM
- Device Family Pack：GigaDevice GD32F4xx DFP
- Peripheral library：GD32 Standard Peripheral Library

当前默认分支不包含 FreeRTOS Kernel、厂商库、启动文件或 Keil 工程。以下步骤仅适用于未来从官方渠道取得依赖后创建的个人工程。

## 建立个人工程

1. 从 FreeRTOS、ARM 与芯片厂商的官方渠道取得适用版本；
2. 使用 Keil MDK-ARM 新建 `.uvprojx`；
3. 按工程中记录的 device pack 版本安装 GD32F4xx 支持包；
4. 检查 Target Device、晶振/系统时钟和 Debug Adapter；
5. 构建后再根据实际开发板选择下载与调试方式。

## 工程文件边界

未来工程需要自行配置 `.uvprojx`、启动文件、链接脚本、BSP、vendor library、FreeRTOS Kernel 与 `FreeRTOSConfig.h`。这些文件当前均未在仓库分发。

## 工程检查状态

当前默认分支只提供架构文档与自绘 SVG，没有可执行的 Keil 构建入口。构建与板端运行记录需要在未来实现、对应工具链和硬件环境中产生。

## 相关内容

- 配置项与内核行为：[freertos-configuration.md](freertos-configuration.md)

