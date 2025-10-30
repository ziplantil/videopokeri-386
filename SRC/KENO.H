
#ifndef _KENO_H
#define _KENO_H

enum PelitilaKeno {
    /* odottaa että pelaaja tekee jotain */
    TK_VALINTA,  /* pelaaja valitsee kortit */
    TK_PANOS,    /* jakojen välissä ilman voittoa */
    TK_VOITTO,   /* käsi jolla voitti, uutta panosta tai tuplaus */
    TK_TUPLAUS,  /* pieni vai iso? */
    TK_TUPLAOK,  /* voitit tuplauksen, uusi tuplaus? */
    TK_TUPLAEI,  /* väärin, menetät rahat. panosta */
    
    /* automaattitiloja */
    TK_SIIRTO0A, /* näyttää kortit valintaa varten */
    TK_SIIRTO0B, /* näyttää kortit valintaa varten */
    TK_SIIRTO1,  /* valitsemattomat kortit poistetaan */
    TK_SIIRTO2,  /* valitut kortit siirtyy yläreunaan */
    TK_SIIRTO3A, /* näyttää kortit valintaa varten */
    TK_SIIRTO3B, /* näyttää kortit valintaa varten */
    
    TK_JAKO1,    /* kortit kerätään vasemmalle pakkaan */
    TK_JAKO2,    /* pakan sekoitus */
    TK_JAKO3,    /* pakka jakaa kortit */
    TK_JAKO4,    /* tinnitys jäävistyy */
    TK_JAKO5,    /* odota häviötä */

    TK_PERU1,    /* kortit kerätään vasemmalle pakkaan */
    TK_PERU2,    /* valitut kortit lentävät vasemmalle */

    TK_TUPLA1,   /* kortit kerätään vasemmalle pakkaan */
    TK_TUPLA2,   /* pakan sekoitus */
    TK_TUPLA3,   /* kortin jako */
    TK_TUPLA4,   /* panos tuplataan */

    TK_VSIIRTO,  /* voiton siirto */
    TK_VLOPPU,   /* päävoiton jälkeen */
    TK_KONKKA,   /* rahat loppu */

    /* esittelytilat */
    TK_ESIT,     /* korttien selkiä pitkin ruutua */

    TK_HUOLTO,   /* hae pelinhoitaja */
};

extern enum PelitilaKeno kenotila;
extern int keno_valintoja_max;
extern const int *keno_kertoimet_kaikki;

void keno_piirra_alapalkki_voitto(char paivita);
void keno_piirra_alapalkki_vain_voitto(char paivita);

void keno_alusta_tila(enum PelitilaKeno tila);
void keno_aloita_peli(int saldo);
void keno_aja_peli(void);

#endif
