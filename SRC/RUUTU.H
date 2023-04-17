
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

char alusta_ruutu(unsigned long *koko);
void piirra_suorakulmio(short x, short y, short w, short h, short vari);
void piirra_suorakulmio2(short x, short y, short w, short h, short vari);
void piirra_kuva_rajaa_y(short x, short y, short w, short h, short rh,
                        const unsigned char* kuva, short mh);
void piirra_kuva_kortti(short x, short y, short w, short h,
                        const unsigned char* kuva);
void piirra_kuva_rajaa(short tx, short ty, short tw, short th,
                        short sx, short sy, short sw, short sh,
                        const unsigned char* kuva);
void piirra_kuva(short x, short y, short w, short h, const unsigned char* kuva);
void piirra_kuva_maski(short x, short y, short w, short h,
                        const unsigned char* kuva, const unsigned char* maski);
void piirra_kuva_vierita(short x, short y, short w, short h,
                const unsigned char* kuva, short h1, short h2, short sy);
void sailyta_kuva(short x, short y, short w, short h, unsigned char* kuva);
void piirra_kadet(char hohda_aina);
void vilkuta_voittokatta(void);
void piilota_kadet(void);
void paivita_alue(unsigned short x, unsigned short y,
            unsigned short w, unsigned short h);
void tyhjenna_ruutu(void);
void piirra_tausta(void);
void valmistele_ruutu(void);
void valmistele_ruutu_peli(void);
void valmistele_ruutu_esittely(void);
short ruudun_vaihto(short tila, char oikeesti);
void vaihda_normaaliin_palettiin(void);
void vaihda_liilaan_palettiin(void);
void ruudun_palautus(short tila);
void ruudun_paivitys(void);
void ruudun_piirto(void);
void tahti(void);

#endif
