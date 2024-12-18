
#ifndef _NAPIT_H
#define _NAPIT_H

struct napit_pokeri {
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

struct napit_keno {
    char valinta;
    char ylos;
    char alas;
    char vasen;
    char oikea;
    char tuplaus;
    char tuplaus_pieni;
    char tuplaus_suuri;
    char panos;
    char voitot;
    char jako;
    char peru;
};

union napit_eli {
    struct napit_pokeri pokeri;
    struct napit_keno keno;
};

struct napit_kaikki {
    union napit_eli p;
    char mika_tahansa;
    char lopeta;
    char lisaa_panos_1;
    char lisaa_panos_5;
};

extern struct napit_kaikki napit_kaikki;

void lue_napit(void);

#endif
