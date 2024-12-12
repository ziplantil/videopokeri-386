
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "MUISTI.H"
#include <assert.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>

struct cms_instrument {
    unsigned char amplitude;        /* attack amplitude */
    unsigned char attack;           /* 0 for instant, 1-7 for faster-slower */
    unsigned char decay;            /* 0 for instant, 1-7 for faster-slower */
    unsigned char sustain;          /* 0-15 amplitude */
    unsigned char release;          /* 0 for instant, 1-7 for faster-slower */
    signed char transpose;          /* note transpose */
    unsigned char noise;            /* use noise gen, 0 or 1 */
    unsigned char nsef;             /* noise gen flags & octave */
    unsigned char nsefreq;          /* noise f-value */
    unsigned char envelope;         /* use envelope, 0 or 1 */
    unsigned char envf;             /* envelope flags */
    signed char transp_env;         /* envelope transpose (rel. transp note) */
};

static int IO_ADDR = 0;
static int IO_DATA = 0;
static unsigned char instruments[4] = { 0 };
static unsigned char amplitudes[4] = { 0 };
static unsigned char enable_freq[4] = { 0 };
static unsigned char enable_noise[4] = { 0 };
static unsigned char noise_driven[4] = { 0 };
static unsigned char noise_octave[4] = { 0 };
static unsigned char noise_freq[4] = { 0 };
static unsigned char use_env[4] = { 0 };
static signed char transpose[4] = { 0 };
static signed char transpose_env[4] = { 0 };
static unsigned char reg1[32] = { 0 };
static unsigned char reg2[32] = { 0 };

static unsigned char key_on[4] = { 0 };
static unsigned char key_peak[4] = { 0 };
static unsigned char attack_speed[4] = { 0 };
static unsigned char attack_rate[4] = { 0 };
static unsigned char decay_speed[4] = { 0 };
static unsigned char decay_rate[4] = { 0 };
static unsigned char sustain_level[4] = { 0 };
static unsigned char release_speed[4] = { 0 };
static unsigned char release_rate[4] = { 0 };
static unsigned char counter[4] = { 0 };
static unsigned char chip_ampl[4] = { 0 };

#define INSTR_SIZE (sizeof(struct cms_instrument))
static const struct cms_instrument _instr[];
static const int instruments_cnt;

#define REG(d) ((d) ? reg2 : reg1)
#define CMS_WRITE(d, r, v) do {                          \
        outp(IO_ADDR + d, (r));                          \
        outp(IO_DATA + d, REG(d)[r] = (v));              \
    } while (0)

void reset_cms(int on) {
    int i;
    for (i = 0; i < 32; ++i) {
        CMS_WRITE(0, i, 0);
        CMS_WRITE(2, i, 0);
    }
    CMS_WRITE(0, 0x1c,  2); CMS_WRITE(2, 0x1c,  2);
    CMS_WRITE(0, 0x1c, on); CMS_WRITE(2, 0x1c, on);
}

void silence_cms(void) {
    char i;
    for (i = 0; i < 16; ++i) {
        CMS_WRITE(0, i, 0);
        CMS_WRITE(2, i, 0);
    }
    CMS_WRITE(0, 0x14, 0);
    CMS_WRITE(2, 0x14, 0);
    CMS_WRITE(0, 0x15, 0);
    CMS_WRITE(2, 0x15, 0);
}

extern unsigned char reset_sb_dsp_io(int base, int sb_ok);

int alusta_cms(void) {
    int io = soittolaite_cmsportti;
    unsigned char tmp;
    IO_ADDR = io + 1;
    IO_DATA = io;

    outp(io + 6, 0xc6);
    outp(io + 10, 0x00);
    if (inp(io + 10) != 0xc6) goto detect_sb;
    outp(io + 6, 0x39);
    outp(io + 10, 0x00);
    if (inp(io + 10) != 0x39) goto detect_sb;
    goto detect_ok;

detect_sb:
    /* ehkä meillä on Sound Blaster 1.0 / 2.0? */
    tmp = reset_sb_dsp_io(io, 0);
    if (tmp == 0xAA) {
        outp(io + 12, 0xe1);
        while (!(inp(io + 14) & 0x80))
            ;
        tmp = inp(io + 10);
        inp(io + 10);
        if (tmp < 3)
            goto detect_ok;
    }
    return 1;

detect_ok:
    reset_cms(1);
    fast_memset(chip_ampl, 0, sizeof(chip_ampl));
    return 0;
}

