# 开发环境 | Development Environment

> [仓库首页](../README.md) · [FreeRTOSConfig](freertos-configuration.md) · [工程清单](../projects/)

## 主线平台

- MCU：GD32F407VE（ARM Cortex-M4）
- 通知工程：GD32F470ZG（ARM Cortex-M4）
- RTOS：FreeRTOS V10.5.1
- IDE：Keil MDK-ARM
- Device Family Pack：GigaDevice GD32F4xx DFP
- Peripheral library：GD32 Standard Peripheral Library

## 打开工程

1. 进入目标项目的 `course/<project>/Project/` 或相应工程目录；
2. 使用 Keil MDK-ARM 打开 `.uvprojx`；
3. 按工程中记录的 device pack 版本安装 GD32F4xx 支持包；
4. 检查 Target Device、晶振/系统时钟和 Debug Adapter；
5. 构建后再根据实际开发板选择下载与调试方式。

## 工程文件

迁移保留 `.uvprojx`、启动文件、链接脚本、BSP、GD32 library、FreeRTOS Kernel 与 `FreeRTOSConfig.h`。`Objects/`、`Listings/`、AXF、HEX、MAP 等构建产物不进入仓库。

## 验证范围

本地仓库已完成文件完整性、18 个 Keil 工程入口和源码 SHA256 检查。当前环境未提供 `UV4.exe`，因此：

```text
BUILD_PASS=0
BUILD_NOT_AUTOMATED=18
```

这里的“工程完整”只表示工程定义与迁移文件存在，不等同于编译通过。仓库也没有新增板端运行、串口输出或时序测量记录。

## 相关内容

- 配置项与内核行为：[freertos-configuration.md](freertos-configuration.md)
- 所有分组工程：[projects/](../projects/)

