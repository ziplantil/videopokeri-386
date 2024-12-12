
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include <assert.h>

struct midi_instrument {
    unsigned char type;
    unsigned char patch;
    signed char transpose;
    unsigned char attack;
    unsigned char release;
    unsigned char volume;
};

static int IO_DATA = 0;
static int IO_STATUS = 0;
static int adlib_delay = 0;
static const struct midi_instrument *_bnk;
static const struct midi_instrument _bnk_midi[];
static const struct midi_instrument _bnk_mt32[];
static const int instruments_cnt;
static int midi_type;

static unsigned short channels[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned short instruments[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char instr_type[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char patch_num[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static signed char transpose[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char velocity_a[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char velocity_r[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static signed char volumes[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static signed char last_note[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

#define MPU_CMD(v) do { \
    while (inp(IO_STATUS) & 0x40); \
    outp(IO_STATUS, (v)); } while (0)
#define MPU_DATA(v) do { \
    while (inp(IO_STATUS) & 0x40); \
    outp(IO_DATA, (v)); } while (0)

void reset_mpu(void) {
    MPU_CMD(0xFF);
}

#define TIMEOUT 3000

int alusta_mpu(int instr_mode) {
    short s;
    IO_DATA = soittolaite_mpuportti;
    IO_STATUS = soittolaite_mpuportti + 1;

    switch (instr_mode) {
    case 0: _bnk = _bnk_midi; break;
    case 1: _bnk = _bnk_mt32; break;
    default: abort();
    }
    midi_type = instr_mode;
    
    s = 0;
    while ((inp(IO_STATUS) & 0x40) && s <= TIMEOUT)
        microsleep(1000), ++s;
    if (s >= TIMEOUT)
        return 1;
    outp(IO_STATUS, 0xFF);

    s = 0;
    for (;;) {
        if (s++ >= TIMEOUT)
            break;
        if ((inp(IO_STATUS) & 0x80)) {
            microsleep(1000);
            continue;
        }
        if (inp(IO_DATA) != 0xFE)
            break;
    }

    s = 0;
    for (;;) {
        if (s++ >= TIMEOUT)
            break;
        if ((inp(IO_STATUS) & 0x40)) {
            microsleep(1000);
            continue;
        }
        outp(IO_STATUS, 0x3F);
    }

    if (s >= TIMEOUT)
        return 1;

    MPU_DATA(0xB0);
    MPU_DATA(0x79);
    MPU_DATA(0x00);
    return 0;
}

static void note_off(short hwc, short ch) {
    if (last_note[hwc] < 0) return;
    MPU_DATA(0x80 | ch);
    MPU_DATA(last_note[hwc]);
    MPU_DATA(velocity_r[hwc]);
    last_note[hwc] = -1;
}

static void note_on(short hwc, short ch, short x) {
    if (last_note[hwc] >= 0)   
        note_off(hwc, ch);
    if (!instr_type[hwc])
        return;
    else if (instr_type[hwc] == 2) {
        x = patch_num[hwc];
        MPU_DATA(0xB0 | ch);
        MPU_DATA(7);
        MPU_DATA(volumes[hwc]);
    }
    last_note[hwc] = x;
    MPU_DATA(0x90 | ch);
    MPU_DATA(x);
    MPU_DATA(velocity_a[hwc]);
}

static int midi_get_melodic(int hwc) {
    switch (midi_type) {
    case 0: return hwc;
    case 1: return hwc + 1;
    }
    return hwc;
}

static int midi_get_percussion(int hwc) {
    switch (midi_type) {
    case 0: return 9;
    case 1: return 9;
    }
    return hwc;
}

void soitto_paivita_mpu(void) { }

void soitto_komento_mpu(short *c, short hwc,
                        unsigned char k, unsigned char x) {
    short ch = *c;
    switch (k) {
    case COMMAND_NOTE_ON:
        note_on(hwc, ch, x + transpose[hwc]);
        break;
    case COMMAND_NOTE_OFF:
        note_off(hwc, ch);
        break;
    case COMMAND_INSTRUMENT:
        if (x < instruments_cnt) {
            const struct midi_instrument *p = &_bnk[x];
            if (last_note[hwc] >= 0)
                note_off(hwc, ch);
            instruments[hwc] = x;
            transpose[hwc] = p->transpose;
            instr_type[hwc] = p->type;
            patch_num[hwc] = p->patch;
            velocity_a[hwc] = p->attack;
            velocity_r[hwc] = p->release;
            volumes[hwc] = p->volume;
            if (p->type == 1) {
                ch = *c = midi_get_melodic(hwc);
                MPU_DATA(0xC0 | ch);
                MPU_DATA(p->patch);
                MPU_DATA(0xB0 | ch);
                MPU_DATA(7);
                MPU_DATA(p->volume);
            } else {
                ch = *c = midi_get_percussion(hwc);
            }
            channels[hwc] = ch;
        }
        break;
    }
}

void soitto_toista_mpu(int m) {
    short i;
    for (i = 0; i < 8; ++i) {
        MPU_DATA(0xE0 + i);
        MPU_DATA(0x00);
        MPU_DATA(0x40);
    }
}

void soitto_pysayta_mpu(void) {
    short i;
    for (i = 0; i < 8; ++i) {
        note_off(i, channels[i]);
    }
    MPU_DATA(0xB0);
    MPU_DATA(0x7B);
    MPU_DATA(0x00);
}

void soitto_lopeta_mpu(void) {
    soitto_pysayta_mpu();
    reset_mpu();
}

/* instrument data for General MIDI */
/*          00:  0 = silent     1 = melodic      2 = percussive
            01: MIDI patch number
            02: transpose
            03: attack velocity
            04: release velocity
            05: volume
*/
static const struct midi_instrument _bnk_midi[] = {
/*  --------- T  PNo   TP    ATK   REL   VOL -- */
/*  0 */    { 1,  34,    0, 0x7F, 0x7F, 0x5F },
/*  1 */    { 1,  37,    0, 0x3F, 0x3F, 0x4F },
/*  2 */    { 1,  34,    0, 0x7F, 0x7F, 0x7F },
/*  3 */    { 1, 116,   50, 0x7F, 0x7F, 0x7F },
/*  4 */    { 1,   3,   24, 0x7F, 0x7F, 0x7F },
/*  5 */    { 1,  38,    0, 0x4F, 0x4F, 0x4F },
/*  6 */    { 1,   7,    0, 0x5F, 0x5F, 0x6F },
/*  7 */    { 2,  42,    0, 0x7F, 0x7F, 0x7F },
/*  8 */    { 2,  40,    0, 0x7F, 0x7F, 0x7F },
/*  9 */    { 1, 127,   40, 0x6F, 0x6F, 0x6F },
/* 10 */    { 1, 122,   40, 0x7F, 0x7F, 0x7F },
/* 11 */    { 2,  44,    0, 0x7F, 0x7F, 0x3F },
/* 12 */    { 1, 122,   72, 0x7F, 0x7F, 0x7F },
/* 13 */    { 2,  37,    0, 0x7F, 0x7F, 0x7F },
/* 14 */    { 2,  62,    0, 0x7F, 0x7F, 0x7F },
/* 15 */    { 1, 115,   57, 0x7F, 0x7F, 0x7F },
/* 16 */    { 1, 117,   81, 0x7F, 0x7F, 0x7F },
/* 17 */    { 2,  39,    0, 0x7F, 0x7F, 0x7F },
/* 18 */    { 1, 127,   40, 0x7F, 0x7F, 0x7F },
/* 19 */    { 1,  73,   24, 0x7F, 0x20, 0x7F },
/* 20 */    { 1, 118,    0, 0x7F, 0x7F, 0x7F },
/* 21 */    { 2,  42,    0, 0x7F, 0x7F, 0x5F },
/* 22 */    { 2,  56,    0, 0x7F, 0x7F, 0x7F },
/* 23 */    { 2,  38,    0, 0x7F, 0x7F, 0x3F },
/* 24 */    { 1,  11,   36, 0x7F, 0x20, 0x7F },
/*  --------- T  PNo   TP    ATK   REL   VOL -- */
};

/* instrument data for Roland MT-32 */
/*          00:  0 = silent     1 = melodic      2 = percussive
            01: MIDI patch number
            02: transpose
            03: velocity
            04: volume
*/
static const struct midi_instrument _bnk_mt32[] = {
/*  --------- T  PNo   TP    ATK   REL   VOL -- */
/*  0 */    { 1,  28,    0, 0x7F, 0x7F, 0x7F },
/*  1 */    { 1,  69,    0, 0x3F, 0x3F, 0x3F },
/*  2 */    { 1,  28,    0, 0x7F, 0x7F, 0x78 },
/*  3 */    { 1, 117,   50, 0x7F, 0x7F, 0x7F },
/*  4 */    { 1,   7,   24, 0x5F, 0x7F, 0x6F },
/*  5 */    { 1,  30,    0, 0x5F, 0x5F, 0x5F },
/*  6 */    { 1,  16,    0, 0x4F, 0x4F, 0x6F },
/*  7 */    { 2,  42,    0, 0x7F, 0x7F, 0x7F },
/*  8 */    { 2,  40,    0, 0x7F, 0x7F, 0x7F },
/*  9 */    { 1, 114,   28, 0x6F, 0x6F, 0x6F },
/* 10 */    { 1, 119,   52, 0x2F, 0x7F, 0x7F },
/* 11 */    { 2,  44,    0, 0x7F, 0x7F, 0x3F },
/* 12 */    { 1, 124,   60, 0x7F, 0x7F, 0x5F },
/* 13 */    { 2,  37,    0, 0x7F, 0x7F, 0x7F },
/* 14 */    { 2,  62,    0, 0x7F, 0x7F, 0x7F },
/* 15 */    { 1, 120,   57, 0x7F, 0x7F, 0x7F },
/* 16 */    { 1, 113,   81, 0x7F, 0x7F, 0x7F },
/* 17 */    { 2,  39,    0, 0x7F, 0x7F, 0x7F },
/* 18 */    { 1, 114,   28, 0x7F, 0x7F, 0x7F },
/* 19 */    { 1,  47,   25, 0x7F, 0x20, 0x7F },
/* 20 */    { 1,  64,    0, 0x7F, 0x7F, 0x4F },
/* 21 */    { 2,  42,    0, 0x7F, 0x7F, 0x5F },
/* 22 */    { 2,  56,    0, 0x7F, 0x7F, 0x7F },
/* 23 */    { 2,  40,    0, 0x7F, 0x7F, 0x5F },
/* 24 */    { 1,  38,   25, 0x7F, 0x20, 0x7F },
/*  --------- T  PNo   TP    VEL   VOL -- */
};
static const int instruments_cnt = sizeof(_bnk_midi) / sizeof(_bnk_midi[0]);
