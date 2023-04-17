
#include <stdlib.h>
#include "POKERI.H"
#include "SOITTO.H"
#include "AANET.H"
#include <assert.h>

/* ks. SOITTO.H */
int soittolaite = 0;
/* Tandy/PCjr: I/O-portti, oletus 0xc0 */
int soittolaite_tandyportti = -1;
/* AdLib: FM-portti, oletus 0x388 */
int soittolaite_fmportti = -1;
/* CMS/Game Blaster: portti, oletus 0x220 */
int soittolaite_cmsportti = -1;
/* Sound Blaster: PCM-portti, oletus 0x220 */
int soittolaite_sbportti = -1;
/* Sound Blaster: IRQ (oletuksena 5) */
int soittolaite_sbirq = -1;
/* Sound Blaster: DMA (oletuksena 1) */
int soittolaite_sbdma = -1;
/* Innovation SSI 2001: SID-portti, oletus 0x280 */
int soittolaite_sidportti = -1;
/* MPU-401-portti, oletus 0x330 */
int soittolaite_mpuportti = -1;

static int pysaytetty = 1;
static int toistaa = 0;

unsigned int soittoaika = 0;
unsigned int soittotempo = 0;

#define OLETUSARVO(x,d) (((x) < 0) ? (d) : (x))
#define OLETUSARVO_ASETA(x,d) (x = OLETUSARVO(x, d))

struct soittokanava {
    const unsigned short *kohta;
    const unsigned short *alku;
    const unsigned short *paluu;
    unsigned long seuraava;
    unsigned short soitin;
    short laitekanava[2];
    unsigned short viime_komento;
};

struct soittokanava kanavat[8];
int kanavia = 0;

int kanavia_max_musa = 0;
int kanavia_max_sfx = 0;

/**********************************/
/*  YLEISIÄ                       */
/**********************************/

void soitto_paivita_0() { }
void soitto_komento_0(short *c, short h, unsigned char k, unsigned char x) { }
void soitto_toista_0(int a) { }
void soitto_pysayta_0() { }
void soitto_lopeta_0() { }

void (*_soitto_paivita)(void)   = &soitto_paivita_0;
void (*_soitto_komento)(short *, short, unsigned char, unsigned char)
                                = &soitto_komento_0;
void (*_soitto_toista)(int)     = &soitto_toista_0;
void (*_soitto_pysayta)(void)   = &soitto_pysayta_0;
void (*_soitto_lopeta)(void)    = &soitto_lopeta_0;

/**********************************/
/*  SISÄINEN KAIUTIN              */
/**********************************/
/* ks. S_PC.C */

void soitto_paivita_pc(void);
void soitto_komento_pc(short *c, short hwc, unsigned char k, unsigned char x);
void soitto_toista_pc(int m);
void soitto_pysayta_pc(void);
void soitto_lopeta_pc(void);

/**********************************/
/*  TANDY / PCJR                  */
/**********************************/
/* ks. S_CMS.C */

