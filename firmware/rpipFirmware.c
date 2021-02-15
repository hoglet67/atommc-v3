/**
 * Copyright (c) 2021 Chris Moulang
 *
 * SPDX-License-Identifier: BSD-3-Clause
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/irq.h"
#include "pico/binary_info.h"
#include "rpipFirmware.pio.h"
#include "pico/sd_card.h"

#include "ff.h"

static const uint PIN_LED = 25;
extern void at_process();
extern void at_initprocessor();

// global variables start
char LatchedData;
int blVersion = 4;
int configByte = 0xFF;

unsigned char windowData[512] = {0};
unsigned char globalData[512] = {0};
// global variables end

static void blink(int count)
{
    for (int i = count * 2 - 1; i > 0; i--)
    {
        gpio_put(PIN_LED, i % 2);
        sleep_ms(100);
    }
    gpio_put(PIN_LED, 0);
}

// Setup the GPIO pins
static void initialiseIO()
{
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    for (int i = 0; i < 8; i++)
    {
        // PIO0 controls direction of D0-D7
        gpio_set_function(GPIO_FIRST + PIN_D0 + i, GPIO_FUNC_PIO0);
    }
}

// return true if read
static inline bool isRead(uint32_t data)
{
    return !!(data & (1u << PIN_R_NW));
}

static PIO pio = pio0;
static uint _write_address;
static uint write_data;
static uint _read_address;
static bool _was_write;

void __time_critical_func(process_pio)()
{
    for (;;)
    {
        _was_write = false;
        // do
        // {
        u_int32_t reg = pio_sm_get_blocking(pio, 0);
        if (!(reg & (1u << PIN_R_NW)))
        {
            _was_write = true;
            _write_address = (reg >> PIN_A0) & 0xF;
            write_data = (reg >> (16 + PIN_D0)) & 0xFF;
        }
        else
        {
            _read_address = (reg >> PIN_A0) & 0xF;
        }
        if (!pio_sm_is_rx_fifo_empty(pio, 0))
        {
            printf("Too slow!\n");
        }
        at_process();
    }
}

uint __time_critical_func(was_write)()
{
    return _was_write;
}

uint __time_critical_func(write_address)()
{
    return _write_address;
}

void __time_critical_func(ReadDataPort)()
{
    LatchedData = write_data;
}

void pico_led(int state)
{
    gpio_put(PIN_LED, !!state);
}

int ReadEEPROM(int addr)
{
    return 0;
}

void WriteEEPROM(unsigned char address, unsigned char val)
{
}

void __time_critical_func(WriteDataPort)(int value)
{
    pio_sm_put_blocking(pio, 0, value | 0xFF00);
};

extern void snoop();

int main()
{
    bi_decl(bi_program_description("Acorn Atom MMC/PL8 Interface" __DATE__ " " __TIME__));
    bi_decl(bi_1pin_with_name(PIN_LED, "On-board LED"));
    stdio_init_all();
    sd_init_1pin();
    sd_set_clock_divider(12 / 12);
    sd_set_wide_bus(false);

    //test_sd_card();

    initialiseIO();
    blink(5);

    printf("Acorn Atom MMC/PL8 Interface " __DATE__ " " __TIME__ "\n");

    at_initprocessor();

    uint offset = pio_add_program(pio, &test_program);
    test_program_init(pio, 0, offset);
    pio_sm_set_enabled(pio, 0, true);

    process_pio();

    panic("SHOULD NOT BE HERE");
}
