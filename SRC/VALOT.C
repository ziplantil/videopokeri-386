
#include <dos.h>
#include <string.h>
#include "VALOT.H"
#include "RUUTU.H"
#include "TEKSTI.H"

struct valot valot = { 0 };
int valo_vilkku = 6;
int valo_vilkku_laskuri = 1;
static int valo_efekti = 0;
static int valo_efekti_laskuri = 0;
#define VALO_VILKKU_PITUUS 32

struct valonappi {
    char *tila;
    short x;
    short y;
    const char *l1;
    const char *l2;
    const char *l3;
    unsigned char vari_on;
    unsigned char vari_off;
};

#define VALO_PUNAINEN   0xcf, 0x04
#define VALO_ORANSSI    0xce, 0x04
#define VALO_KELTAINEN  0x6e, 0x06
#define VALO_VIHREA     0x2f, 0x02
#define VALO_SININEN    0x9b, 0x01

const struct valonappi valonapit_eng[] = {
    { &valot.valitse_1,  6, 52, 
                        "  HOLD  ",
                        "   1    ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_2, 18, 52, 
                        "  HOLD  ",
                        "   2    ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_3, 30, 52, 
                        "  HOLD  ",
                        "   3    ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_4, 42, 52, 
                        "  HOLD  ",
                        "   4    ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_5, 54, 52, 
                        "  HOLD  ",
                        "   5    ",
                        "        ", VALO_PUNAINEN },
    { &valot.tuplaus,  6, 56, 
                        "        ",
                        " DOUBLE ",
                        "        ", VALO_ORANSSI },
    { &valot.tuplaus_pieni, 18, 56, 
                        "  LOW   ",
                        " 1 - 6  ",
                        "        ", VALO_ORANSSI },
    { &valot.tuplaus_suuri, 30, 56, 
                        " HIGH   ",
                        " 8 -13  ",
                        "        ", VALO_ORANSSI },
    { &valot.panos, 42, 56, 
                        "        ",
                        "  BET   ",
                        "        ", VALO_SININEN },
    { &valot.voitot, 54, 56, 
                        "        ",
                        "COLLECT ",
                        "        ", VALO_KELTAINEN },
    { &valot.jako, 66, 56, 
                        "        ",
                        "  DEAL  ",
                        "        ", VALO_VIHREA },
};
const struct valonappi valonapit_fin[] = {
    { &valot.valitse_1,  6, 52, 
                        "VALITSE ",
                        "  1     ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_2, 18, 52, 
                        "VALITSE ",
                        "  2     ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_3, 30, 52, 
                        "VALITSE ",
                        "  3     ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_4, 42, 52, 
                        "VALITSE ",
                        "  4     ",
                        "        ", VALO_PUNAINEN },
    { &valot.valitse_5, 54, 52, 
                        "VALITSE ",
                        "  5     ",
                        "        ", VALO_PUNAINEN },
    { &valot.tuplaus,  6, 56, 
                        "        ",
                        "TUPLAUS ",
                        "        ", VALO_ORANSSI },
    { &valot.tuplaus_pieni, 18, 56, 
                        " PIENI  ",
                        " 1 - 6  ",
                        "        ", VALO_ORANSSI },
    { &valot.tuplaus_suuri, 30, 56, 
                        " SUURI  ",
                        " 8 -13  ",
                        "        ", VALO_ORANSSI },
    { &valot.panos, 42, 56, 
                        "        ",
                        " PANOS  ",
                        "        ", VALO_SININEN },
    { &valot.voitot, 54, 56, 
                        " VOITON ",
                        " MAKSU  ",
                        "        ", VALO_KELTAINEN },
    { &valot.jako, 66, 56, 
                        "        ",
                        "  JAKO  ",
                        "        ", VALO_VIHREA },
};
const struct valonappi *valonapit;
#define VALONAPIT_LKM (sizeof(valonapit_fin) / sizeof(valonapit_fin[0]))

void alusta_valot(void) {
    valonapit = english ? valonapit_eng : valonapit_fin;
}

void kaikki_valot_pois(void) {
    valo_efekti = 0;
    memset(&valot, 0, sizeof(valot));
}

#define OHJE_TEKSTI 0x08

void piirra_palkki_napit_eka(void) {
    piirra_nappiteksti(66, 52, " QWERT  ", OHJE_TEKSTI);
    piirra_nappiteksti(66, 53, " ASDFGH ", OHJE_TEKSTI);
}

void piirra_nappi(int x, int y, const char *s1, const char *s2, const char *s3,
                int vari) {
    piirra_nappiteksti(x, y, s1, vari);
    piirra_nappiteksti(x, y + 1, s2, vari);
    piirra_nappiteksti(x, y + 2, s3, vari);
}

void valot_efekti(void) {
    valo_efekti_laskuri = VALONAPIT_LKM - 1;
    valo_vilkku_laskuri = 1;
    kaikki_valot_pois();
    valo_efekti = 1;
}

void piirra_nappi_struct(const struct valonappi *nappi) {
    piirra_nappi(nappi->x, nappi->y, nappi->l1, nappi->l2, nappi->l3,
            (*nappi->tila & valo_vilkku) ? nappi->vari_on : nappi->vari_off);
};

void piirra_nappi_struct_r(const struct valonappi *nappi) {
    piirra_nappi_struct(nappi);
    paivita_alue(nappi->x * 8, nappi->y * 8, 64, 24);  
};

#define NAPPI piirra_nappi
#define NAPPI_R(x, y, s1, s2, s3, vari) \
    (piirra_nappi(x, y, s1, s2, s3, vari), paivita_alue(x * 8, y * 8, 64, 24))

void piirra_palkki_vilkku(void) {
    short i;
    if (valo_efekti) {
        short l = valo_efekti_laskuri, l2;
        l2 = (l + 1) % VALONAPIT_LKM;
        *valonapit[l].tila = 0;
        *valonapit[l2].tila = 7;
        piirra_nappi_struct_r(&valonapit[l]);
        piirra_nappi_struct_r(&valonapit[l2]);
        valo_efekti_laskuri = l2;
    } else {
        for (i = 0; i < VALONAPIT_LKM; ++i)
            if (VALO_VILKKUMASSA(*valonapit[i].tila))
                piirra_nappi_struct_r(&valonapit[i]);
    }
}

void piirra_palkki_napit(void) {
    short i;
    for (i = 0; i < VALONAPIT_LKM; ++i)
        piirra_nappi_struct(&valonapit[i]);
}

void paivita_valot(void) {
    if (!--valo_vilkku_laskuri) {
        valo_vilkku ^= 3;
        valo_vilkku_laskuri = VALO_VILKKU_PITUUS;
        piirra_palkki_vilkku();
    }
}
