
#ifndef _PELI_H
#define _PELI_H

#include "KORTTI.H"

enum Pelitila {
    /* odottaa ett„ pelaaja tekee jotain */
    T_PANOS,    /* aivan alussa */
    T_VALITSE,  /* pelaaja valitsee kortit */
    T_VOITTO,   /* k„si jolla voitti, uutta panosta tai tuplaus */
    T_TUPLAUS,  /* pieni vai iso? */
    T_TUPLAOK,  /* voitit tuplauksen, uusi tuplaus? */
    T_TUPLAEI,  /* v„„rin, menet„t rahat. panosta */
    
    /* automaattitiloja */
    T_JAKO1,    /* kortit ker„t„„n */
    T_JAKO2,    /* pakan sekoitus */
    T_JAKO3,    /* pakka jakaa kortit */

    T_UJAKO1,   /* valitsemattomat pudotetaan */
    T_UJAKO2,   /* uudet kortit jaetaan */
    T_UJAKO3,   /* tinnitys j„„vistyy */
    T_UJAKO4,   /* pakka lent„„ takaisin */
    
    T_TUPLA1,   /* kortit/kortti ker„t„„n */
    T_TUPLA2,   /* panos tuplataan */
    T_TUPLA3,   /* pakan sekoitus */
    T_TUPLA4,   /* kortin jako */

    T_VSIIRTO,  /* voiton siirto */
    T_KONKKA,   /* rahat loppu */

    /* esittelytilat */
    T_ESIT0,    /* korttien selki„ pitkin ruutua */
    T_ESIT1,    /* kortit liukuvat alas ruutua */
    T_ESIT2,    /* k„sien esittely */
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
extern char jatka;
extern char jokeri;
extern unsigned long tick;
extern unsigned long anim;
extern char tickskip;
extern char frameskip;
extern char alapalkki_fade;

void piirra_valinnat(char paivita);
void piirra_alapalkki_voitto(char paivita);
void piirra_alapalkki_vain_voitto(char paivita);

void aanitesti(void);
void alusta_tila(enum Pelitila tila);
void aloita_peli(int saldo);
void aja_peli(void);

extern const char *pelitila_nimi(enum Pelitila t);

#endif
