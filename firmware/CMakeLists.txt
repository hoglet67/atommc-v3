# Copyright (c) 2021 Chris Moulang
# 
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

include(pico_extras_import.cmake)

project(AtomMMC3_project)


pico_sdk_init()

add_executable(rpipFirmware3)

pico_generate_pio_header(rpipFirmware3 ${CMAKE_CURRENT_LIST_DIR}/rpipFirmware.pio)
pico_generate_pio_header(rpipFirmware3 ${CMAKE_CURRENT_LIST_DIR}/snoop.pio)

pico_enable_stdio_uart(rpipFirmware3 1)

pico_add_extra_outputs(rpipFirmware3)

add_compile_definitions(PLATFORM=4)
add_compile_definitions(rom=)

target_sources(rpipFirmware3 
    PRIVATE rpipFirmware.c 
    PRIVATE rpipMmcio.c 
    PRIVATE snoop.c 
    PRIVATE diskio.c 
    PRIVATE atmmccore.c 
    PRIVATE atmmcwfn.c 
    PRIVATE ff.c
    PRIVATE wildcard.c)

target_link_libraries(rpipFirmware3 pico_stdlib hardware_pio hardware_dma pico_multicore hardware_irq pico_sd_card)
