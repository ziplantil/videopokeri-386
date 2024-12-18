
#include <dos.h>
#include <string.h>
#include <bios.h>
#include "NAPIT.H"
#include "POKERI.H"

struct napit_kaikki napit_kaikki = { { { 0 } } };

void lue_napit(void) {
    unsigned short s;
    memset(&napit_kaikki, 0, sizeof(napit_kaikki));
    while (_bios_keybrd(_KEYBRD_READY)) {
        s = _bios_keybrd(_KEYBRD_READ) >> 8;
        napit_kaikki.mika_tahansa = 1;
        switch (s)
        {
        case 0x01:
        case 0x0E:
            napit_kaikki.lopeta = 1;
            break;
        case 0x31:
            napit_kaikki.lisaa_panos_1 = 1;
            break;
        case 0x32:
            napit_kaikki.lisaa_panos_5 = 1;
            break;
        }
        if (keno) {
            switch (s)
            {
            case 0x10:
                napit_kaikki.p.keno.tuplaus = 1;
                break;
            case 0x11:
                napit_kaikki.p.keno.voitot = 1;
                break;
            case 0x12:
                napit_kaikki.p.keno.valinta = 1;
                break;
            case 0x15:
                napit_kaikki.p.keno.panos = 1;
                break;
            case 0x1e:
                napit_kaikki.p.keno.tuplaus_pieni = 1;
                break;
            case 0x1f:
                napit_kaikki.p.keno.tuplaus_suuri = 1;
                break;
            case 0x20:
                napit_kaikki.p.keno.peru = 1;
                break;
            case 0x23:
                napit_kaikki.p.keno.jako = 1;
                break;
            case 0x48:
                napit_kaikki.p.keno.ylos = 1;
                break;
            case 0x50:
                napit_kaikki.p.keno.alas = 1;
                break;
            case 0x4B:
                napit_kaikki.p.keno.vasen = 1;
                break;
            case 0x4D:
                napit_kaikki.p.keno.oikea = 1;
                break;
            }
        } else {
            switch (s)
            {
            case 0x10:
                napit_kaikki.p.pokeri.valitse_1 = 1;
                break;
            case 0x11:
                napit_kaikki.p.pokeri.valitse_2 = 1;
                break;
            case 0x12:
                napit_kaikki.p.pokeri.valitse_3 = 1;
                break;
            case 0x13:
                napit_kaikki.p.pokeri.valitse_4 = 1;
                break;
            case 0x14:
                napit_kaikki.p.pokeri.valitse_5 = 1;
                break;
            case 0x1e:
                napit_kaikki.p.pokeri.tuplaus = 1;
                break;
            case 0x1f:
                napit_kaikki.p.pokeri.tuplaus_pieni = 1;
                break;
            case 0x20:
                napit_kaikki.p.pokeri.tuplaus_suuri = 1;
                break;
            case 0x21:
                napit_kaikki.p.pokeri.panos = 1;
                break;
            case 0x22:
                napit_kaikki.p.pokeri.voitot = 1;
                break;
            case 0x23:
                napit_kaikki.p.pokeri.jako = 1;
                break;
            }
        }
    }
}
