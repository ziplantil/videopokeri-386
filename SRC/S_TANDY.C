
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "MUISTI.H"
#include <assert.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>

struct tandy_instrument {
    unsigned char amplitude;        /* attack amplitude */
    unsigned char attack;           /* 0 for instant, 1-7 for faster-slower */
    unsigned char decay;            /* 0 for instant, 1-7 for faster-slower */
    unsigned char sustain;          /* 0-15 amplitude */
    unsigned char release;          /* 0 for instant, 1-7 for faster-slower */
    signed char transpose;          /* note transpose */
    unsigned char noise;            /* is noise? 0 or 1 */
    unsigned char nsef;             /* noise gen flags */
};

static int IO_PORT = 0;
static unsigned char reg1[8] = { 0 };
static unsigned char reg2[8] = { 0 };

static unsigned char instruments[4] = { 0 };
static signed char transpose[4] = { 0 };
static unsigned char key_on[4] = { 0 };
static unsigned char key_peak[4] = { 0 };
static unsigned char amplitudes[4] = { 0 };
static unsigned char attack_speed[4] = { 0 };
static unsigned char attack_rate[4] = { 0 };
static unsigned char decay_speed[4] = { 0 };
static unsigned char decay_rate[4] = { 0 };
static unsigned char sustain_level[4] = { 0 };
static unsigned char release_speed[4] = { 0 };
static unsigned char release_rate[4] = { 0 };
static unsigned char counter[4] = { 0 };
static unsigned char chip_ampl[4] = { 0 };

#define INSTR_SIZE (sizeof(struct tandy_instrument))
static const struct tandy_instrument _instr[];
static const int instruments_cnt;

#define TANDY_WRITE(r, v) do {                                          \
        outp(IO_PORT, (unsigned char)(0x80 | ((r) << 4)                 \
                                        | (reg1[r] = ((v) & 15))));     \
    } while (0)

#define TANDY_WRITEW(r, v) do {                                         \
        outp(IO_PORT, (unsigned char)(0x80 | ((r) << 4)                 \
                                        | (reg1[r] = ((v) & 15))));     \
        outp(IO_PORT, (reg2[r] = ((v) >> 4) & 63));                     \
    } while (0)

void reset_tandy(void) {
    int i;
    for (i = 0; i < 8; i += 2) {
        TANDY_WRITE(i + 0, 0);
        TANDY_WRITE(i + 1, 15);
    }
}

void silence_tandy(void) {
    int i;
    for (i = 0; i < 8; i += 2) {
        TANDY_WRITE(i + 1, 15);
    }
}

int alusta_tandy(void) {
    IO_PORT = soittolaite_tandyportti;
    
    /* TODO: latches */

    reset_tandy();
    fast_memset(chip_ampl, 0, sizeof(chip_ampl));
    return 0;
}

/* calibrated for PCjr/tandy -- master frequency (315/88)*1e6 */
static const unsigned short f_table[96] = {
   /* C-0 */    0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
   /* F#0 */    0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
   /* C-1 */    0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
   /* F#1 */    0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
   /* C-2 */    0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
   /* F#2 */    0x000, 0x000, 0x000, 0x3f9, 0x3c0, 0x38a,
   /* C-3 */    0x357, 0x327, 0x2fa, 0x2cf, 0x2a7, 0x281,
   /* F#3 */    0x25d, 0x23b, 0x21b, 0x1fc, 0x1e0, 0x1c5,
   /* C-4 */    0x1ac, 0x194, 0x17d, 0x168, 0x153, 0x140,
   /* F#4 */    0x12e, 0x11d, 0x10d, 0x0fe, 0x0f0, 0x0e2,
   /* C-5 */    0x0d6, 0x0ca, 0x0be, 0x0b4, 0x0aa, 0x0a0,
   /* F#5 */    0x097, 0x08f, 0x087, 0x07f, 0x078, 0x071,
   /* C-6 */    0x06b, 0x065, 0x05f, 0x05a, 0x055, 0x050,
   /* F#6 */    0x04c, 0x047, 0x043, 0x040, 0x03c, 0x039,
   /* C-7 */    0x035, 0x032, 0x030, 0x02d, 0x02a, 0x028,
   /* F#7 */    0x026, 0x024, 0x022, 0x020, 0x01e, 0x01c
};
static const unsigned char v_table[16] = {
    15, 12,  9,  7,  6,  5,  4,  3,
     3,  2,  2,  1,  1,  1,  0,  0
};

