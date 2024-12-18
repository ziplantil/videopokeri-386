
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "PCM.H"
#if SB_PCM
#include "DMA.H"
#endif
#include "MUISTI.H"
#include <assert.h>

static unsigned short IO_SB = 0;
static unsigned char sb_irq = 0;
static unsigned char sb_dma = 0;
static char pcm_enabled = 0;
static char disable_fm = 0;
static volatile char pcm_playing = 0;
static volatile char in_irq = 0;

/**********************************/
/*  ADLIB                         */
/**********************************/
/* ks. S_ADLIB.C */

int alusta_adlib(void);
void soitto_paivita_adlib(void);
void soitto_komento_adlib(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_adlib(int m);
void soitto_pysayta_adlib(void);
void soitto_lopeta_adlib(void);

/* sound blaster */

unsigned char reset_sb_dsp_io(int base, int sb_ok) {
    outp(base + 6, 1);
    microsleep(5);
    outp(base + 6, 0);
    if (sb_ok) {
        while (!(inp(base + 14) & 0x80))
            ;
    } else
        microsleep(1000);
    return inp(base + 10);
}

unsigned char reset_sb_dsp(int sb_ok) {
    return reset_sb_dsp_io(IO_SB, sb_ok);
}

#if SB_PCM

#define DMA_CHANNEL_IO(c,r) (((c) >= 4 ? 0xC0 + ((r) << 1) : (r)))
static struct dma_ptr dmabuf;

typedef void (__interrupt __far *int_func_t)();
static int_func_t dma_interrupt_backup;
static unsigned char imr_backup;

static void __interrupt __far pcm_stop_irq();

static const unsigned char int_num[16] = {
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77
};

static unsigned char int_irq(unsigned char irq) {
    return int_num[irq];
}

static unsigned char get_imr(unsigned char io) {
    return inp(io + 1);
}

static void set_imr(unsigned char io, unsigned char s) {
    outp(io + 1, s);
}

static void sb_write(unsigned char x) {
    unsigned short s = IO_SB + 0x0c;
    while (inp(s) & 0x80)
        ;
    outp(s, x);
}

static const unsigned char dma_page[8] = {
    0x87, 0x83, 0x81, 0x82, 0x8f, 0x8b, 0x89, 0x8a
};

static void swap_inth(unsigned char intn, int_func_t *oldhandler,
                      int_func_t newhandler) {
    if (oldhandler)
        *oldhandler = _dos_getvect(intn);
    _dos_setvect(intn, newhandler);
}

static void pcm_stop(void) {
    if (!pcm_playing) return;
    pcm_playing = 0;
    sb_write(0xd3);
    outp(DMA_CHANNEL_IO(sb_dma, 0xA), 4 + (sb_dma & 3));
    if (!in_irq) _disable();
    set_imr(sb_irq & 8 ? 0xa0 : 0x20, imr_backup);
    swap_inth(int_irq(sb_irq), NULL, dma_interrupt_backup);
    reset_sb_dsp(1);
    if (!in_irq) _enable();
}

static void pcm_play(const unsigned char *data, unsigned n, int samplerate) {
    unsigned char pic, *p = dma_getptr(&dmabuf), tmp;
    unsigned long lp = dma_linearptr(&dmabuf);
    unsigned long buf = samplerate / 10;
    assert((lp >> 24) == 0);
    /* 100 ms eteen kaiken varalta */
    fast_memset(p, 0x80, buf);
    fast_memcpy(p + buf, data, n);
    n += buf;
    _disable();
    pcm_stop();
    pcm_playing = 1;
    --n;
    pic = sb_irq & 8 ? 0xa0 : 0x20;
    swap_inth(int_irq(sb_irq), &dma_interrupt_backup, &pcm_stop_irq);
    imr_backup = get_imr(pic);
    outp(pic, 0x60 + (sb_irq & 7));
    set_imr(pic, imr_backup & ~(1 << (sb_irq & 7)));
    sb_write(0x40);
    sb_write((unsigned char)(256UL - (1000000UL / samplerate)));
    sb_write(0xd1);
    outp(DMA_CHANNEL_IO(sb_dma, 0xA), 4 + (sb_dma & 3));
    outp(DMA_CHANNEL_IO(sb_dma, 0xC), 1);
    outp(DMA_CHANNEL_IO(sb_dma, 0xB), 0x48 + (sb_dma & 3));
    outp(dma_page[sb_dma], lp >> 16);
    outp(DMA_CHANNEL_IO(sb_dma, 0xC), 1);
    outp(DMA_CHANNEL_IO(sb_dma, 0x2), lp & 0xFF);
    outp(DMA_CHANNEL_IO(sb_dma, 0x2), (lp >> 8) & 0xFF);
    outp(DMA_CHANNEL_IO(sb_dma, 0xC), 1);
    outp(DMA_CHANNEL_IO(sb_dma, 0x3), n & 0xFF);
    outp(DMA_CHANNEL_IO(sb_dma, 0x3), (n >> 8) & 0xFF);
    outp(DMA_CHANNEL_IO(sb_dma, 0xA), sb_dma & 3);
    sb_write(0x14);
    sb_write(n & 0xFF);
    sb_write((n >> 8) & 0xFF);
    _enable();
}

static void interrupt far pcm_stop_irq() {
    _disable();
    in_irq = 1;
    (void)inp(IO_SB + 0x0e);
    pcm_stop();
    if (sb_irq & 8)
        outp(0xA0, 0x20);
    outp(0x20, 0x20);
    in_irq = 0;
    _enable();
}
#else
#define pcm_stop() 
#endif

int alusta_sb(void) {
    int result, result2;
    IO_SB = soittolaite_sbportti;
    sb_irq = soittolaite_sbirq;
    sb_dma = soittolaite_sbdma;
    if (sb_irq & ~15) abort();
    if (sb_dma & ~7) abort();
    if (sb_irq == 2) sb_irq = 9;
    soittolaite_fmportti = IO_SB + 8;

    result = alusta_adlib();
    if (result)
        return result;
#if SB_PCM
    result = dma_alloc(&dmabuf, 8000);
    if (result)
        return result;
    pcm_enabled = reset_sb_dsp(0) == 0xaa;
#else
    pcm_enabled = 0;
#endif
    disable_fm = 0;
    return result;
}

void soitto_paivita_sb(void) {
    soitto_paivita_adlib();
}

void soitto_komento_sb(short *c, short hwc,
                    unsigned char k, unsigned char x) {
    if (disable_fm) return;
    soitto_komento_adlib(c, hwc, k, x);
}

void soitto_toista_sb(int m) {
#if SB_PCM
    if (pcm_enabled) {
        switch (m) {
        case AANI_TUPLAUS_HAVITTY:
            soitto_pysayta_adlib();
            disable_fm = 1;
            pcm_play(pcm_gosh, pcm_gosh_n, 8000);
            return;
        default:
            disable_fm = 0;
        }
    }
#endif
    soitto_toista_adlib(m);
}

void soitto_pysayta_sb(void) {
    pcm_stop();
    soitto_pysayta_adlib();
}

void soitto_lopeta_sb(void) {
    pcm_stop();
    sb_write(0xd3);
    reset_sb_dsp(1);
    soitto_lopeta_adlib();
    dma_free(&dmabuf);
}
