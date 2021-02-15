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


// return true if read
static inline bool isRead(uint32_t data)
{
    return !!(data & (1u << snoop_PIN_R_NW));
}


#define BUFFER_SIZE 100000/4
static uint buffer[BUFFER_SIZE];
static uint ptr=0;

static void dump_reg(uint reg) {
        uint address = (reg >> snoop_PIN_A0) & 0xF;
        uint data = (reg >> (16 + snoop_PIN_D0)) & 0xFF;
        if (!(reg & (1u << snoop_PIN_R_NW)))
        {
            printf("WRITE address=%x data=%x %x\n", address, data, reg);
        }
        else
        {
           printf("READ  address=%x data=%x %x\n", address, data, reg);
        }
}


static void process_pio(PIO pio, uint sm)
{
    dump_reg(pio_sm_get_blocking(pio, sm));
}

static void buffer_pio(PIO pio, uint sm) {
    u_int32_t reg = pio_sm_get_blocking(pio, sm);
    buffer[ptr++] = reg;
    uint address = (reg >> snoop_PIN_A0) & 0xF;
    if (ptr >= BUFFER_SIZE || address == 0xF) {
        for(uint row =0; row<ptr; row+=8) {
            printf("\n");
            uint i;
            for (i=0; i<8 && row+i<ptr; i++) {
                uint reg = buffer[row+i];
                printf(!(reg & (1u << snoop_PIN_R_NW)) ? "w" : "r");
                uint address = (reg >> snoop_PIN_A0) & 0xF;
                uint data = (reg >> (16 + snoop_PIN_D0)) & 0xFF;
                printf("%x,%02x ", address, data);
            }
            for (;i<8;i++)
            {
                printf("      ");
            }
            for (i=0; i<8 && row+i<ptr; i++) {
                uint reg = buffer[row+i];
                uint data = (reg >> (16 + snoop_PIN_D0)) & 0xFF;
                putchar( isprint(data) ? data : '.');
            }
        }
        ptr = 0;
        printf("\n");
    }
}


void snoop()
{
    PIO pio = pio0;
    uint sm = 1;
    uint offset = pio_add_program(pio, &snoop_program);
    printf("Snoop " __DATE__ " " __TIME__ "\n");
    snoop_program_init(pio, sm, offset);
    pio_sm_set_enabled(pio, sm, true);

    for (;;)
    {
        buffer_pio(pio, sm);
    }
}