static void note_program(unsigned char c, unsigned char x, unsigned char a) {
    unsigned short tone;
    x += 12;
    assert(x < 96);
    tone = f_table[x];
    if (!tone) {
        /* no jaa */
        a = 0;
    }
    TANDY_WRITE((c << 1) | 1, v_table[a]);
    TANDY_WRITEW(c << 1, tone);
}

static void note_off(short ch);

static void update_adsr(short ch) {
    unsigned char al = chip_ampl[ch], a = al, ta;
    if (key_on[ch]) {
        if (!key_peak[ch]) {
            ta = amplitudes[ch];
            if (!attack_rate[ch]) {
                key_peak[ch] = 1;
                a = ta;
            } else if (++counter[ch] >= attack_rate[ch]) {
                a = a + attack_speed[ch];
                if (a >= ta)
                    a = ta, key_peak[ch] = 1;
                counter[ch] = 0;
            }
        }
        if (key_peak[ch]) {
            ta = sustain_level[ch];
            if (!decay_rate[ch]) {
                a = ta;
            } else if (a > ta && ++counter[ch] >= decay_rate[ch]) {
                a = a < decay_speed[ch] ? 0 : a - decay_speed[ch];
                if (a < ta)
                    a = ta;
                counter[ch] = 0;
            }
        }
    } else if (!key_on[ch]) {
        if (!a)
            return;
        if (!release_rate[ch]) {
            a = 0;
        } else if (++counter[ch] >= release_rate[ch]) {
            a = a < release_speed[ch] ? 0 : a - release_speed[ch];
            counter[ch] = 0;
        }
    }
    if (a != al) {
        a &= 15;
        TANDY_WRITE((ch << 1) | 1, v_table[a]);
        if (a && !al) {
            counter[ch] = 0;
        } else if (!a && al) {
            note_off(ch);
            counter[ch] = 0;
        }
        chip_ampl[ch] = a;
    }
}

void soitto_paivita_tandy(void) {
    short i, s;
    for (i = 0; i < 4; ++i) {
        update_adsr(i);
    }
}

#define AD_R_PARAM(ch, type, value) do {                                    \
        assert(value < 8);                                                  \
        if (value == 0)                                                     \
            type##_rate[ch] = 0;                                            \
        else {                                                              \
            type##_speed[ch] = value < 4 ? 5 - value : 1;                   \
            type##_rate[ch] = value > 4 ? value - 3 : 1;                    \
        }                                                                   \
    } while (0);

static void note_off(short ch) {
    if (ch < 3)
        TANDY_WRITEW(ch << 1, 0);
    switch (instruments[ch]) {
    case 10:
        TANDY_WRITEW(4, 0);
        break;
    }
}

void soitto_komento_tandy(short *c, short hwc,
                    unsigned char k, unsigned char x) {
    short ch = *c;
    assert(ch < 4);
    switch (k) {
    case COMMAND_NOTE_ON: {
        if (ch < 3)
            note_program(ch, x + transpose[ch], 0);
        switch (instruments[ch]) {
        case 10:
            note_program(2, x + 76, 0);
            break;
        }
        key_on[ch] = 1;
        key_peak[ch] = 0;
        counter[ch] = 0;
        break;
    }
    case COMMAND_NOTE_OFF:
        key_on[ch] = 0;
        break;
    case COMMAND_INSTRUMENT:
        if (x < instruments_cnt) {
            const struct tandy_instrument *in = &_instr[x];
            
            note_off(ch);
            if (in->noise) {
                if (ch != 3)
                    *c = ch = 3;
                TANDY_WRITE(ch << 1, in->nsef & 7);
            } else if (ch >= 3) {
                *c = ch = hwc % 3;
            }

            instruments[ch] = x;
            amplitudes[ch] = in->amplitude;
            transpose[ch] = in->transpose;

            counter[ch] = 0;
            AD_R_PARAM(ch, attack, in->attack);
            AD_R_PARAM(ch, decay, in->decay);
            sustain_level[ch] = in->sustain;
            AD_R_PARAM(ch, release, in->release);

            if (!in->sustain && !in->decay)
                sustain_level[ch] = in->amplitude;

            key_on[ch] = chip_ampl[ch] = 0;
        }
        break;
    }
}

