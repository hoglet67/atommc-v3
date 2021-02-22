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
#include "snoop.pio.h"

#define BUFFER_SIZE (1024 * 100 / 4)
static uint buffer[BUFFER_SIZE];
static uint ptr = 0;
static uint count = 0;

static PIO pio = pio0;
static uint sm = 1;

static void irq_handler()
{
    while (!pio_sm_is_rx_fifo_empty(pio, sm))
    {
        u_int32_t reg = pio_sm_get_blocking(pio, sm);
        buffer[ptr++] = reg;
        count += 1;

        if (ptr >= BUFFER_SIZE)
        {
            ptr = 0;
        }
    }
}

static void show_buffer()
{
    int start;
    if (count > BUFFER_SIZE)
    {
        start = ptr;
        count = BUFFER_SIZE;
    }
    else
    {
        start = 0;
    }

    for (uint row = 0; row < count; row += 8)
    {
        printf("\n%04x ", row);
        uint i;
        for (i = 0; i < 8 && row + i < count; i++)
        {
            uint reg = buffer[(start + row + i) % BUFFER_SIZE];
            printf(!(reg & (1u << snoop_PIN_R_NW)) ? "w" : "r");
            uint address = (reg >> snoop_PIN_A0) & 0xF;
            uint data = (reg >> (16 + snoop_PIN_D0)) & 0xFF;
            printf("%x,%02x ", address, data);
        }
        for (; i < 8; i++)
        {
            printf("      ");
        }
        for (i = 0; i < 8 && row + i < count; i++)
        {
            uint reg = buffer[(start + row + i) % BUFFER_SIZE];
            uint data = (reg >> (16 + snoop_PIN_D0)) & 0xFF;
            putchar(isprint(data) ? data : '.');
        }
    }
    ptr = 0;
    count = 0;
    printf("\n");
}

void snoop()
{
    uint offset = pio_add_program(pio, &snoop_program);
    snoop_program_init(pio, sm, offset);
    pio_sm_set_enabled(pio, sm, true);

    pio->inte0 = PIO_IRQ0_INTE_SM1_RXNEMPTY_BITS;
    irq_set_exclusive_handler(PIO0_IRQ_0, irq_handler);
    irq_set_priority(PIO0_IRQ_0, 0x00);
    irq_set_enabled(PIO0_IRQ_0, true);

    printf("Snoop " __DATE__ " " __TIME__ "\n");
    printf("d - dump\n");

    for (;;)
    {
        char c = getchar();
        if (c == 'd')
        {
            show_buffer();
        }
    }
}
