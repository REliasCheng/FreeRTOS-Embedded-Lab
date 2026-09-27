# Third-Party Notices

`projects/**/course/` 保存筛选后的配套参考工程，原有源码、版权头和许可证保持不变。仓库根目录的 MIT License 仅适用于本仓库新增的原创文档、图示与后续明确标注的个人代码，不覆盖这些参考工程及其第三方组件。

主要第三方内容包括：

- FreeRTOS Kernel V10.5.1 及其 portable layer，遵循工程内保留的许可证与版权声明；
- GigaDevice GD32F4xx device support、标准外设库、启动文件和 CMSIS 组件，归其原权利人所有；
- Keil 工程定义与厂商 pack/RTE 配置，按各自工具和组件条款使用；
- `projects/11-system-reference/` 中的平衡球工程作为综合参考工程保留。

迁移文件的来源、目标路径和 SHA256 记录在 [SOURCE_SELECTION_MANIFEST.csv](SOURCE_SELECTION_MANIFEST.csv)；迁移后的复核结果见 [MIGRATION_HASH_VERIFICATION.csv](MIGRATION_HASH_VERIFICATION.csv)。

