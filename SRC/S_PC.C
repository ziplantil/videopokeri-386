
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include <assert.h>
#include <dos.h>
#include <conio.h>

static int beeping = 0;
static int instrument = -1;
static int last_note = 0;
static int last_divider = 0;
static int tupl = 0;
static char skipper = 0;

const unsigned short dividers[] = {
    65535, 65535, 65009, 61361, 57917, 54666,
    51598, 48702, 45968, 43388, 40953, 38655,
    36485, 34437, 32505, 30680, 28958, 27333,
    25799, 24351, 22984, 21694, 20477, 19327,
    18243, 17219, 16252, 15340, 14479, 13666,
    12899, 12175, 11492, 10847, 10238,  9664,
     9121,  8609,  8126,  7670,  7240,  6833,
     6450,  6088,  5746,  5424,  5119,  4832,
     4561,  4305,  4063,  3835,  3620,  3417,
     3225,  3044,  2873,  2712,  2560,  2416,
     2280,  2152,  2032,  1918,  1810,  1708,
     1612,  1522,  1437,  1356,  1280,  1208,
     1140,  1076,  1016,   959,   905,   854,
      806,   761,   718,   678,   640,   604,
      570,   538,   508,   479,   452,   427,
      403,   380,   359,   339,   320,   302,
      285,   269,   254,   240,   226,   214,
      202,   190,   180,   169,   160,   151
};

void beeper_on(unsigned short divider) {
    unsigned char mask;
    if (!divider) return;
    outp(0x42, divider);
    outp(0x42, divider >> 8);
    mask = inp(0x61);
    if (!(mask & 3)) {
        outp(0x61, mask | 3);
        outp(0x43, 0x86);
    }
    last_divider = divider;
    beeping = 1;
}

void beeper_off(void) {
    outp(0x61, inp(0x61) & 0xfc);
    beeping = 0;
}

void soitto_paivita_pc(void) {
    if (!beeping) return;
    switch (instrument) {
    case 3:
        beeper_on(last_divider + (skipper ? 500 : -400));
        skipper ^= 1;
        break;
    case 10:
    case 12:
        beeper_on(last_divider + 192);
        break;
    case 11:
        beeper_on(last_divider + 2048);
        break;
    case 13:
        beeper_on(last_divider ^ 2047);
        break;
    case 14:
        if (skipper++) beeper_off();
        break;
    case 15:
        beeper_on(last_divider ^ 512);
        break;
    case 16:
        beeper_on(last_divider - 32);
        break;
    case 9:
    case 18:
        beeper_on(dividers[last_note + tupl * 12]);
        tupl ^= 1;
        break;
    }
}

void soitto_komento_pc(short *c, short hwc, unsigned char k, unsigned char x) {
    if (*c) return;
    switch (k) {
    case COMMAND_NOTE_ON: {
        switch (instrument) {
        case 0:     x -= 12; break;
        case 9:     x += 24; break;
        case 18:    x += 24; break;
        case 3:     x += 24; break;
        case 13:    x += 32; break;
        case 10:    x += 60; break;
        case 11:    x += 60; break;
        case 14:    x += 60; break;
        case 15:    x += 60; break;
        case 16:    x += 60; break;
        case 12:    x += 84; break;
        }
        skipper = 0;
        last_note = x;
        beeper_on(dividers[x]);
        break;
    }
    case COMMAND_NOTE_OFF:
        beeper_off();
        break;
    case COMMAND_INSTRUMENT:
        instrument = x;
        switch (instrument) {
        case 9:
        case 18:    tupl = 0; break;
        }
        break;
    }
}

void soitto_toista_pc(int m) { }

void soitto_pysayta_pc(void) {
    beeper_off();
}

void soitto_lopeta_pc(void) {
    soitto_pysayta_pc();
}
