cmake_minimum_required(VERSION 3.20)
project(freertos_runtime_foundation C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(UNIX)
    set(FREERTOS_PORT GCC_POSIX CACHE STRING "Official FreeRTOS host port" FORCE)
elseif(MINGW)
    set(FREERTOS_PORT MSVC_MINGW CACHE STRING "Official FreeRTOS host port" FORCE)
else()
    message(FATAL_ERROR "This Host demo supports the official GCC_POSIX and MSVC_MINGW ports.")
endif()
set(FREERTOS_HEAP 4 CACHE STRING "Official FreeRTOS heap implementation" FORCE)

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(third_party/FreeRTOS-Kernel)
if(UNIX)
    # The upstream POSIX port uses robust pthread and clock interfaces.
    # Keep the feature macro confined to that port; application C stays C11.
    target_compile_definitions(freertos_kernel_port PRIVATE _GNU_SOURCE)
endif()

add_library(sample_processing STATIC app/sample_processing.c)
target_include_directories(sample_processing PUBLIC include)
target_compile_options(sample_processing PRIVATE -Wall -Wextra -Werror -pedantic)

add_executable(rtos_demo app/rtos_demo.c)
target_link_libraries(rtos_demo PRIVATE sample_processing freertos_kernel)
target_compile_options(rtos_demo PRIVATE -Wall -Wextra -Werror -pedantic)

enable_testing()
add_executable(test_processing tests/test_processing.c)
target_link_libraries(test_processing PRIVATE sample_processing)
target_compile_options(test_processing PRIVATE -Wall -Wextra -Werror -pedantic)
add_test(NAME processing COMMAND test_processing)

foreach(scenario normal backpressure timeout)
    add_test(NAME kernel_${scenario} COMMAND rtos_demo ${scenario})
    set_tests_properties(kernel_${scenario} PROPERTIES TIMEOUT 15)
endforeach()
