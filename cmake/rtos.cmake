set(FREERTOS_DIR ${CMAKE_SOURCE_DIR}/rtos)

# Make sure that git submodule is initialized and updated
if (NOT EXISTS "${FREERTOS_DIR}")
    message(FATAL_ERROR "FreeRTOS not found.")
endif()

set(FREERTOS_INC_DIR
        ${FREERTOS_DIR}/include
        ${FREERTOS_DIR}/portable/GCC/ARM_CM3
        )

include_directories(
        ${FREERTOS_DIR}/config # FreeRTOSConfig.h
        ${FREERTOS_INC_DIR}
)

set(FREERTOS_LIB_SRC
        ${FREERTOS_DIR}/portable/GCC/ARM_CM3/port.c
        ${FREERTOS_DIR}/portable/MemMang/heap_4.c
        ${FREERTOS_DIR}/croutine.c
        ${FREERTOS_DIR}/event_groups.c
        ${FREERTOS_DIR}/list.c
        ${FREERTOS_DIR}/queue.c
        ${FREERTOS_DIR}/readme.txt
        ${FREERTOS_DIR}/stream_buffer.c
        ${FREERTOS_DIR}/tasks.c
        ${FREERTOS_DIR}/timers.c
        )

set_source_files_properties(${PLATFORM_ARCH}
        PROPERTIES COMPILE_FLAGS ${DEVICE}
        # DEVICE are defined in the top level CMakeLists.txt
        )

add_library(freertos STATIC ${FREERTOS_LIB_SRC})

set_target_properties(freertos PROPERTIES LINKER_LANGUAGE C
                            C_STANDARD 11)

set(EXTERNAL_LIBS ${EXTERNAL_LIBS} freertos)