void soitto_toista_tandy(int m) {
    silence_tandy();
}

void soitto_pysayta_tandy(void) {
    fast_memset(key_on, 0, sizeof(key_on));
}

void soitto_lopeta_tandy(void) {
    soitto_pysayta_tandy();
    silence_tandy();
    reset_tandy();
}

/* 8 bytes per instrument
    0       (attack) amplitude
    1       attack. 0 for instant, 1-7 for faster-slower
    2       decay. 0 for instant, 1-7 for faster-slower
                if sustain = 0, decay = 0, ADSR disabled
    3       sustain. 0-15
    4       release. 0 for instant, 1-7 for faster-slower
    5       note transpose
    6       is noise? 0 = tone, 1 = noise
    7       SN76489 noise flags (0-2 perid, +4 = white, +0 = periodic)
*/
static const struct tandy_instrument _instr[] = {
    /*                VOL  A  D   S   R   TP  N? N-F */
    /*  0 */        { 0xD, 0, 4, 0x0, 2,   0, 0, 0x0 },
    /*  1 */        { 0xA, 0, 0, 0xA, 6,   0, 0, 0x0 },
    /*  2 */        { 0xD, 0, 4, 0x0, 2,   0, 0, 0x0 },
    /*  3 */        { 0xE, 0, 3, 0x0, 3,  12, 1, 0x4 },
    /*  4 */        { 0xF, 0, 3, 0x0, 2,  12, 0, 0x0 },
    /*  5 */        { 0x8, 0, 0, 0x8, 6,   0, 0, 0x0 },
    /*  6 */        { 0xF, 0, 6, 0x0, 6, -12, 0, 0x0 },
    /*  7 */        { 0x6, 0, 3, 0x0, 0,   0, 1, 0x4 },
    /*  8 */        { 0xC, 0, 4, 0x0, 0,   0, 1, 0x5 },
    /*  9 */        { 0xC, 0, 4, 0x4, 4,   0, 1, 0x6 },
    /* 10 */        { 0xB, 2, 5, 0x0, 5,   0, 1, 0x7 },
    /* 11 */        { 0x7, 0, 1, 0x5, 1,   0, 1, 0x4 },
    /* 12 */        { 0xB, 4, 5, 0x0, 5,   0, 1, 0x4 },
    /* 13 */        { 0xF, 0, 2, 0x0, 2,   0, 1, 0x1 },
    /* 14 */        { 0xC, 0, 1, 0x0, 0,  32, 0, 0x0 },
    /* 15 */        { 0xD, 0, 4, 0x0, 4,  43, 0, 0x0 },
    /* 16 */        { 0xF, 1, 2, 0x0, 2,  44, 0, 0x0 },
    /* 17 */        { 0xA, 0, 4, 0x0, 0,   0, 1, 0x5 },
    /* 18 */        { 0xF, 0, 4, 0xF, 4,   0, 1, 0x6 },
    /* 19 */        { 0xF, 3, 4, 0x8, 1,   0, 0, 0x0 },
    /* 20 */        { 0x8, 0, 2, 0x0, 2,   0, 0, 0x0 },
    /* 21 */        { 0xB, 0, 3, 0x0, 0,   0, 1, 0x0 },
    /* 22 */        { 0x9, 0, 2, 0x0, 0,  72, 0, 0x0 },
    /* 23 */        { 0xB, 0, 3, 0x0, 0,   0, 1, 0x0 },
    /*                VOL  A  D   S   R   TP  N? N-F */
};
static const int instruments_cnt = sizeof(_instr) / INSTR_SIZE;