int alusta_tandy(void);
void soitto_paivita_tandy(void);
void soitto_komento_tandy(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_tandy(int m);
void soitto_pysayta_tandy(void);
void soitto_lopeta_tandy(void);

/**********************************/
/*  ADLIB                         */
/**********************************/
/* ks. S_ADLIB.C */

int alusta_adlib(void);
void soitto_paivita_adlib(void);
void soitto_komento_adlib(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_adlib(int m);
void soitto_pysayta_adlib(void);
void soitto_lopeta_adlib(void);

/**********************************/
/*  SOUND BLASTER                 */
/**********************************/
/* ks. S_SB.C */

int alusta_sb(void);
void soitto_paivita_sb(void);
void soitto_komento_sb(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_sb(int m);
void soitto_pysayta_sb(void);
void soitto_lopeta_sb(void);

/**********************************/
/*  CMS / GAME BLASTER            */
/**********************************/
/* ks. S_CMS.C */

int alusta_cms(void);
void soitto_paivita_cms(void);
void soitto_komento_cms(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_cms(int m);
void soitto_pysayta_cms(void);
void soitto_lopeta_cms(void);

/**********************************/
/*  INNOVATION SSI 2001           */
/**********************************/
/* ks. S_INNOVA.C */

int alusta_innova(void);
void soitto_paivita_innova(void);
void soitto_komento_innova(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_innova(int m);
void soitto_pysayta_innova(void);
void soitto_lopeta_innova(void);

/**********************************/
/*  MPU-401 MIDI                  */
/**********************************/
/* ks. S_MPU.C */

int alusta_mpu(int instr_mode);
void soitto_paivita_mpu(void);
void soitto_komento_mpu(short *c, short hwc,
                    unsigned char k, unsigned char x);
void soitto_toista_mpu(int m);
void soitto_pysayta_mpu(void);
void soitto_lopeta_mpu(void);

/**********************************/
/*  YLEISET                       */
/**********************************/

#define SOITTOFUNKTIOT(n) do {                              \
        _soitto_paivita = &soitto_paivita_##n##;            \
        _soitto_komento = &soitto_komento_##n##;            \
        _soitto_toista = &soitto_toista_##n##;              \
        _soitto_pysayta = &soitto_pysayta_##n##;            \
        _soitto_lopeta = &soitto_lopeta_##n##;              \
    } while (0)

int alusta_soitto(void) {
    switch (soittolaite) {
    case 0:
        SOITTOFUNKTIOT(0);
        kanavia_max_musa = kanavia_max_sfx = 0;
        return 0;
    case 1:
        SOITTOFUNKTIOT(pc);
        kanavia_max_musa = kanavia_max_sfx = 1;
        return 0;
    case 2:
        OLETUSARVO_ASETA(soittolaite_tandyportti, 0xc0);
        if (alusta_tandy()) {
            soittolaite = 0;
            return 1;
        }
        SOITTOFUNKTIOT(tandy);
        kanavia_max_musa = kanavia_max_sfx = 4;
        return 0;
    case 3:
        OLETUSARVO_ASETA(soittolaite_fmportti, 0x388);
        if (alusta_adlib()) {
            soittolaite = 0;
            return 1;
        }
        SOITTOFUNKTIOT(adlib);
        kanavia_max_musa = 6, kanavia_max_sfx = 6;
        return 0;
    case 4:
        OLETUSARVO_ASETA(soittolaite_cmsportti, 0x220);
        if (alusta_cms()) {
            soittolaite = 0;
            return 1;
        }
        SOITTOFUNKTIOT(cms);
        kanavia_max_musa = 4, kanavia_max_sfx = 4;
        return 0;
    case 5:
        OLETUSARVO_ASETA(soittolaite_fmportti, 0x388);
        OLETUSARVO_ASETA(soittolaite_sbportti, 0x220);
        OLETUSARVO_ASETA(soittolaite_sbirq, 5);
        OLETUSARVO_ASETA(soittolaite_sbdma, 1);
        if (alusta_sb()) {
            soittolaite = 0;
            return 1;
        }
        SOITTOFUNKTIOT(sb);
        kanavia_max_musa = 6, kanavia_max_sfx = 6;
        return 0;
    case 6:
        OLETUSARVO_ASETA(soittolaite_sidportti, 0x280);
        if (alusta_innova()) {
            soittolaite = 0;
            return 1;
        }
        SOITTOFUNKTIOT(innova);
        kanavia_max_musa = 4, kanavia_max_sfx = 4;
        return 0;
    case 7:
    case 8:
        OLETUSARVO_ASETA(soittolaite_mpuportti, 0x330);
        if (alusta_mpu(soittolaite - 7)) {
            soittolaite = 0;
            return 1;
        }
        SOITTOFUNKTIOT(mpu);
        kanavia_max_musa = 6, kanavia_max_sfx = 6;
        return 0;
    default:
        return 1;
    }
}

const char *soittolaitteen_nimi(int soittolaite) {
    switch (soittolaite) {
    case 0:         return english ? "(no sound)" : "(ei ääniä)";
    case 1:         return english ? "PC speaker" : "sisäinen kaiutin";
    case 2:         return "Tandy";
    case 3:         return "AdLib";
    case 4:         return "Creative Music System";
    case 5:         return "Sound Blaster";
    case 6:         return "Innovation SSI 2001";
    case 7:         return "MPU-401 / General MIDI";
    case 8:         return "Roland MT-32";
    default:        abort(); return NULL;
    }
}

void paivita_soitto(void) {
    short i;
    unsigned long v;
    unsigned short komento;
    const unsigned short *p;
    unsigned char k, x;
    if (!soittolaite) return;

    if (toistaa) {
        for (i = 0; i < kanavia; ++i) {
            p = kanavat[i].kohta;
            if (!p) continue;
            v = kanavat[i].seuraava;
            while (soittoaika >= v) {
                komento = *++p;
                k = (komento >> 8) & 0xFF; x = komento & 0xFF;
                ++p;
                switch (k) {
                case COMMAND_RESTART: {
                    int j;
                    assert(i == 0);
                    toistaa = kanavia;
                    for (j = 0; j < kanavia; ++j) {
                        kanavat[j].seuraava =
                                *(kanavat[j].kohta = kanavat[j].alku) << 8;
                    }
                    v = *(p = kanavat[i].alku) << 8;
                    soittoaika &= 0xFF;
                    continue;
                }
                case COMMAND_END:
                    --toistaa;
                    p = NULL;
                    goto finish;
                case COMMAND_INSTRUMENT:
                    if (kanavat[i].soitin == x)
                        break;
                    kanavat[i].soitin = x;
                    /* fall-through */
                default:
                    if (!(k & 0x80)) {
                        (*_soitto_komento)(&kanavat[i].laitekanava[0],
                                        kanavat[i].laitekanava[1], k, x);
                    }
                }
                v += (unsigned long)*p << 8ul;
            }
finish:
            kanavat[i].kohta = p;
            kanavat[i].seuraava = v;
        }
    }
    (*_soitto_paivita)();
    soittoaika += soittotempo;
}

struct aanitieto {
    const unsigned short **data;
    int tempo;
};

#define MUSA_VOITTO 1
#define MUSA_TUPLAUS 2
#define MUSA_PAAVOITTO 3
#define MUSA_KOPUTUS 4
#define MUSA_KONKKA 5
#define AANI_KORTTI_LIUKUU 6
#define AANI_SEKOITA 7
#define AANI_KORTTI_JAETAAN 8
#define AANI_KORTTI_KAANTYY 9
#define AANI_VALINTA 10
#define AANI_KORTTIEN_PUDOTUS 11
#define AANI_UUSI_KORTTI_ASETTUU 12
#define AANI_VOITONMAKSU 13
#define AANI_TUPLAUS_HAVITTY 14
#define AANI_ALOITA 15
#define AANI_PANOS1 16
#define AANI_PANOS2 17
#define AANI_PANOS3 18
#define AANI_PANOS4 19
#define AANI_PANOS5 20

const struct aanitieto aanitiedot[] = {      
    { A_tyhja,    120 },                     /* - */
    { A_Mvoitto,   96 },                     /* MUSA_VOITTO */
    { A_Mtuplaus, 120 },                     /* MUSA_TUPLAUS */
    { A_Mtsajaja,  48 },                     /* MUSA_PAAVOITTO */
    { A_Mkoputus, 144 },                     /* MUSA_KOPUTUS */
    { A_Mkonkka,   72 },                     /* MUSA_KONKKA */
    { A_Aliuku,   120 },                     /* AANI_KORTTI_LIUKUU */
    { A_Asekoita, 240 },                     /* AANI_SEKOITA */
    { A_Aliuku,   120 },                     /* AANI_KORTTI_JAETAAN */
    { A_Akaantyy, 120 },                     /* AANI_KORTTI_KAANTYY */
    { A_Avalinta, 120 },                     /* AANI_VALINTA */
    { A_Apudotus, 120 },                     /* AANI_KORTTIEN_PUDOTUS */
    { A_Aasettuu, 120 },                     /* AANI_UUSI_KORTTI_ASETTUU */
    { A_Amaksu,   120 },                     /* AANI_VOITONMAKSU */
    { A_Aperkele, 200 },                     /* AANI_TUPLAUS_HAVITTY */
    { A_Aaloita,  120 },                     /* AANI_ALOITA */
    { A_Apanos1,  120 },                     /* AANI_PANOS1 */
    { A_Apanos2,  120 },                     /* AANI_PANOS2 */
    { A_Apanos3,  120 },                     /* AANI_PANOS3 */
    { A_Apanos4,  120 },                     /* AANI_PANOS4 */
    { A_Apanos5,  120 },                     /* AANI_PANOS5 */
};
#define AANET_N (sizeof(aanitiedot) / sizeof(aanitiedot[0]))

void varaa_kanavat(const unsigned short **aani, int offset, int max) {
    int i = offset, n = 0;

    while (n < max && *aani) {
        kanavat[n].alku = kanavat[n].kohta = *aani;
        kanavat[n].laitekanava[0] = i;
        kanavat[n].laitekanava[1] = i;
        kanavat[n].seuraava = (unsigned long)*kanavat[n].kohta << 8ul;
        kanavat[n].soitin = -1;
        kanavat[n].viime_komento = 0;
        ++aani; ++n; ++i;
    }

    toistaa = kanavia = n;
}

void toista_musiikki(int aani, int tempo) {
    if (!soittolaite) return;
    if (aani < AANET_N) {
        if (toistaa)
            pysayta_aanet();
        (*_soitto_toista)(aani);
        soittoaika = 0;
        pysaytetty = 0;
        soittotempo = OLETUSARVO(tempo, aanitiedot[aani].tempo);
        soittotempo = (soittotempo * 1024) / 60;
        varaa_kanavat(aanitiedot[aani].data, 0, kanavia_max_musa);
    }
}

void toista_musiikki_oletus(int aani) {
    toista_musiikki(aani, -1);
}

void toista_aani(int aani) {
    if (!soittolaite) return;
    if (aani < AANET_N) {
        if (toistaa)
            pysayta_aanet();
        (*_soitto_toista)(aani);
        soittoaika = 0;
        pysaytetty = 0;
        soittotempo = aanitiedot[aani].tempo;
        soittotempo = (soittotempo * 1024) / 60;
        varaa_kanavat(aanitiedot[aani].data, 0, kanavia_max_sfx);
    }
}

unsigned char toistaa_aanta(void) {
    return toistaa;
}

void pysayta_aanet(void) {
    if (pysaytetty) return;
    (*_soitto_pysayta)();
    kanavia = 0;
    toistaa = 0;
    pysaytetty = 1;
}

void lopeta_soitto(void) {
    (*_soitto_lopeta)();
    kanavia = 0;
    toistaa = 0;
}
