
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "MUISTI.H"
#include <assert.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>

struct sid_instrument {
    unsigned char control;          /* control register */
    unsigned char volume;           /* 0-15, unused on a standard SID */
    unsigned char attack;           /* 0-15 */
    unsigned char decay;            /* 0-15 */
    unsigned char sustain;          /* 0-15 */
    unsigned char release;          /* 0-15 */
    signed char transpose;          /* note transpose */
    unsigned short pulsewidth;      /* 0-4095 */
};

static int IO_PORT = 0;
static unsigned char reg[29] = { 0 };

static unsigned short current_track = 0;
static unsigned char instruments[4] = { 0 };
static unsigned char chip_instruments[3] = { 0 };
static signed char transpose[4] = { 0 };
static signed char channelmap[8] = { -1, -1, -1, -1, -1, -1, -1 };

#define INSTR_SIZE (sizeof(struct sid_instrument))
static const struct sid_instrument _instr[];
static const int instruments_cnt;

#define SID_WRITE(r, v) do {                 \
        outp(IO_PORT + (r), reg[r] = (v));   \
    } while (0)

void reset_innova(void) {
    int i;
    for (i = 0; i < 25; ++i) {
        SID_WRITE(i, 0);
    }
}

void silence_innova(void) {
    int i;
    for (i = 0; i < 21; i += 7) {
        SID_WRITE(i, 0);
        SID_WRITE(i + 1, 0);
        SID_WRITE(i + 4, reg[i + 4] & 0xF0);
    }
}

int alusta_innova(void) {
    int s, ok;
    unsigned char a1, a2, a3, a4;
    IO_PORT = soittolaite_sidportti;
    
    reset_innova();
    /* Onkos siellä SIDiä? kokeillaas OSC3 */
    s = IO_PORT + 0x1b;
    /* OSC3 hiljaiseksi */
    SID_WRITE(0x18, reg[0x18] | 0x80);
    /* ja sitten päälle. 50 Hz (0x0937) ja pulssisuhde aivan ylös */
    SID_WRITE(0x0E, 0x37);
    SID_WRITE(0x0F, 0x09);
    SID_WRITE(0x10, 0xFF);
    SID_WRITE(0x11, 0x0F);
    SID_WRITE(0x12, 0x41);
    microsleep(30000);
    a1 = inp(s);
    microsleep(50000);
    a2 = inp(s);
    /* päittäin */
    SID_WRITE(0x10, 0x00);
    SID_WRITE(0x11, 0x00);
    microsleep(30000);
    a3 = inp(s);
    microsleep(50000);
    a4 = inp(s);
    ok = (a1 == a2) && (a3 == a4) && (a1 ^ a3);
    /* nollaa taas */
    SID_WRITE(0x12, 0x00);
    SID_WRITE(0x18, 0x0E);

    return !ok;
}

/* calibrated for SSI-2001 -- master frequency (315/88)*1e6/4 */
static const unsigned short f_table[96] = {
   /* C-0 */    0x0133, 0x0145, 0x0158, 0x016d, 0x0182, 0x0199,
   /* F#0 */    0x01b2, 0x01cb, 0x01e7, 0x0204, 0x0222, 0x0243,
   /* C-1 */    0x0265, 0x028a, 0x02b0, 0x02d9, 0x0304, 0x0332,
   /* F#1 */    0x0363, 0x0397, 0x03cd, 0x0407, 0x0444, 0x0485,
   /* C-2 */    0x04ca, 0x0513, 0x0560, 0x05b2, 0x0609, 0x0665,
   /* F#2 */    0x06c6, 0x072d, 0x079b, 0x080e, 0x0889, 0x090b,
   /* C-3 */    0x0994, 0x0a26, 0x0ac1, 0x0b64, 0x0c12, 0x0cca,
   /* F#3 */    0x0d8c, 0x0e5b, 0x0f35, 0x101d, 0x1112, 0x1216,
   /* C-4 */    0x1329, 0x144d, 0x1582, 0x16c9, 0x1824, 0x1993,
   /* F#4 */    0x1b19, 0x1cb5, 0x1e6a, 0x2039, 0x2224, 0x242b,
   /* C-5 */    0x2652, 0x2899, 0x2b03, 0x2d92, 0x3048, 0x3327,
   /* F#5 */    0x3631, 0x396a, 0x3cd4, 0x4072, 0x4447, 0x4857,
   /* C-6 */    0x4ca4, 0x5132, 0x5606, 0x5b24, 0x608f, 0x664d,
   /* F#6 */    0x6c62, 0x72d4, 0x79a8, 0x80e4, 0x888e, 0x90ad,
   /* C-7 */    0x9947, 0xa265, 0xac0d, 0xb648, 0xc11f, 0xcc9a,
   /* F#7 */    0xd8c5, 0xe5a9, 0xf351, 0xffff, 0xffff, 0xffff
};