/* calibrated for CMS -- master frequency 2*(315/88)*1e6. base: A */
static const unsigned char f_table[12] = {
    3, 31, 58, 83, 107, 130, 151, 172, 191, 209, 226, 242
};
static const unsigned short sekoitus_s[6] = {
    0x80, 0, 0x120, 0, 0x140, 0x1A0
};

static void note_program_noise(unsigned char d, unsigned char c, short ch,
                            unsigned char x) {
    unsigned char t, o, n, v = 1 << c;
    short s;
    o = noise_octave[ch], n = noise_freq[ch];
    switch (instruments[ch]) {
    case 3:
        if (x >= 9) n += 0x50;
        break;
    case 10:
        s = sekoitus_s[x];
        o = 4 + (s >> 8); n = s & 0xFF;
        break;
    }
    t = 0x10 + (c >> 1);
    if (c & 1)
        CMS_WRITE(d, t, (REG(d)[t] & 0x0F) | (o << 4));
    else
        CMS_WRITE(d, t, (REG(d)[t] & 0xF0) | o);
    CMS_WRITE(d, 0x08 | c, n);
}

static void note_program(unsigned char d, unsigned char c,
                unsigned char x, unsigned char a) {
    unsigned char t, o, n;
    x += 3;
    o = x / 12 - 1, n = x % 12;
    t = 0x10 + (c >> 1);
    CMS_WRITE(d, c, a);
    if (c & 1)
        CMS_WRITE(d, t, (REG(d)[t] & 0x0F) | (o << 4));
    else
        CMS_WRITE(d, t, (REG(d)[t] & 0xF0) | o);
    CMS_WRITE(d, 0x08 | c, f_table[n]);
}

static void note_on(short ch) {
    unsigned char d = ch & 2, v = 1 << (3 * (ch & 1) + 2);
    if (enable_freq[ch])
        CMS_WRITE(d, 0x14, REG(d)[0x14] | v);
    if (enable_noise[ch])
        CMS_WRITE(d, 0x15, REG(d)[0x15] | v);
}

static void note_off(short ch) {
    unsigned char c = ch & 1, d = ch & 2, v = 1 << c, t = REG(d)[0x14];
    t &= ~(1 << (c * 3 + 2));
    if (noise_driven[ch])       t &= ~(1 << (c * 3));
    if (use_env[ch])            t &= ~(1 << (c * 3 + 1));
    if (REG(d)[0x15] & v)
        CMS_WRITE(d, 0x15, REG(d)[0x15] & ~v);
    if (t != REG(d)[0x14])
        CMS_WRITE(d, 0x14, t);
}

static void note_off_all(short ch) {
    unsigned char c = ch & 1, d = ch & 2, v = 1 << c, t = REG(d)[0x14];
    t &= ~(7 << (c * 3));
    if (REG(d)[0x15] & v)
        CMS_WRITE(d, 0x15, REG(d)[0x15] & ~v);
    if (t != REG(d)[0x14])
        CMS_WRITE(d, 0x14, t);
}

static void update_adsr(short ch) {
    unsigned char cl = ch & 1, d = ch & 2, al = chip_ampl[ch], a = al, ta;
    cl = 3 * cl + 2;
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
        CMS_WRITE(d, cl, a | (a << 4));
        if (a && !al) {
            note_on(ch);
            counter[ch] = 0;
        } else if (!a && al) {
            note_off(ch);
            counter[ch] = 0;
        }
        chip_ampl[ch] = a;
    }
}

static char get_octave(short ch, char x) {
    unsigned char c = ch & 1, d = ch & 2, t;
    c = 3 * c + x;
    t = 0x10 | (c >> 1);
    return c & 1 ? (REG(d)[t] & 0x70) >> 4 : REG(d)[t] & 0x07;
}

static void set_octave(short ch, char x, char o) {
    unsigned char c = ch & 1, d = ch & 2, t;
    c = 3 * c + x;
    t = 0x10 | (c >> 1);
    if (c & 1) {
        CMS_WRITE(d, t, (REG(d)[t] & 0x0F) | (o << 4));
    } else {
        CMS_WRITE(d, t, (REG(d)[t] & 0xF0) | o);
    }
}

