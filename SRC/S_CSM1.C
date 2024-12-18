
#if 0

#include "POKERI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "MUISTI.H"
#include <assert.h>
#include <dos.h>
#include <stdlib.h>
#include <conio.h>

static int IO_PORT = 0;
static unsigned char reg[16] = { 0 };

static unsigned char ay_read(unsigned char reg) {
    outp(IO_PORT + 0, reg);
    return inp(IO_PORT + 1);
}

#define AY_WRITE(r, v) do {    \
        outp(IO_PORT + 0, r);  \
        outp(IO_PORT + 1, v);  \
        reg[r] = v;            \
    } while (0)

static void silence_csm1(void) {
    AY_WRITE(7, 0x3F);
}

static void reset_csm1(void) {
    int i;
    for (i = 0; i <= 15; ++i)
        AY_WRITE(i, 0x00);
}

int alusta_csm1(void) {
    int s, ok;
    unsigned char a1, a2, a3;
    IO_PORT = soittolaite_csm1portti;

    /* testataan löytyyko AY */
    reset_csm1();
    /* Onkos siellä AYtä? */
    AY_WRITE(0x0, 0xA5);
    AY_WRITE(0xC, 0x5A);
    AY_WRITE(0x3, 0x0F);
    a1 = ay_read(0x0);
    a2 = ay_read(0xC);
    a3 = ay_read(0x3);
    ok = a1 == 0xA5 && a2 == 0x5A && a3 == 0x0F;

    return !ok;
}

void soitto_paivita_csm1(void) {

}

void soitto_komento_csm1(short *c, short hwc,
                         unsigned char k, unsigned char x) {
    
}

void soitto_toista_csm1(int m) {
    
}

void soitto_pysayta_csm1(void) {
    silence_csm1();
}

void soitto_lopeta_csm1(void) {
    reset_csm1();
}

#endif