static void note_program(unsigned char c, unsigned char x) {
    unsigned short tone;
    assert(x < 93);
    tone = f_table[x];
    c *= 7;
    SID_WRITE(c, tone & 0xFF);
    SID_WRITE(c + 1, (tone >> 8) & 0xFF);
}

static void note_on(short ch) {
    ch = 7 * ch + 4;
    SID_WRITE(ch, reg[ch] | 1);
}

static void note_off(short ch) {
    ch = 7 * ch + 4;
    SID_WRITE(ch, reg[ch] & ~1);
}

static void set_instrument(unsigned char cl, unsigned char x) {
    const struct sid_instrument *in = &_instr[x];
    chip_instruments[cl] = x;
    cl *= 7;
    SID_WRITE(cl + 2, in->pulsewidth & 0xFF);
    SID_WRITE(cl + 3, (in->pulsewidth >> 8) & 0x0F);
    SID_WRITE(cl + 4, in->control & 0xFE);
    SID_WRITE(cl + 5, ((in->attack & 0xF) << 4) | (in->decay & 0xF));
    SID_WRITE(cl + 6, ((in->sustain & 0xF) << 4) | (in->release & 0xF));
}

void soitto_paivita_innova(void) {
    short i, s;
    for (i = 0; i < 3; ++i) {
        switch (chip_instruments[i]) {
        }
    }
}

void soitto_komento_innova(short *c, short hwc,
                    unsigned char k, unsigned char x) {
    short ch = *c;
    signed char cl;
    cl = channelmap[ch];
    if (cl < 0)
        return;
    switch (k) {
    case COMMAND_NOTE_ON: {
        if (chip_instruments[cl] != instruments[ch]) {
            note_off(cl);
            set_instrument(cl, instruments[ch]);
        }
        note_program(cl, x + transpose[ch]);
        note_on(cl);
        break;
    }
    case COMMAND_NOTE_OFF:
        note_off(cl);
        break;
    case COMMAND_INSTRUMENT:
        if (x < instruments_cnt) {
            instruments[ch] = x;
            transpose[ch] = _instr[x].transpose;
        }
        if (current_track == MUSA_PAAVOITTO && ch == 1 && x == 19) {
            channelmap[1] = 1;
            channelmap[2] = -1;
        }
        break;
    }
}

static void setup_filter(int m) {
    unsigned char filter;
    unsigned short filter_freq;
    switch (m) {
    default:
        break;
    }

}

static void map_channels(int m) {
    short i = 0;
    fast_memset(channelmap, -1, sizeof(channelmap));
    switch (m) {
    case MUSA_VOITTO:
        channelmap[i++] = 0;
        channelmap[i++] = 1;
        channelmap[i++] = 0;
        channelmap[i++] = 2;
        break;
    case MUSA_PAAVOITTO:
        channelmap[i++] = 0;
        channelmap[i++] = 0;
        channelmap[i++] = 1;
        channelmap[i++] = 2;
        break;
    default:
        for (; i < 3; ++i)
            channelmap[i] = i;
    }
}

void soitto_toista_innova(int m) {
    short i;
    silence_innova();
    for (i = 0; i < 4; ++i)
        instruments[i] = chip_instruments[i] = -1;
    current_track = m;
    setup_filter(m);
    map_channels(m);
}

