
#include <conio.h>
#include <dos.h>
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include <assert.h>

static int IO_ADDR = 0;
static int IO_DATA = 0;
static int adlib_delay = 0;
static unsigned char reg[256] = { 0 };
#define INSTR_SIZE 14
static const unsigned char _bnk[];
static const int instruments_cnt;

#define ADLIB_READ() (inp(IO_ADDR))

//#define OPL2_DATA_WAIT() microsleep(23)
//#define OPL2_ADDR_WAIT() microsleep(4)

static void opl2_addr_wait_(void) {
    /* 6 */
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
}

static void opl2_data_wait_(void) {
    /* 35 */
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
    ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ(); ADLIB_READ();
}

#define OPL2_ADDR_WAIT opl2_addr_wait_
#define OPL2_DATA_WAIT opl2_data_wait_

#define ADLIB_WRITE(r, v) do {                              \
        if (adlib_delay)    OPL2_DATA_WAIT();               \
        outp(IO_ADDR, (r)); OPL2_ADDR_WAIT();               \
        outp(IO_DATA, reg[r] = (v)); adlib_delay = 1;       \
    } while (0);

static void reset_adlib(void) {
    int i;
    for (i = 1; i <= 0xf5; ++i)
        ADLIB_WRITE(i, i >= 0x40 && i <= 0x55 ? 0xFF : 0);
}

int alusta_adlib(void) {
    int s, s2;
    IO_ADDR = soittolaite_fmportti;
    IO_DATA = soittolaite_fmportti + 1;
    ADLIB_WRITE(4, 0x60);
    ADLIB_WRITE(4, 0x80);
    s = ADLIB_READ();
    ADLIB_WRITE(2, 0xFF);
    ADLIB_WRITE(4, 0x21);
    microsleep(1000);
    s2 = ADLIB_READ();
    ADLIB_WRITE(4, 0x60);
    ADLIB_WRITE(4, 0x80);
    s &= 0xE0; s2 &= 0xE0;
    if (s != 0x00 || s2 != 0xC0)
        return 1;
    reset_adlib();
    ADLIB_WRITE(0x01, 0x20);
    ADLIB_WRITE(0xbd, 0x20);
    return 0;
}

