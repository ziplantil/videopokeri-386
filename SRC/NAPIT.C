
#include <dos.h>
#include <string.h>
#include <bios.h>
#include "NAPIT.H"

struct napit napit = { 0 };

void lue_napit(void) {
    unsigned short s;
    memset(&napit, 0, sizeof(napit));
    while (_bios_keybrd(_KEYBRD_READY)) {
        s = _bios_keybrd(_KEYBRD_READ) >> 8;
        napit.mika_tahansa = 1;
        switch (s)
        {
        case 0x01:
        case 0x0E:
            napit.lopeta = 1;
            break;
        case 0x10:
            napit.valitse_1 = 1;
            break;
        case 0x11:
            napit.valitse_2 = 1;
            break;
        case 0x12:
            napit.valitse_3 = 1;
            break;
        case 0x13:
            napit.valitse_4 = 1;
            break;
        case 0x14:
            napit.valitse_5 = 1;
            break;
        case 0x1e:
            napit.tuplaus = 1;
            break;
        case 0x1f:
            napit.tuplaus_pieni = 1;
            break;
        case 0x20:
            napit.tuplaus_suuri = 1;
            break;
        case 0x21:
            napit.panos = 1;
            break;
        case 0x22:
            napit.voitot = 1;
            break;
        case 0x23:
            napit.jako = 1;
            break;
        }
    }
}