void soitto_pysayta_innova(void) {
    short i;
    for (i = 0; i < 3; ++i)
        note_off(i);
}

void soitto_lopeta_innova(void) {
    soitto_pysayta_innova();
    silence_innova();
    reset_innova();
}

/* 9 bytes per instrument
    0       SID control register
                NOISE, SQUARE, SAW, TRI, TEST, RINGMOD, SYNC, 0
    1       instrument volume. does nothing on a standard SID
    2       attack, 0-15
    3       decay, 0-15
    4       sustain, 0-15
    5       release, 0-15
    6       note transpose
    7       pulse width (0-4095, pulse only)
*/
static const struct sid_instrument _instr[] = {
    /*                SIDC  -V-,  A    D    S    R    TP  Pu--Wd */
    /*  0 */        { 0x40, 0xF, 0x0, 0x8, 0x0, 0x8,   0, 0x400 },
    /*  1 */        { 0x10, 0xF, 0x1, 0x6, 0xB, 0xC, -12, 0x000 },
    /*  2 */        { 0x40, 0xF, 0x0, 0x6, 0x0, 0x6,   0, 0x400 },
    /*  3 */        { 0x80, 0xF, 0x0, 0x5, 0x0, 0x5,  44, 0x000 },
    /*  4 */        { 0x40, 0xF, 0x0, 0x7, 0x0, 0x7,  12, 0x400 },
    /*  5 */        { 0x10, 0xF, 0x2, 0x3, 0xA, 0x6,   0, 0x200 },
    /*  6 */        { 0x20, 0xF, 0x1, 0xA, 0x0, 0xA,   0, 0x000 },
    /*  7 */        { 0x80, 0x9, 0x2, 0x3, 0x0, 0x3,  92, 0x800 },
    /*  8 */        { 0x80, 0xC, 0x0, 0x6, 0x0, 0x6,  72, 0x000 },
    /*  9 */        { 0x80, 0xF, 0x0, 0x0, 0xF, 0x6,  26, 0x000 },
    /* 10 */        { 0x80, 0xC, 0x3, 0x2, 0x9, 0x9,  54, 0x000 },
    /* 11 */        { 0x80, 0xF, 0x3, 0x0, 0x0, 0x0,  93, 0x000 },
    /* 12 */        { 0x80, 0xF, 0x9, 0x9, 0x0, 0x9,  80, 0x000 },
    /* 13 */        { 0x20, 0xB, 0x1, 0x1, 0x0, 0x1,  45, 0x000 },
    /* 14 */        { 0x80, 0xF, 0x0, 0x2, 0x0, 0x2,  36, 0x000 },
    /* 15 */        { 0x10, 0xF, 0x0, 0x5, 0x0, 0x5,  60, 0x000 },
    /* 16 */        { 0x40, 0xF, 0x2, 0x3, 0x0, 0x3,  69, 0x300 },
    /* 17 */        { 0x80, 0xC, 0x0, 0x6, 0x0, 0x6,  68, 0x000 },
    /* 18 */        { 0x80, 0xF, 0x0, 0x0, 0xF, 0x8,  26, 0x000 },
    /* 19 */        { 0x40, 0xE, 0x3, 0x2, 0x8, 0xC,  12, 0x400 },
    /* 20 */        { 0x40, 0xC, 0x0, 0x6, 0x0, 0x6,   0, 0x200 },
    /* 21 */        { 0x80, 0xF, 0x0, 0x4, 0x0, 0x4,  69, 0x000 },
    /* 22 */        { 0x10, 0xF, 0x0, 0x4, 0x0, 0x4,  83, 0x000 },
    /* 23 */        { 0x80, 0xF, 0x0, 0x4, 0x0, 0x4,  69, 0x000 },
    /* 24 */        { 0x40, 0xF, 0x0, 0xA, 0x0, 0xA,  12, 0x200 },
    /*                SIDC  -V-   A    D    S    R    TP  Pu-Wd */
};
static const int instruments_cnt = sizeof(_instr) / INSTR_SIZE;
