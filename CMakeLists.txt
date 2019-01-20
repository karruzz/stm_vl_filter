cmake_minimum_required(VERSION 2.8)

project(filter C ASM)

set(ARMCPP      arm-none-eabi-g++)
set(ARMC        arm-none-eabi-gcc)
set(ARMLD       arm-none-eabi-ld)
set(ARMOBJCOPY  arm-none-eabi-objcopy)

set(CMAKE_CXX_COMPILER ${ARMCPP})
set(CMAKE_C_COMPILER   ${ARMC})
set(CMAKE_ASM_COMPILER ${ARMC})

# Compiler definitions
add_definitions(
  -DSTM32F10X_MD_VL
  -DUSE_STDPERIPH_DRIVER
)

# stm spl
set(LIB     /home/kar/Work/CortexM/STM32/lib)
set(SPL_INC "${LIB}/std_periph_lib/inc")
set(SPL_SRC "${LIB}/std_periph_lib/src")
set(CMSIS   "${LIB}/cmsis")
set(STARTUP "${LIB}/startup")
set(NOKIA   "${LIB}/periphery/nokia_5110")

include_directories(
  ${CMSIS}
  ${SPL_INC}
  ${NOKIA}
  include
)

set(SOURCES
    src/main.c
    src/periph/adc.c
    src/periph/dac.c
    src/periph/timer.c
    src/filter/utils.S
    src/filter/low_pass.c
    src/filter/fft.c
    ${STARTUP}/startup_stm32f10x_md_vl.s
    ${SPL_SRC}/system_stm32f10x.c
    ${SPL_SRC}/stm32f10x_adc.c
    ${SPL_SRC}/stm32f10x_dac.c
    ${SPL_SRC}/stm32f10x_dma.c
    ${SPL_SRC}/stm32f10x_exti.c
    ${SPL_SRC}/stm32f10x_gpio.c
    ${SPL_SRC}/stm32f10x_rcc.c
    ${SPL_SRC}/stm32f10x_spi.c
    ${SPL_SRC}/stm32f10x_tim.c
    ${SPL_SRC}/misc.c
    ${NOKIA}/nokia_5110.c
)

#    ${FREERTOS}/portable/GCC/ARM_CM3/port.c
#    ${FREERTOS}/portable/MemMang/heap_1.c
#    ${FREERTOS}/list.c
#    ${FREERTOS}/queue.c
#    ${FREERTOS}/tasks.c
#    ${FREERTOS}/timers.c


# avr-gcc: error: unrecognized command line option ‘-rdynamic’
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

# params
set(LINKER_SCRIPT "${STARTUP}/stm32_flash.ld")
#set(LIBSPEC "-L/usr/lib/arm-none-eabi/newlib -L/usr/lib/gcc/arm-none-eabi/6.3.1/thumb/v7-m")
set(LDFLAGS "-T${LINKER_SCRIPT} ${LIBSPEC} --specs=nosys.specs")

set(GCC_PARAMS "-O2 -mlittle-endian -mthumb -mcpu=cortex-m3")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LDFLAGS} -O2 -mlittle-endian -mthumb -mcpu=cortex-m3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_PARAMS}")
set(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

# build
add_executable(${PROJECT_NAME} ${SOURCES} ${LINKER_SCRIPT})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

add_custom_target(hex ALL ${ARMOBJCOPY} -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS ${PROJECT_NAME})
add_custom_target(bin ALL ${ARMOBJCOPY} -O binary "${PROJECT_NAME}.elf" "${PROJECT_NAME}.bin" DEPENDS ${PROJECT_NAME})
