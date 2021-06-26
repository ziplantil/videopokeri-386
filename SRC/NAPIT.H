
#ifndef _NAPIT_H
#define _NAPIT_H

struct napit {
    char mika_tahansa;
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
    char lopeta;
};

extern struct napit napit;

void lue_napit(void);

#endif