static unsigned short instruments[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char first_note[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static unsigned short pseudonoise_c = 709;

static unsigned short pseudonoise(void) {
    return (pseudonoise_c = 40185 * pseudonoise_c + 18287);
}

void soitto_paivita_adlib(void) {
    short i;
    adlib_delay = 0;
    
    for (i = 0; i < 6; ++i) {
        if (first_note[i]) {
            first_note[i] = 0;
            continue;
        }
        if (instruments[i] == 10) {
            short s = ((((reg[0xb0 | i] & 3) << 8) | reg[0xa0 | i]) - 80);
            if (s >= 0) {
                ADLIB_WRITE(0xa0 | i, s & 0xFF);
                ADLIB_WRITE(0xb0 | i, (reg[0xb0] & 0xFC) | ((s >> 8) & 3));
            }
        } else if (instruments[i] == 16) {
            short s = ((((reg[0xb0 | i] & 3) << 8) | reg[0xa0 | i]) + 16);
            if (s >= 0) {
                ADLIB_WRITE(0xa0 | i, s & 0xFF);
                ADLIB_WRITE(0xb0 | i, (reg[0xb0] & 0xFC) | ((s >> 8) & 3));
            }
        }
    }
}

static const unsigned char OPL2_LO[] = {
                                  0x00, 0x01, 0x02, 0x08, 0x09, 0x0a, 0, 0,
                                  0x11, 0x12, 0x12, 0x11, 0x10 };
static const unsigned char OPL2_HI[] = {
                                  0x03, 0x04, 0x05, 0x0b, 0x0c, 0x0d, 0, 0,
                                  0x14, 0x15, 0x15, 0x14, 0x13 };
static const unsigned char drum_ch[] = { 7, 8, 8, 7, 6 };
static signed char transpose[] = { 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0 };

static const unsigned short f_table[] = {
    0x156, 0x16b, 0x181, 0x198, 0x1b0, 0x1ca,
    0x1e5, 0x202, 0x220, 0x241, 0x263, 0x287
};

static void adlib_turn_note_on(int ch, int octave, int note) {
    unsigned short s = 0x2000 | (octave << 10) | f_table[note];
    ADLIB_WRITE(0xa0 | ch, s & 0xFF);
    ADLIB_WRITE(0xb0 | ch, (s >> 8) & 0xFF);
}

static void adlib_turn_note_off(int ch) {
    ADLIB_WRITE(0xb0 | ch, reg[0xb0 | ch] & ~0x20);
}

void soitto_komento_adlib(short *c, short hwc,
                        unsigned char k, unsigned char x) {
    short ch = *c;
    switch (k) {
    case COMMAND_NOTE_ON: {
        x += transpose[ch];
        first_note[ch] = 1;
        if (ch & 8) {
            /* percussion */
            unsigned short s = ((x / 12) << 10) | f_table[x % 12];
            ch &= 7;
            ADLIB_WRITE(0xa0 | drum_ch[ch], s & 0xFF);
            ADLIB_WRITE(0xb0 | drum_ch[ch], (s >> 8) & 0xFF);
            ADLIB_WRITE(0xbd, reg[0xbd] | (1 << ch));
        } else {
            unsigned char o = x / 12, n = x % 12;
            assert(ch < 6);
            adlib_turn_note_on(ch, o, n);
            if (instruments[hwc] == 9 || instruments[hwc] == 18) {
                adlib_turn_note_on(++ch, o + 1, n + 1);
                adlib_turn_note_on(++ch, o + 2, n + 2);
            }
        }
        break;
    }
    case COMMAND_NOTE_OFF:
        if (ch & 8) {
            /* percussion */
            ch &= 7;
            ADLIB_WRITE(0xbd, reg[0xbd] & ~(1 << ch));
        } else {
            assert(ch < 6);
            adlib_turn_note_off(ch);
            if (instruments[hwc] == 9 || instruments[hwc] == 18) {
                adlib_turn_note_off(++ch);
                adlib_turn_note_off(++ch);
            }
        }
        break;
    case COMMAND_INSTRUMENT:
        if (x < instruments_cnt) {
            const unsigned char *p = _bnk + x * INSTR_SIZE;
            unsigned char t, l, h;
            int i = (x == 9 || x == 18) ? 3 : 1;
            t = *p++;
            if (t) {
                *c = ch = 8 + (*p & 7);
            } else if (ch & 8) {
                *c = ch = hwc; /* oletuskanavan palautus */
            }
            instruments[hwc] = x;

            while (i--) {
                l = OPL2_LO[ch], h = OPL2_HI[ch];
                ++p;
                t = *p++; ADLIB_WRITE(0xc0 | ch, t);
                t = *p++; ADLIB_WRITE(0xe0 | l, t);
                t = *p++; ADLIB_WRITE(0x20 | l, t);
                t = *p++; ADLIB_WRITE(0x40 | l, t);
                t = *p++; ADLIB_WRITE(0x60 | l, t);
                t = *p++; ADLIB_WRITE(0x80 | l, t);

                transpose[ch] = (signed char)(*p++);
                t = *p++; ADLIB_WRITE(0xe0 | h, t);
                t = *p++; ADLIB_WRITE(0x20 | h, t);
                t = *p++; ADLIB_WRITE(0x40 | h, t);
                t = *p++; ADLIB_WRITE(0x60 | h, t);
                t = *p++; ADLIB_WRITE(0x80 | h, t);
                ++ch;
            }

            pseudonoise_c = 709;
        }
        break;
    }
}

void soitto_toista_adlib(int m) { }

void soitto_pysayta_adlib(void) {
    int i;
    adlib_delay = 1;
    for (i = 0xb0; i < 0xb6; ++i)
        ADLIB_WRITE(i, reg[i] & ~0x20);
    for (i = 0x40; i <= 0x55; ++i)
        ADLIB_WRITE(i, 0xFF);
}

void soitto_lopeta_adlib(void) {
    soitto_pysayta_adlib();
    reset_adlib();
}

/* instrument data */
/* 14 (0x0e) bytes per instrument */
/*          00:  0 = melodic             1 = percussive
            01: only if percussive:
                    0 = hi-hat
                    1 = cymbal
                    2 = tom tom
                    3 = snare drum
                    4 = bass drum
            02:                     base c0 (feedback, connect)

            03: modulator (op 0)    base e0 (waveform)
            04: modulator (op 0)    base 20 (flags, EG, key scale, mul)
            05: modulator (op 0)    base 40 (key scale, output level)
            06: modulator (op 0)    base 60 (attack/decay)
            07: modulator (op 0)    base 80 (sustain/release)

            08: transpose           +12 = +1 8ve, -12 = -1 8ve

            09: carrier (op 1)      base e0 (waveform)
            0a: carrier (op 1)      base 20 (flags, EG, key scale, mul)
            0b: carrier (op 1)      base 40 (key scale, output level)
            0c: carrier (op 1)      base 60 (attack/decay)
            0d: carrier (op 1)      base 80 (sustain/release)
    

    c0      bits 3-1: feedback (0-7), bit 0: connecting mode (0 = FM, 1 = AM)

    e0      bits 1-0: waveform (sine, half-sine, abs-sine, quarter-sine)
    20      bit 7: tremolo, bit 6: vibrato, bit 5: EG, bit 4: KSR,
            bits 0-3: mod mul (0-15)
    40      bits 7-6: scaling level/8ve (0-3)
                00 = -0, 01 = -3 dB/8ve, 10 = -1.5 dB/8ve, 11 = -6 dB/8ve
            bits 5-0: output level (0-63)
                00 = loudest, 63 = quietest
    60      bits 7-4: attack rate (0-15), bits 3-0: decay rate (0-15)
    80      bits 7-4: sustain level (0-15), bits 3-0: release rate (0-15)
                    0 loudest, 15 quietest
*/
#define N(x) ((x + 0x100) & 0xFF)
static const unsigned char _bnk[] = {
/*  0 */
    0x00, 0x00, 0x00, 0x03, 0x20, 0x60, 0x91, 0x56,
                0x00, 0x03, 0x30, 0x00, 0xb0, 0x25,
/*  1 */
    0x00, 0x00, 0x08, 0x00, 0x10, 0x0a, 0xfe, 0x05,
                0x00, 0x00, 0x11, 0x08, 0xf2, 0x7d,
/*  2 */
    0x00, 0x00, 0x00, 0x03, 0x00, 0x60, 0x95, 0x5a,
                0x00, 0x03, 0x10, 0x00, 0xb5, 0x59,
/*  3 */
    0x00, 0x00, 0x0c, 0x02, 0x0d, 0x80, 0xf0, 0xf0,
              N(-45), 0x02, 0x0b, 0x80, 0xa7, 0xe7,
/*  4 */
    0x00, 0x00, 0x00, 0x02, 0x22, 0x59, 0x91, 0xf4,
                  12, 0x00, 0x21, 0x00, 0xf3, 0xf6,
/*  5 */
    0x00, 0x00, 0x06, 0x00, 0x01, 0x8d, 0xfe, 0x01,
              N(-12), 0x00, 0x21, 0x4c, 0xf2, 0x72,
/*  6 */
    0x00, 0x00, 0x04, 0x03, 0x24, 0x2b, 0x86, 0xf5,
                0x00, 0x02, 0x20, 0x00, 0xf4, 0xf3,
/*  7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*  8 */
    0x00, 0x00, 0x0a, 0x00, 0x0c, 0x00, 0xf6, 0x08,
                  48, 0x02, 0x12, 0x05, 0x7b, 0x47,
/*  9 */
    0x00, 0x00, 0x0c, 0x02, 0x0d, 0x00, 0xf0, 0xf0,
              N(-55), 0x02, 0x0b, 0x00, 0xf7, 0xe9,
/* 10 */
    0x00, 0x00, 0x0e, 0x00, 0xee, 0x00, 0xf0, 0x00,
                  60, 0x03, 0xe0, 0x08, 0x75, 0x05,
/* 11 */
    0x00, 0x00, 0x0e, 0x00, 0x1b, 0x80, 0xf0, 0xf1,
                  48, 0x03, 0x10, 0x12, 0xf6, 0x05,
/* 12 */
    0x00, 0x00, 0x0e, 0x02, 0x1b, 0x80, 0x50, 0xf1,
                0x00, 0x03, 0x10, 0x10, 0x56, 0x03,
/* 13 */
    0x00, 0x00, 0x0e, 0x00, 0x1b, 0x80, 0xf0, 0xf1,
                  20, 0x03, 0x10, 0x08, 0xf7, 0x0a,
/* 14 */
    0x00, 0x00, 0x00, 0x00, 0x05, 0x17, 0xfc, 0x05,
                  48, 0x02, 0x20, 0x10, 0xfc, 0xf2,
/* 15 */
    0x00, 0x00, 0x02, 0x01, 0x00, 0x00, 0xf8, 0xf6,
                  26, 0x03, 0x01, 0x00, 0xf9, 0x09,
/* 16 */
    0x00, 0x00, 0x08, 0x02, 0x00, 0x00, 0xfb, 0x56,
                  79, 0x00, 0x00, 0x00, 0xfa, 0x26,
/* 17 */
    0x00, 0x00, 0x0e, 0x02, 0x00, 0x00, 0xfc, 0x05,
                  48, 0x00, 0x00, 0x00, 0xfa, 0x17,
/* 18 */
    0x00, 0x00, 0x0c, 0x02, 0x0d, 0x00, 0xf0, 0xf0,
              N(-55), 0x02, 0x0b, 0x00, 0xf5, 0xe6,
/* 19 */
    0x00, 0x00, 0x00, 0x02, 0x22, 0x60, 0x90, 0x56,
                  12, 0x00, 0x31, 0x00, 0xb0, 0x25,
/* 20 */
    0x00, 0x00, 0x00, 0x02, 0x03, 0x60, 0x95, 0x5a,
                0x00, 0x01, 0x11, 0x0a, 0xb5, 0x59,
/* 21 */
    0x00, 0x00, 0x0e, 0x02, 0x00, 0x00, 0xfc, 0x05,
                0x00, 0x00, 0x00, 0x07, 0xfa, 0x17,
/* 22 */
    0x00, 0x00, 0x0e, 0x00, 0x12, 0x45, 0xf8, 0x37,
                  83, 0x00, 0x10, 0x08, 0xf3, 0x05,
/* 23 */
    0x00, 0x00, 0x0e, 0x02, 0x00, 0x00, 0xfc, 0x05,
                0x00, 0x00, 0x00, 0x07, 0xfa, 0x17,
/* 24 */
    0x00, 0x00, 0x00, 0x02, 0x22, 0x60, 0xb3, 0x53,
                  12, 0x00, 0x31, 0x00, 0xf2, 0xf1,
/* 25 */
    0x00, 0x00, 0x0c, 0x02, 0x0d, 0x00, 0xf0, 0xf0,
              N(-55), 0x02, 0x0b, 0x10, 0xf7, 0xe9,
/* 26 */
    0x00, 0x00, 0x0c, 0x02, 0x0d, 0x00, 0xf0, 0xf0,
              N(-55), 0x02, 0x0b, 0x10, 0xf5, 0xe6,
};
static const int instruments_cnt = sizeof(_bnk) / INSTR_SIZE;
