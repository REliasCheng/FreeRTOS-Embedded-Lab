# Source and asset decision record

The implementation was built after a bounded review of the repository's
existing documentation and historical FreeRTOS/Keil course assets. Historical
records identify FreeRTOS V10.5.1, GD32/CMSIS components, and task/Queue/
notification examples, but they do not establish redistribution rights for
the adjacent course application. No historical application, configuration,
board-support, image, startup, or project file was copied into this branch.

| Classification | Material | Decision |
| --- | --- | --- |
| `USER_ORIGINAL` | Existing repository-authored docs/SVG and new application, Host configuration, tests, CMake and CI | Retain under this repository's MIT license |
| `LICENSED_REUSABLE` | Official FreeRTOS Kernel V11.3.2 and its POSIX/MinGW ports, pinned at `8a59a4b19bf72977819feddef87d41163ced7004` | Use as a Git submodule with upstream MIT license and copyright intact |
| `REFERENCE_ONLY` | Historical task, Queue, Mutex, notification, and target project layouts | Inform the interface requirements only; no source transplant |
| `RIGHTS_UNVERIFIED` | Historical course application, reference project, board assets and images | Exclude from public implementation |
| `BUILD_EVIDENCE_ONLY` | Historical Keil/project records | Do not claim they build the new source or validate a board |

The Gitlink is the version lock. The tag name is a human-readable release
label; the complete commit SHA is authoritative. A similarly named upstream
branch points to a different commit, so cloning by `--branch V11.3.2` alone
is not a sufficient version pin. Verify the checkout with
`git submodule status`.

The Host runtime tests establish FreeRTOS task and IPC behavior on the
selected simulator ports. They do not establish target startup, Cortex-M
interrupt priority, `FromISR` correctness, physical timing, or device I/O.
