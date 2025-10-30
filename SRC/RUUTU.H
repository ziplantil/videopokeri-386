
#ifndef _RUUTU_H
#define _RUUTU_H

#include "POKERI.H"

#define PPB 8
#define PLANE_WIDTH 640
#define PLANE_HEIGHT 480
#define GAREA_WIDTH PLANE_WIDTH
#define GAREA_HEIGHT 400
#define CARDAREA_HEIGHT (400 - 32)
#define STRIDE (PLANE_WIDTH / PPB)
#define GAREA_SIZE (STRIDE * GAREA_HEIGHT)
#define PLANE_SIZE (STRIDE * PLANE_HEIGHT)

extern unsigned char *taso0;
extern unsigned char *taso1;
extern unsigned char *taso2;
extern unsigned char *taso3;
extern unsigned char *valittu_kuva;
extern unsigned char *tuplaus_viesti;
extern unsigned char *selka_cache;
extern unsigned char *kortti_cache;
extern unsigned char *kortti_cache1;
extern unsigned char *kortti_cache2;
extern unsigned char *kortti_cache3;
extern unsigned char *kortti_cache4;

extern char _paivita_palkki;
extern char _paivita_ylapalkki;
extern char _paivita_ylapalkki_voitot;
extern char _paivita_alapalkki;
extern char _paivita_alapalkki_voitto;
extern char _paivita_alapalkki_valinnat;

#define ALAPALKKI_Y (GAREA_HEIGHT - 32)

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define paivita_palkki() (_paivita_palkki = 1)
#define paivita_ylapalkki() (_paivita_ylapalkki = 1)
#define paivita_ylapalkki_voitot() (_paivita_ylapalkki_voitot = 1)
#define paivita_alapalkki() (_paivita_alapalkki = 1)
#define paivita_alapalkki_voitto() (_paivita_alapalkki_voitto = 1)
#define paivita_alapalkki_valinnat() (_paivita_alapalkki_valinnat = 1)

#define taso0_MAX (taso0 + GAREA_SIZE)

extern int tuplaus_banner_x;

// varaa muistin ruudulle ym.
char alusta_ruutu(unsigned long *koko);
// piirtää suorakulmion, ei tarkista koordinaatteja
void piirra_suorakulmio(short x, short y, short w, short h, short vari);
// piirtää suorakulmion, tarkistaa koordinaatit
void piirra_suorakulmio2(short x, short y, short w, short h, short vari);
// piirtää kuvan Y-koordinaatteja rajaten. x, y ovat kohdekoordinaatit,
// w on leveys, h on haluttu korkeus, rh on lähdekuvan oikea korkeus,
// mh on rajattava maksimi Y-koordinaatti
// x:n ja w:n oltava 8:lla jaollisia
void piirra_kuva_rajaa_y(short x, short y, short w, short h, short rh,
                        const unsigned char* kuva, short mh);
// piirtää kuvan pelialueelle, rajaa Y-koordinaatit sen mukaisesti
// x:n ja w:n oltava 8:lla jaollisia
void piirra_kuva(short x, short y, short w, short h, const unsigned char* kuva);
// piirtää kuvan korttialueelle, rajaa Y-koordinaatit sen mukaisesti
// x:n ja w:n oltava 8:lla jaollisia
void piirra_kuva_kortti(short x, short y, short w, short h,
                        const unsigned char* kuva);
// piirtää kuvan rajattuna: kohde (tx, ty), koko (tw, th)
// kuvan alkuperäinen koko (sw, sh) ja lähdekoordinaatit (sx, sy)
// sx:n, tx:n, sw:n, tw:n oltava 8:lla jaollisia
void piirra_kuva_rajaa(short tx, short ty, short tw, short th,
                       short sx, short sy, short sw, short sh,
                       const unsigned char* kuva);
// piirtää kuvan rajattuna: kohde (tx, ty), koko (tw, th)
// kuvan alkuperäinen koko (sw, sh) ja lähdekoordinaatit (sx, sy)
// sx, tx, sw, tw ei tarvitse olla 8:lla jaollisia
// transp jättää mustat pikselit piirtämättä
void piirra_kuva_suikale(short tx, short ty, short tw, short th,
                         short sx, short sy, short sw, short sh,
                         const unsigned char* kuva, unsigned char transp);
// piirtää kuvaa maskilla; esim. 0xF0 piirtää jokaisesta 8 pikselin rivistä
// x:n ja w:n oltava 8:lla jaollisia
void piirra_kuva_maski(short x, short y, short w, short h,
                       const unsigned char* kuva, const unsigned char* maski);
// piirtää kuvan vieritettynä ylös/alas
// suurempi sy siirtää lähdekuvaa alaspäin
// h1, h2 rajaa Y-koordinaatit
// x:n ja w:n oltava 8:lla jaollisia
void piirra_kuva_vierita(short x, short y, short w, short h,
                const unsigned char* kuva, short h1, short h2, short sy);
// tallentaa kuvan puskuriin
void sailyta_kuva(short x, short y, short w, short h, unsigned char* kuva);

void piirra_alapalkki(void);
// piirtää tuplausbannerin (A-6 tai 8-K)
void paivita_tuplaus_alapalkki(void);
// ilmoittaa että alue ruudulla päivitettävä
void paivita_alue(unsigned short x, unsigned short y,
                  unsigned short w, unsigned short h);
// tyhjentää ruudun
void tyhjenna_ruutu(void);
// piirtää taustan
void piirra_tausta(void);
// valmistelee ruudut: piirtää palkit, ym.
void valmistele_ruutu(void);
// valmistelee ruudut pelitilaa varten: piirtää palkit, ym.
void valmistele_ruutu_peli(void);
// valmistelee ruudut esittelytilaa varten: piirtää palkit, ym.
void valmistele_ruutu_esittely(void);

// vaihtaa paletteja (näytön vilkutus)
void vaihda_normaaliin_palettiin(void);
void vaihda_liilaan_palettiin(void);
void vaihda_keltaiseen_palettiin(void);
void vaihda_punaiseen_palettiin(void);
void vaihda_vihreaan_palettiin(void);

// vaihtaa ruudun näyttötilaa, palauttaa vanhan tilan
short ruudun_vaihto(short tila, char oikeesti);
// palauttaa ruudun eri tilaan
void ruudun_palautus(short tila);
// päivittää peliin liittyviä asioita ruudulla
void ruudun_paivitys(void);
// piirtää ruudut VGA-muistiin
void ruudun_piirto(void);
// odottaa pystytahdistusta
void tahti(void);

#endif
