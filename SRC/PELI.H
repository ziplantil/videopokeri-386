
#ifndef _PELI_H
#define _PELI_H

#include "KORTTI.H"

enum Pelitila {
    /* odottaa että pelaaja tekee jotain */
    T_RAHAA,    /* pelaaja lisännyt rahaa */
    T_PANOS,    /* pelaaja valitsee panoksen */
    T_VALITSE,  /* pelaaja valitsee kortit */
    T_VOITTO,   /* käsi jolla voitti, uutta panosta tai tuplaus */
    T_TUPLAUS,  /* pieni vai iso? */
    T_TUPLAOK,  /* voitit tuplauksen, uusi tuplaus? */
    T_TUPLAEI,  /* väärin, menetät rahat. panosta */
    
    /* automaattitiloja */
    T_JAKO1,    /* kortit kerätään */
    T_JAKO2,    /* pakan sekoitus */
    T_JAKO3,    /* pakka jakaa kortit */

    T_UJAKO0,   /* odota ennen pudotusta */
    T_UJAKO1,   /* valitsemattomat pudotetaan */
    T_UJAKO2,   /* uudet kortit jaetaan */
    T_UJAKO3,   /* tinnitys jäävistyy */
    T_UJAKO4,   /* pakka lentää takaisin */
    
    T_TUPLA1,   /* kortit/kortti kerätään */
    T_TUPLA2,   /* panos tuplataan */
    T_TUPLA3,   /* pakan sekoitus */
    T_TUPLA4,   /* kortin jako */

    T_VSIIRTO,  /* voiton siirto */
    T_VLOPPU,   /* päävoiton jälkeen */
    T_KONKKA,   /* rahat loppu */

    /* esittelytilat */
    T_ESIT0,    /* korttien selkiä pitkin ruutua */
    T_ESIT1,    /* kortit liukuvat alas ruutua */
    T_ESIT2,    /* käsien esittely */

    T_HUOLTO,   /* hae pelinhoitaja */
};

struct Pelikortti {
    short x;
    short y;
    kortti_t kortti;
    char selka;
    char nakyva;
    char muutettu;
};

extern enum Pelitila tila;

extern int panos;
extern long pelit;
extern long voitot;
extern long lisarahat;
extern char jatka;
extern char jokeri;
extern unsigned long tick;
extern unsigned long anim;
extern char tickskip;
extern char frameskip;
extern char alapalkki_fade;
extern int lisarahat_ok;

void piirra_valinnat(char paivita);
void piirra_alapalkki_voitto(char paivita);
void piirra_alapalkki_vain_voitto(char paivita);
void lisaa_panos(unsigned panos);

void aanitesti(void);
void alusta_tila(enum Pelitila tila);
void aloita_peli(int saldo);
void aja_peli(void);

#endif
