
#ifndef _VALOT_H
#define _VALOT_H

#include "VGA.H"

struct valot {
    char valitse_1;
    char valitse_2;
    char valitse_3;
    char valitse_4;
    char valitse_5;
    char tuplaus;
    char tuplaus_pieni;
    char tuplaus_suuri;
    char panos;
    char voitot;
    char jako;
};

extern struct valot valot;
extern int valo_vilkku;
extern int valo_vilkku_laskuri;
#define VALO_VILKKU_PITUUS 32

#define VALO_POIS(valo) (valo = 0)
#define VALO_VILKKU(valo,n) (valo = n)
#define VALO_VILKKU_ALUSTA() (valo_vilkku_laskuri = 1, valo_vilkku = 6)
#define VALO_PAALLE(valo) (valo = 7)
#define VALO_VILKKUMASSA(valo) (valo == 1 || valo == 2)

void alusta_valot(void);
void paivita_valot(void);
void kaikki_valot_pois(void);
void piirra_palkki_napit(void);
void piirra_palkki_napit_eka(void);
void valot_efekti(void);

#endif