void soitto_paivita_cms(void) {
    short i, s;
    unsigned char c, d, t;
    for (i = 0; i < 4; ++i) {
        c = 3 * (i & 2), d = i & 2;
        update_adsr(i);
        switch (instruments[i]) {
        case 3:
            CMS_WRITE(d, 0x08 | (c + 2), REG(d)[0x08 | (c + 2)] + 1);
            break;
        case 14:
            CMS_WRITE(d, 0x08 | c, REG(d)[0x08 | c] - 48);
            CMS_WRITE(d, 0x08 | (c + 2), REG(d)[0x08 | (c + 2)] - 24);
            break;
        case 9:
        case 18:
            set_octave(i, 0, get_octave(i, 0) ^ 1);
            break;
        case 15:
            CMS_WRITE(d, 0x08 | c, REG(d)[0x08 | c] - 7);
            break;
        case 16:
            s = REG(d)[0x08 | (c + 2)] + 2;
            CMS_WRITE(d, 0x08 | (c + 2), s);
            if (s >= 0x100) {
                set_octave(i, 2, get_octave(i, 2) + 1);
            }
            break;
        }
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

void soitto_komento_cms(short *c, short hwc, unsigned char k, unsigned char x) {
    short ch = *c;
    unsigned char cl = ch & 1, d = ch & 2;
    assert(ch < 4);
    switch (k) {
    case COMMAND_NOTE_ON: {
        unsigned char t;
        t = REG(d)[0x14];
        if (noise_driven[ch]) {
            note_program_noise(d, cl * 3 + 0, ch, x);
            t |= 1 << (cl * 3 + 0);
        }
        if (use_env[ch]) {
            note_program(d, cl * 3 + 1, x + transpose_env[ch], 0);
            t |= 1 << (cl * 3 + 1);
        }
        note_program(d, cl * 3 + 2, x + transpose[ch], 0);
        if (t != REG(d)[0x14])
            CMS_WRITE(d, 0x14, t);
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
            const struct cms_instrument *in = &_instr[x];
            unsigned char ef, nf;

            instruments[ch] = x;
            amplitudes[ch] = in->amplitude;
            transpose[ch] = in->transpose;
            enable_noise[ch] = in->noise;
            enable_freq[ch] = in->noise != 1;
            nf = in->nsef & 3;
            noise_freq[ch] = in->nsefreq;
            use_env[ch] = in->envelope;
            ef = in->envelope ? in->envf | 0x80 : 0;
            transpose_env[ch] = in->transp_env;
            noise_driven[ch] = in->noise && nf == 3;
            noise_octave[ch] = (in->nsef >> 4) & 7;

            counter[ch] = 0;
            AD_R_PARAM(ch, attack, in->attack);
            AD_R_PARAM(ch, decay, in->decay);
            sustain_level[ch] = in->sustain;
            AD_R_PARAM(ch, release, in->release);

            if (!in->sustain && !in->decay)
                sustain_level[ch] = in->amplitude;

            key_on[ch] = chip_ampl[ch] = 0;
            note_off_all(ch);
            CMS_WRITE(d, cl, 0);

            if (in->envelope || (REG(d)[0x18 + cl] & 0x80))
                CMS_WRITE(d, 0x18 + cl, ef);

            if (in->noise) {
                if (cl & 1)
                    CMS_WRITE(d, 0x16, (REG(d)[0x16] & 0xCF) | (nf << 4));
                else
                    CMS_WRITE(d, 0x16, (REG(d)[0x16] & 0xFC) | nf);
            }
        }
        break;
    }
}

void soitto_toista_cms(int m) {
    silence_cms();
}

void soitto_pysayta_cms(void) {
    fast_memset(key_on, 0, sizeof(key_on));
}

void soitto_lopeta_cms(void) {
    soitto_pysayta_cms();
    silence_cms();
    reset_cms(0);
}

/* 12 bytes per instrument
    0       (attack) amplitude
    1       attack. 0 for instant, 1-7 for faster-slower
    2       decay. 0 for instant, 1-7 for faster-slower
                if sustain = 0, decay = 0, ADSR disabled
    3       sustain. 0-15
    4       release. 0 for instant, 1-7 for faster-slower
    5       note transpose
    6       use noise gen. 0 = tone only, 1 = noise only, 2 = tone and noise
    7       lo 2: noise gen flags (3 for "drive noise"). hi 3: octave (driven)
    8       noise value, 0 - 255 (only matters if driven)
    9       use envelope, 0 or 1
    10      envelope flags
    11      envelope transpose (relative to transposed note)
*/
static const struct cms_instrument _instr[] = {
    /*                VOL  A  D   S   R   TP  N? N-OF  N-VL  E? E-FF E-TP */
    /*  0 */        { 0xD, 0, 4, 0x0, 2,   0, 0, 0x00, 0x00, 0, 0x00,   0 },
    /*  1 */        { 0xA, 0, 0, 0xA, 6, -12, 0, 0x00, 0x00, 0, 0x8E,  36 },
    /*  2 */        { 0xD, 0, 4, 0x0, 2, -12, 0, 0x00, 0x00, 0, 0x00,   0 },
    /*  3 */        { 0xE, 0, 5, 0x0, 5,  12, 1, 0x33, 0xA0, 0, 0x00,   0 },
    /*  4 */        { 0xF, 0, 3, 0x0, 2,  12, 0, 0x00, 0x00, 0, 0x00,   0 },
    /*  5 */        { 0x8, 0, 0, 0x8, 6, -12, 0, 0x00, 0x00, 0, 0x8E,  36 },
    /*  6 */        { 0xF, 0, 6, 0x0, 6,   0, 0, 0x00, 0x00, 1, 0x8E,  36 },
    /*  7 */        { 0x6, 0, 3, 0x0, 0,   0, 1, 0x73, 0x40, 0, 0x00,   0 },
    /*  8 */        { 0xC, 0, 4, 0x0, 0,   0, 1, 0x63, 0xD0, 0, 0x00,   0 },
    /*  9 */        { 0x8, 0, 4, 0xF, 4,   0, 1, 0x53, 0x40, 0, 0x00,   0 },
    /* 10 */        { 0xB, 2, 5, 0x0, 5,   0, 1, 0x03, 0x00, 0, 0x00,   0 },
    /* 11 */        { 0xC, 0, 1, 0x5, 1,   0, 1, 0x02, 0x00, 0, 0x00,   0 },
    /* 12 */        { 0xE, 4, 5, 0x0, 5,   0, 1, 0x63, 0xD0, 0, 0x00,   0 },
    /* 13 */        { 0xF, 0, 2, 0x0, 2,   0, 1, 0x63, 0x70, 0, 0x00,   0 },
    /* 14 */        { 0xC, 0, 1, 0x0, 0,  26, 2, 0x33, 0xFF, 1, 0x8E,   0 },
    /* 15 */        { 0xD, 0, 4, 0x0, 4,  31, 2, 0x23, 0xFF, 0, 0x00,   0 },
    /* 16 */        { 0xF, 1, 3, 0x0, 3,  56, 0, 0x00, 0x00, 1, 0x8E,  24 },
    /* 17 */        { 0xF, 0, 4, 0x0, 0,   0, 1, 0x63, 0x80, 0, 0x00,   0 },
    /* 18 */        { 0xF, 0, 4, 0xF, 4,   0, 1, 0x53, 0x40, 0, 0x00,   0 },
    /* 19 */        { 0xF, 3, 4, 0x8, 1,   0, 0, 0x00, 0x80, 0, 0x00,   0 },
    /* 20 */        { 0x8, 0, 2, 0x0, 2, -12, 0, 0x00, 0x40, 0, 0x00,   0 },
    /* 21 */        { 0xB, 0, 3, 0x0, 0,   0, 1, 0x73, 0x10, 0, 0x00,   0 },
    /* 22 */        { 0x9, 0, 2, 0x0, 0,  84, 0, 0x00, 0x80, 0, 0x00,   0 },
    /* 23 */        { 0xB, 0, 3, 0x0, 0,   0, 1, 0x73, 0x10, 0, 0x00,   0 },
    /* 24 */        { 0xE, 0, 6, 0x0, 6,   0, 0, 0x00, 0x00, 0, 0x00,   0 },
    /*                VOL  A  D   S   R   TP  N? N-OF  N-VL  E? E-FF E-TP */
};
static const int instruments_cnt = sizeof(_instr) / INSTR_SIZE;
