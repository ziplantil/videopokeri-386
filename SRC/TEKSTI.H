
#ifndef _TEKSTI_H
#define _TEKSTI_H

#include "RUUTU.H"

extern int tekstivali;

void piirra_levea_luku(int x, int y, int vari,
                    unsigned int n, int paivita);
void piirra_levea_luku_oikea(int x, int y, int vari,
                    unsigned int n, int paivita);
void piirra_teksti(int x, int y, int vari,
                    int lihavoi, const char *t, int paivita);
void piirra_teksti_oikea(int x, int y, int vari,
                    int lihavoi, const char *t, int paivita);
void piirra_teksti_keski(int x, int y, int vari,
                    int lihavoi, const char *t, int paivita);
void piirra_luku(int x, int y, int vari,
                    int lihavoi, unsigned int n, int paivita);
void piirra_luku_oikea(int x, int y, int vari,
                    int lihavoi, unsigned int n, int paivita);
void piirra_luku_keski(int x, int y, int vari,
                    int lihavoi, unsigned int n, int paivita);

void piirra_nappiteksti(int x, int y, const char *t, int vari);

#endif
