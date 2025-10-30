
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "POKERI.H"
#include "NAPIT.H"
#include "PELI.H"
#include "KORTTI.H"
#include "VALOT.H"
#include "RUUTU.H"
#include "NAPIT.H"
#include "PAKKA.H"
#include "POYTA.H"
#include "KUVAT.H"
#include "TEKSTI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "MUISTI.H"
#include <assert.h>

enum Pelitila tila; // pelitila. ks. PELI.H
char jatka; // <>0 jatkaa peliä, =0 lopettaa
int panos; // tämänhetkinen panos
long pelit; // pelirahojen määrä
long lisarahat; // paljonko pelaaja on lisännyt rahaa alun jälkeen
long voitot; // voittojen määrä
static int voitto; // voitto tällä kierroksella
char jokeri; // onko jokeri pelissä?
static char voitto_vilkku = 0; // käytetään voittosumman vilkuttamiseen
static char musan_toisto = 0; // toistaako musiikkia?
static char skip_input_read = 0; // jätä napit lukematta, esittelystä tultaessa
static int voitto_disp = 0; // alapalkissa näytettävät voitot
            // esim. voitto_disp kasvaa hitaasti kohti voittoa
static int voitto_hitaus = 0; // kuinka hitaasti lasketaan voittoa näytöllä
            // suuremmat luvut = hitaampi
static char new_mode = 0; // vaihdettiinko juuri pelitilaa?
static char card0zoom = 0; // jos pakka (kortti 0) lentää, päivitä se kahdesti
char tickskip = 0; // montako tickkiä jätetään välistä joka frame
static char tickskip_i = 0; // tickskip-laskuri
char frameskip = 0; // montako framea jätetään välistä joka frame
unsigned long anim = 0; // tilan alussa 0, kasvaa jokaista näytettyä framea
            // kohden, eli tickskip + 1 joka tick
unsigned long tick = 0; // montako tickiä yhteensä
static char tuplaus = 0; // ollaanko tuplaamassa
char alapalkki_fade = 0; // alapalkin väri: 0 = ei voittoa, 8 = voitto (liila),
            // 1-7 = animaatio jossa palkki vähitellen muuttuu liilaksi,
            // 9-15 = animaatio jossa palkin liila väri katoaa
static char alapalkki_vapaaksi = 0; // vaaditaan alapalkkia vapaaksi,
            // eli että valittu-kuvat ovat hävinneet sieltä pois
static char tuplaus_ctr = 0;
            // montako kertaa tuplattu (vaikuttaa musiikin nopeuteen)
static char oli_paavoitto = 0;
            // oliko päävoitto? (vilkuta ruutua, ym.)
static char palswap = 0;
            // ruudun vilkutus päävoiton yhteydessä
static char yks_kerrallaan = 0;
            // pudotetaanko voittokuvat yksi kerrallaan?
static int inactive = 0;
            // laskuri jos käyttäjä ei paina mitään, siirtyy esittelytilaan
static int demo_y = 0;
            // esittelytila: Y-koordinaatti ruudulla
static int demo_hand = 0;
            // esittelytila: esiteltävän käden numero
static int demo_panos;
            // esittelytila: esiteltävä panos
static int koputus_anim;
            // koputusanimaatiossa käytettävä laskuri
int lisarahat_ok = 1; // voiko pelaaja lisätä rahaa pelin aikana

#define napit napit_kaikki.p.pokeri
#define valot valot_eli.pokeri

#define PAKKA_X 16
#define PAKKA_Y 48
#define KERAYS_Y 192
#define POYTA_X0 48
#define POYTA_XL 112
#define POYTA_Y 224
#define TUPLAUS_Y 208
#define POYTA_X(i) ((POYTA_X0) + ((POYTA_XL) * ((i) - 1)))
#define TUPLAUS_KORTTI_I 3

#define ANIM_EQ(n) ((anim >= (n)) && (anim <= ((n) + tickskip)))
#define ANIM_LE(n) (anim <= ((n) + tickskip))
#define SPEED_MUL(x) ((x) * (tickskip + 1))
#define PYSAYTA_AANET() (musan_toisto = 0, pysayta_aanet())

// kortti 0 on pakka, 1-5 ovat pöydällä olevat 5 korttia
struct Pelikortti pelikortit[6] = { 
    { PAKKA_X, PAKKA_Y, 0, 1, 1, 0 }, /* selkäpuoli */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 1, tyhjä */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 2, tyhjä */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 3, tyhjä */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 4, tyhjä */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }  /* kortti 5, tyhjä */
};
// säilyttää edelliset kortit, täältä tarkistetaan esim. vanhat koordinaatit
struct Pelikortti pelikortit_v[6];
// kopiot korttikuvista, esittely käyttää 1-5 mutta itse peli vain 0
static unsigned char *kortti_caches[6] = { 0 };

/** "   X    X  X  XXXX   XXXXX   XXX    "
    "   X    X  X  X   X  X      X   X   "
    "    X  X   X  X   X  XXXXX  X   X   "
    "     X X   X  X   X  X      X   X   "
    "      X    X  XXXX   XXXXX   XXX    "
    "                                    "
    "                                    "
    "XXXX    XXX   X   X  XXXXX  XXXX   X"
    "X   X  X   X  X  X   X      X   X  X"
    "XXXX   X   X  XXX    XXXXX  XXXX   X"
    "X      X   X  X  X   X      X  X   X"
    "X       XXX   X   X  XXXXX  X   X  X"; */
static const unsigned char logokartta[] = {
    0x10, 0x93, 0xc7, 0xc7, 0x00,
    0x10, 0x92, 0x24, 0x08, 0x80,
    0x09, 0x12, 0x27, 0xc8, 0x80,
    0x05, 0x12, 0x24, 0x08, 0x80,
    0x02, 0x13, 0xc7, 0xc7, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0xf0, 0xe2, 0x27, 0xcf, 0x10,
    0x89, 0x12, 0x44, 0x08, 0x90,
    0xf1, 0x13, 0x87, 0xcf, 0x10,
    0x81, 0x12, 0x44, 0x09, 0x10,
    0x80, 0xe2, 0x27, 0xc8, 0x90,
};

#define PAIVITA_KORTTI(i) (pelikortit[i].muutettu = 1)

static void paivita_kaikki_kortit(void) {
    PAIVITA_KORTTI(0);
    PAIVITA_KORTTI(1);
    PAIVITA_KORTTI(2);
    PAIVITA_KORTTI(3);
    PAIVITA_KORTTI(4);
    PAIVITA_KORTTI(5);
}

// lineaari-interpolaatio
static int lerp(int y1, int y2, int x1, int x, int x2) {
    if (x > x2) x = x2;
    return (y1 * (x2 - x) + y2 * (x - x1)) / (x2 - x1);
}

static const char *tuplaus_onnittelut[] = {
    "USKOMATONTA",
    "MESTARITUPLAUS",
    "ONNEKAS ARVAUS",
    "KYLMÄT HERMOT",
    "LUOVUTAN!"
};
static const char *tuplaus_onnittelut_e[] = {
    "EXCELLENT",
    "NICELY DOUBLED",
    "WELL GUESSED",
    "NERVES OF STEEL",
    "I GIVE UP!"
};
#define ONNITTELU_N (sizeof(tuplaus_onnittelut) / sizeof(tuplaus_onnittelut[0]))

#if 0
const char *pelitila_nimi(enum Pelitila t) {
    switch (t) {
    case T_PANOS: return "T_PANOS";
    case T_VALITSE: return "T_VALITSE";
    case T_VOITTO: return "T_VOITTO";
    case T_TUPLAUS: return "T_TUPLAUS";
    case T_TUPLAOK: return "T_TUPLAOK";
    case T_TUPLAEI: return "T_TUPLAEI";
    case T_JAKO1: return "T_JAKO1";
    case T_JAKO2: return "T_JAKO2";
    case T_JAKO3: return "T_JAKO3";
    case T_UJAKO1: return "T_UJAKO1";
    case T_UJAKO2: return "T_UJAKO2";
    case T_UJAKO3: return "T_UJAKO3";
    case T_UJAKO4: return "T_UJAKO4";
    case T_TUPLA1: return "T_TUPLA1";
    case T_TUPLA2: return "T_TUPLA2";
    case T_TUPLA3: return "T_TUPLA3";
    case T_TUPLA4: return "T_TUPLA4";
    case T_VSIIRTO: return "T_VSIIRTO";
    case T_ESIT1: return "T_ESIT1";
    case T_ESIT2: return "T_ESIT2";
    default: abort(); return NULL;
    }
}
#endif

static void paivita_alue_kortti(short x1, short y1, short x2, short y2) {
    if (y2 > CARDAREA_HEIGHT) y2 = CARDAREA_HEIGHT;
    if (y1 > CARDAREA_HEIGHT) y1 = CARDAREA_HEIGHT;
    if (y1 < y2)
        paivita_alue(x1, y1, x2 - x1, y2 - y1);
}

static void paivita_kortit(void) {
    int i;
    struct Pelikortti *pk, *pv;
    char r = 0;
    short rx1, ry1, rx2, ry2;
    short tx1, ty1, tx2, ty2;

    // x- ja y-koordinaatit jotka päivittää vanhasta pelikorista verrattuna
    // esim. jos kortti liikkuu vasemmalle, niin suorakulmio on sen uuden
    // sijainnin oikealla puolella
#define AUTO_RECT() do {                                        \
                if (pk->x < pv->x)                              \
                    tx1 = pk->x, tx2 = pv->x + KORTTI_L;        \
                else                                            \
                    tx1 = pv->x, tx2 = pk->x + KORTTI_L;        \
                if (pk->y < pv->y)                              \
                    ty1 = pk->y, ty2 = pv->y + KORTTI_K;        \
                else                                            \
                    ty1 = pv->y, ty2 = pk->y + KORTTI_K;        \
    } while (0);
    // päivittää alueen tx1-tx2, ty1-ty2
    // yrittää yhdistää suorakulmioon r
#define COMMIT_RECT() do {                                      \
        if (!r)                                                 \
            ++r, rx1 = tx1, ry1 = ty1, rx2 = tx2, ry2 = ty2;    \
        else if (ry1 > ty2 || ty1 > ry2)                        \
            paivita_alue_kortti(tx1, ty1, tx2, ty2);            \
        else                                                    \
            rx1 = MIN(rx1, tx1), ry1 = MIN(ry1, ty1),           \
            rx2 = MAX(rx2, tx2), ry2 = MAX(ry2, ty2);           \
    } while (0);

    if (card0zoom && pelikortit[0].muutettu) {
        // tyhjän tilan piirto heti pakalle
        pk = &pelikortit[0];
        pv = &pelikortit_v[0];
        siirra_tyhja(pv->x, pv->y, pk->x, pk->y);
    }

    for (i = 5; i >= 0; --i) {
        pk = &pelikortit[i];
        pv = &pelikortit_v[i];
        if (pk->muutettu) {
            pk->muutettu = 0;
            if (!i && card0zoom) {
                // kortti 0 lentää; piirrä aina selkä
                if (pk->selka)
                    piirra_selka(pk->x, pk->y);
                else
                    piirra_kortti(pk->x, pk->y, pk->kortti);
                AUTO_RECT();
            } else if (pk->nakyva && pv->nakyva
                    && (pk->x != pv->x || pk->y != pv->y)) {
                if (pk->selka)
                    siirra_selka(pv->x, pv->y, pk->x, pk->y);
                else
                    siirra_kortti(pv->x, pv->y, pk->x, pk->y, pk->kortti);
                AUTO_RECT();
            } else if (pk->nakyva) {
                if (pk->selka)
                    piirra_selka(pk->x, pk->y);
                else
                    piirra_kortti(pk->x, pk->y, pk->kortti);
                tx1 = pk->x, ty1 = pk->y;
                tx2 = tx1 + KORTTI_L, ty2 = ty1 + KORTTI_K;
            } else if (pv->nakyva) {
                pyyhi_kortti(pv->x, pv->y);
                tx1 = pv->x, ty1 = pv->y;
                tx2 = tx1 + KORTTI_L, ty2 = ty1 + KORTTI_K;
            } else {
                *pv = *pk;
                continue;
            }
            COMMIT_RECT();
            *pv = *pk;
        }
    }

    if (r)
        paivita_alue_kortti(rx1, ry1, rx2, ry2);
}

void aanitesti(void) {
    int n, kappale;
    for (;;) {
        if (english)
            printf("Secret sound test. Enter 1 - %d or 0 to quit\n",
                    AANI_AANTEN_LOPPU - 1);
        else
            printf("Äänitesti. Syötä numero 1 - %d, tai 0 lopettaa\n",
                    AANI_AANTEN_LOPPU - 1);
        n = scanf("%u", &kappale);
        if (n <= 0)
            return;
        if (kappale <= 0)
            return;
        if (kappale < MUSA_MUSIEN_LOPPU)
            toista_musiikki_oletus(kappale);
        else
            toista_aani(kappale);
        puts(english ? "Playing... (press ESC to stop)"
                     : "Toistetaan (ESC lopettaa)...");
        anim = 0;
        while (!napit_kaikki.lopeta && toistaa_aanta()) {
            lue_napit();
            paivita_soitto();
            tahti();
        }
        PYSAYTA_AANET();
        lue_napit();
    }
}

void aloita_peli(int saldo) {
    alusta_sekoitin();
    jatka = 1;
    panos = 1;
    pelit = saldo;
    jokeri = 0;
    voitot = 0;
    voitto = 0;
    srand(time(NULL));
    demo_hand = rand() % kasi_lkm;
    alusta_tila(T_ESIT0);
    valot_efekti();
}

static int keraa_kortit(void) {
    if (tuplaus) {
        // tuplatessa vain tuplauskortti (3) kerätään
        if (!ANIM_LE(16)) {
            pelikortit[0].x = PAKKA_X;
            pelikortit[0].y = PAKKA_Y;
            pelikortit[TUPLAUS_KORTTI_I].nakyva = 0;
            PAIVITA_KORTTI(0);
            PAIVITA_KORTTI(TUPLAUS_KORTTI_I);
            return 1;
        }
        pelikortit[TUPLAUS_KORTTI_I].x =
                    lerp(POYTA_X(TUPLAUS_KORTTI_I), PAKKA_X, 0, anim, 16);
        pelikortit[TUPLAUS_KORTTI_I].y =
                    lerp(TUPLAUS_Y, PAKKA_Y, 0, anim, 16);
        pelikortit[TUPLAUS_KORTTI_I].selka = 1;
        pelikortit[TUPLAUS_KORTTI_I].nakyva = anim < 16;
        PAIVITA_KORTTI(TUPLAUS_KORTTI_I);
        PAIVITA_KORTTI(0);
        return 0;
    }

    if (ANIM_LE(15)) {
        // pakka siirtyy alaoikealle
        int x = pelikortit[0].x;
        // piirrä kortit 1-3 uudelleen jos pakka liikkuu niiden päältä
        if (POYTA_X(1) <= x && x < POYTA_X(1) + KORTTI_L)
            PAIVITA_KORTTI(1);
        if (POYTA_X(2) <= x && x < POYTA_X(2) + KORTTI_L)
            PAIVITA_KORTTI(2);
        if (POYTA_X(3) <= x && x < POYTA_X(3) + KORTTI_L)
            PAIVITA_KORTTI(3);
        pelikortit[0].x = lerp(PAKKA_X, POYTA_X(3), 0, anim, 15);
        pelikortit[0].y = lerp(PAKKA_Y, KERAYS_Y, 0, anim, 15);
        PAIVITA_KORTTI(0);
    }
    if (ANIM_EQ(16)) {
        // varmista että pakka on oikeassa paikassa
        pelikortit[0].x = POYTA_X(3);
        pelikortit[0].y = KERAYS_Y;
        PAIVITA_KORTTI(0);
    }
    if (10 <= anim && ANIM_LE(20)) {
        // keskimmäinen kortti (#3) liikkuu pakan kohdalle
        pelikortit[3].y = lerp(POYTA_Y, KERAYS_Y, 10, anim, 20);
        PAIVITA_KORTTI(3);
    }
    if (20 <= anim && ANIM_LE(30)) {
        // kortti #2 liikkuu pakan kohdalle
        if (!pelikortit[2].nakyva) anim += 10;
        pelikortit[2].x = lerp(POYTA_X(2), POYTA_X(3), 20, anim, 30);
        pelikortit[2].y = lerp(POYTA_Y, KERAYS_Y, 20, anim, 30);
        PAIVITA_KORTTI(2);
    }
    if (30 <= anim && ANIM_LE(40)) {
        // kortti #4 liikkuu pakan kohdalle
        if (!pelikortit[4].nakyva) anim += 10;
        pelikortit[4].x = lerp(POYTA_X(4), POYTA_X(3), 30, anim, 40);
        pelikortit[4].y = lerp(POYTA_Y, KERAYS_Y, 30, anim, 40);
        PAIVITA_KORTTI(4);
    }
    if (40 <= anim && ANIM_LE(50)) {
        // kortti #1 liikkuu pakan kohdalle
        if (!pelikortit[1].nakyva) anim += 10;
        pelikortit[1].x = lerp(POYTA_X(1), POYTA_X(3), 40, anim, 50);
        pelikortit[1].y = lerp(POYTA_Y, KERAYS_Y, 40, anim, 50);
        PAIVITA_KORTTI(1);
    }
    if (50 <= anim && ANIM_LE(60)) {
        // kortti #5 liikkuu pakan kohdalle
        if (!pelikortit[5].nakyva) anim += 10;
        pelikortit[5].x = lerp(POYTA_X(5), POYTA_X(3), 50, anim, 60);
        pelikortit[5].y = lerp(POYTA_Y, KERAYS_Y, 50, anim, 60);
        PAIVITA_KORTTI(5);
    }
    if (ANIM_EQ(60)) {
        // piilota kortit
        int i;
        for (i = 1; i <= 5; ++i)
            pelikortit[i].nakyva = 0;
        paivita_kaikki_kortit();
    }
    if (anim <= 60 && anim % 10 <= tickskip)
        toista_aani(AANI_KORTTI_LIUKUU);
    if (60 <= anim && ANIM_LE(75)) {
        // siirrä pakka takaisin ylävasemmalle
        pelikortit[0].x = lerp(POYTA_X(3), PAKKA_X, 60, anim, 75);
        pelikortit[0].y = lerp(KERAYS_Y, PAKKA_Y, 60, anim, 75);
        PAIVITA_KORTTI(0);
    }
    if (ANIM_EQ(75)) {
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        PAIVITA_KORTTI(0);
        return 1;
    }
    return 0;
}

static int shuffle_i = 0;
static signed char p0 = 0, p1 = 0, p2 = 0;
static signed char p0d = 0, p1d = 0, p2d = 0;
static int pakan_sekoitus(void) {
    char i, maxv = MIN(tickskip + 1, 4);
    signed char pmin, pmax;
    // sekoita kortteja
    for (i = 0; i < 2 * (tickskip + 1); ++i)
        if (shuffle_i) sekoita_pakka(shuffle_i--);

    // liikuta pakkakopioita
    pmin = pmax = p0;
    if (p1 < pmin) pmin = p1;
    if (p1 > pmax) pmax = p1;
    if (p2 < pmin) pmin = p2;
    if (p2 > pmax) pmax = p2;

    p0 += p0d * maxv;
    if (p0 >= 8) p0d = -1, p0 = 16 - p0;
    if (p0 < 0) p0 = 0;
    if (p0 == 0) p0d = 0;
    if (p0 < pmin) pmin = p0;
    if (p0 > pmax) pmax = p0;

    p1 += p1d * maxv;
    if (p1 >= 8) p1d = -1, p1 = 16 - p1;
    if (p1 < 0) p1 = 0;
    if (p1 == 0) p1d = 0;
    if (p1 < pmin) pmin = p1;
    if (p1 > pmax) pmax = p1;

    p2 += p2d * maxv;
    if (p2 >= 8) p2d = -1, p2 = 16 - p2;
    if (p2 < 0) p2 = 0;
    if (p2 == 0) p2d = 0;
    if (p2 < pmin) pmin = p2;
    if (p2 > pmax) pmax = p2;

    piirra_suorakulmio(PAKKA_X, PAKKA_Y, KORTTI_L + 16 * 8, KORTTI_K, 0);
    piirra_selka_peitetty(PAKKA_X + 8 * p0, PAKKA_X + 8 * p1, PAKKA_Y);
    piirra_selka_peitetty(PAKKA_X + 8 * p1, PAKKA_X + 8 * p2, PAKKA_Y);
    piirra_selka(PAKKA_X + 8 * p2, PAKKA_Y);
    paivita_alue(PAKKA_X + 8 * pmin, PAKKA_Y,
                KORTTI_L + 8 * (pmax - pmin + 1), KORTTI_K);

    // ala siirtämään kopioita #0, #1, #2 tietyissä kohtaa
    if (ANIM_EQ(1) || ANIM_EQ(16) || ANIM_EQ(31)) p0d = 1;
    if (ANIM_EQ(6) || ANIM_EQ(21) || ANIM_EQ(36)) p1d = 1;
    if (ANIM_EQ(11) || ANIM_EQ(26) || ANIM_EQ(41)) p2d = 1;
    
    if (anim >= 66 && p0 == 0 && p1 == 0 && p2 == 0) {
        // sekoita loppuun ja siirry seuraavaan tilaan
        while (shuffle_i) sekoita_pakka(shuffle_i--);
        paivita_kaikki_kortit();
        return 1;
    }
    return 0;
}

// alusta kortti i pakan (kortti #0) kohdalle
static kortti_t alusta_yksi_kortti(int i, int selka, kortti_t kortti) {
    RANGE_CHECK(i, 1, 6);
    pelikortit[i].x = pelikortit[0].x;
    pelikortit[i].y = pelikortit[0].y;
    pelikortit[i].nakyva = 1;
    pelikortit[i].selka = selka;
    pelikortit[i].kortti = kortti;
    poyta[i - 1] = kortti;
    PAIVITA_KORTTI(i);
    return kortti;
}

// jaa pakasta kortti i pakan (kortti #0) kohdalle
static kortti_t jaa_yksi_kortti(int i, int selka) {
    kortti_t kortti = jaa_kortti();
    return alusta_yksi_kortti(i, selka, kortti);
}

static short jaa_i = 0;
static short jaa_ctr = 0;
static void jaa_kortit(void) {
    short i, x, landed = 1;
    for (i = 1; i <= 5; ++i) {
        // pudota kortteja
        if (pelikortit[i].nakyva && pelikortit[i].y < POYTA_Y) {
            pelikortit[i].y = MIN(pelikortit[i].y + SPEED_MUL(16), POYTA_Y);
            PAIVITA_KORTTI(i);
            landed = 0;
        }
    }

    if (jaa_i >= 6) {
        if (pelikortit[0].x == PAKKA_X) {
            if (landed) {
                // kaikki kortit jaettu
                alusta_tila(T_VALITSE);
                card0zoom = 0;
            }
            return;
        }
        // siirrä pakka takaisin vasemmalle
        pelikortit[0].x = x = MAX(pelikortit[0].x - SPEED_MUL(40), PAKKA_X);
        PAIVITA_KORTTI(0);
        for (i = 1; i <= 5; ++i) {
            // paljasta kortteja sitä mukaa kun pakka liikkuu
            if (pelikortit[i].selka && x < POYTA_X(6 - i)) {
                pelikortit[i].selka = 0;
                PAIVITA_KORTTI(i);
                toista_aani(AANI_KORTTI_KAANTYY);
            }
        }
        return;
    }

    if (jaa_ctr) {
        // pitääkö jakaa seuraava kortti?
        jaa_ctr -= (tickskip + 1);
        if (jaa_ctr >= 6 - tickskip && jaa_ctr <= 6) {
            jaa_yksi_kortti(jaa_i, 1);
            toista_aani(AANI_KORTTI_JAETAAN);
        }
        if (jaa_ctr <= 0) {
            jaa_ctr = 0;
            ++jaa_i;
        }
    } else {
        // pakkaa oikealle
        int x2 = POYTA_X(jaa_i);
        pelikortit[0].x = x = MIN(pelikortit[0].x + SPEED_MUL(16), x2);
        if (x == x2)
            jaa_ctr = 10;
    }
    PAIVITA_KORTTI(0);
}

static void jaa_tuplaus_kortti(void) {
    if (!jaa_i) {
        ++jaa_i;
        alusta_yksi_kortti(TUPLAUS_KORTTI_I, 1, 0);
    }
    if (ANIM_LE(16)) {
        // siirrä tuplauskortti oikealle paikalle
        pelikortit[TUPLAUS_KORTTI_I].x = lerp(
                PAKKA_X, POYTA_X(TUPLAUS_KORTTI_I), 0, anim, 16);
        pelikortit[TUPLAUS_KORTTI_I].y = lerp(
                PAKKA_Y, TUPLAUS_Y, 0, anim, 16);
        PAIVITA_KORTTI(0);
        PAIVITA_KORTTI(TUPLAUS_KORTTI_I);
    } else
        alusta_tila(T_TUPLAUS);
}

static void update_voitto_hitaus(int offset, char slow) {
    offset *= tickskip + 1;
    if (offset <= 10)
        voitto_hitaus = 8;
    else if (offset <= 20)
        voitto_hitaus = 4;
    else
        voitto_hitaus = 2;
    if (slow) voitto_hitaus *= 2;
}

static char valinnat[5] = { 0 };
static char pudotettavat = 0;
static char* valintavalot[5] = { &valot.valitse_1, &valot.valitse_2,
        &valot.valitse_3, &valot.valitse_4, &valot.valitse_5 };
static char valintanakyy[5] = { 0, 0, 0, 0, 0 };
static int valintoja = 0;
static int valintoja_nakyvissa = 0;
static signed char valintaajastin[5] = { 0, 0, 0, 0, 0 };

static void arvioi_kasi(void) {
    fast_memset(valinnat, 0, sizeof(valinnat));
    pelikortit[0].x = PAKKA_X;
    pelikortit[0].y = PAKKA_Y;
    card0zoom = 0;
    PYSAYTA_AANET();
    PAIVITA_KORTTI(0);
    voitto_disp = 0;
    alapalkki_vapaaksi = 1;
    if (arvioi_voitto_kasi()) {
        voitto = panos * (jokeri ?
            kadet[voitto_kasi].kerroin_jokeri : kadet[voitto_kasi].kerroin);
        if (voitto > paavoitto) voitto = paavoitto;
        update_voitto_hitaus(voitto, 0);
        alusta_tila(T_VOITTO);
    } else {
        voitto = 0;
        alusta_tila(T_PANOS);
    }
}

static void pudota_kortit(void) {
    int i;

    if (!jaa_ctr) {
        while (!(pudotettavat & (1 << (jaa_i - 1))) && jaa_i <= 5) {
            ++jaa_i;
        }
        if (jaa_i > 5) {
            alusta_tila(T_UJAKO2);
            return;
        }
        jaa_ctr = 1;
    }

    i = jaa_i;
    pelikortit[i].y += SPEED_MUL(12);
    pelikortit[i].selka = 1;
    if (pelikortit[i].y >= ALAPALKKI_Y) {
        pelikortit[i].nakyva = 0;
        ++jaa_i;
        jaa_ctr = 0;
    }
    PAIVITA_KORTTI(i);
}

static int jaa_n = 1;
static void jaa_uudet_kortit(void) {
    int i, x, wait = 0;

    for (i = 1; i <= 5; ++i) {
        // tarkista onko jaettuja putoavia kortteja
        if (pelikortit[i].nakyva && pelikortit[i].selka
                && pelikortit[i].y < POYTA_Y) {
            // pudota korttia
            pelikortit[i].y = MIN(pelikortit[i].y + SPEED_MUL(16), POYTA_Y);
            if (pelikortit[i].y == POYTA_Y) {
                // paljasta
                pelikortit[i].selka = 0;
                toista_aani(AANI_UUSI_KORTTI_ASETTUU);
            } else
                wait = 1;
            PAIVITA_KORTTI(i);
        }
    }

    if (wait) {
        // älä tee mitään muuta jos kortti putoamassa
        PAIVITA_KORTTI(0);
        return;
    }

    // onko kortteja jaettu jo?
    if (jaa_ctr) {
        if (!pudotettavat) {
            // ei enää pudotettavia kortteja, ala pudottaa pakkaa
            // viimeiselle kohdalle
            pelikortit[0].y = MIN(pelikortit[0].y + SPEED_MUL(8), POYTA_Y);
            if (pelikortit[0].y == POYTA_Y) {
                short x = pelikortit[0].x, y = pelikortit[0].y;
                // siirry koputusvaiheeseen
                kortti_t k = jaa_yksi_kortti(jaa_i, 0);
                // tallenna kuva kortista
                pyyhi_kortti(x, y);
                piirra_kortti(x, y, k);
                sailyta_kuva(x, y, KORTTI_L, KORTTI_K, kortti_cache);
                alusta_tila(T_UJAKO3);
            }
            PAIVITA_KORTTI(0);
            return;
        }
        // onko aika jakaa seuraava kortti?
        jaa_ctr -= SPEED_MUL(1);
        if (jaa_ctr <= 0) {
            // jaa kortti pudotettavaksi
            jaa_ctr = 0;
            jaa_yksi_kortti(jaa_i, 1);
            toista_aani(AANI_KORTTI_JAETAAN);
            PAIVITA_KORTTI(jaa_i);
            jaa_n = 1;
            ++jaa_i;
        }
    } else {
        int x2;
        if (jaa_n) {
            // etsi seuraava kortti joka jakaa
            while (!(pudotettavat & 1)) {
                ++jaa_i;
                pudotettavat >>= 1;
            }
            pudotettavat >>= 1;
            jaa_n = 0;
        }
        x2 = POYTA_X(jaa_i);
        pelikortit[0].x = x = MIN(pelikortit[0].x + SPEED_MUL(16), x2);
        if (x == x2) {
            jaa_ctr = 5;
            if (!pudotettavat)
                toista_aani(AANI_KORTTI_JAETAAN);
        }
    }
    PAIVITA_KORTTI(0);
}

static void valitse_kortti(int i) {
    --i;
    if (valinnat[i] ^= 1) {
        VALO_PAALLE(*valintavalot[i]);
        if (!valintanakyy[i]) {
            valintanakyy[i] = 1;
            ++valintoja_nakyvissa;
            valintaajastin[i] = -32;
        }
        if (++valintoja)
            VALO_VILKKU(valot.jako, 1);
    } else {
        VALO_VILKKU(*valintavalot[i], 1);
        if (--valintoja == 0)
            VALO_POIS(valot.jako);
    }
    paivita_palkki();
    toista_aani(AANI_VALINTA);
}

static char valintanopeus = 1;
static void paivita_valinnat(void) {
    char i, wn;
    for (i = 0; i < 5; ++i) {
        if (valintanakyy[i] && (!valinnat[i] || valintaajastin[i] != 0)) {
            wn = valintaajastin[i] < 0;
            if (valintaajastin[i] >= 32) {
                if (valinnat[i])
                    valintaajastin[i] = -32;
                else {
                    valintanakyy[i] = 0;
                    --valintoja_nakyvissa;
                    if (!valintoja_nakyvissa && tila == T_VOITTO
                            && alapalkki_fade == 0)
                        alapalkki_fade = 1;
                }
            }
            valintaajastin[i] += SPEED_MUL(valintanopeus);
            if (wn && valinnat[i] && valintaajastin[i] > 0)
                valintaajastin[i] = 0;
            paivita_alapalkki_valinnat();
            if (valintaajastin[i] <= 24 && yks_kerrallaan)
                break;
        }
    }
    valintanopeus ^= 3;
}

void piirra_valinnat(char paivita) {
    short i, y = GAREA_HEIGHT - 32;
    for (i = 0; i < 5; ++i) {
        if (valintanakyy[i]) {
            piirra_kuva_vierita(POYTA_X(i + 1), y, 96, 32,
                    valittu_kuva, y, GAREA_HEIGHT, valintaajastin[i]);
            if (paivita)
                paivita_alue(POYTA_X(i + 1), y, 96, 32);
        }
    }
}

void piirra_alapalkki_vain_voitto(char paivita) {
    piirra_suorakulmio(264, ALAPALKKI_Y + 8, 112, 16, 1);
    piirra_levea_luku_oikea(344, ALAPALKKI_Y + 10, 15, voitto_disp, 0);
    if (paivita)
        paivita_alue(264, ALAPALKKI_Y + 8, 112, 16);
}

void piirra_alapalkki_voitto(char paivita) {
    const int y = ALAPALKKI_Y;
    switch (tila)
    {
    case T_VOITTO:
    case T_TUPLAOK:
        if (english)
            piirra_teksti_oikea(160, y + 10, 11, 0, "YOU WON", 0);
        else
            piirra_teksti(160, y + 10, 11, 0, "VOITIT", 0);
        piirra_teksti( 424, y + 10, 11, 0,
                    english ? "DOUBLE" : "TUPLAATKO", 0);
        piirra_kuva_maski(256, y + 8, 8, 16, G_tulosv, G_tulosv_M);
        piirra_kuva_maski(376, y + 8, 8, 16, G_tuloso, G_tuloso_M);
        break;
    case T_TUPLA1:
    case T_TUPLA2:
    case T_TUPLA3:
    case T_TUPLAEI:
    case T_TUPLAUS:
        piirra_teksti(424, y + 10, 11, 0,
                english ? "DOUBLE" : "TUPLAATKO", 0);
    case T_TUPLA4:
        piirra_teksti(160, y + 10, 11, 0, english ? "BET" : "PANOS", 0);
        piirra_kuva_maski(256, y + 8, 8, 16, G_tulosv, G_tulosv_M);
        piirra_kuva_maski(376, y + 8, 8, 16, G_tuloso, G_tuloso_M);
        break;
    }
    piirra_alapalkki_vain_voitto(0);
    if (paivita)
        paivita_alue(0, y, GAREA_WIDTH, 32);
}

static void lopeta(void)  {
    pelit += voitot + voitto;
    jatka = 0;
}

static long laske_kohti(long nyt, long tavoite) {
    long erotus = tavoite - nyt;
    if (erotus >= 500)
        return 100;
    else if (erotus >= 200)
        return 10;
    else if (erotus >= 50)
        return 5;
    else if (erotus > 0)
        return 1;
    else
        return 0;
}

static void draw_konkka_line(short y) {
    if (y < 0 || y >= GAREA_HEIGHT) return;
    if (y < 160 || y >= 192) {
        fast_memset(taso0 + STRIDE * y, 0, STRIDE);
        fast_memset(taso1 + STRIDE * y, 0, STRIDE);
        fast_memset(taso2 + STRIDE * y, 0, STRIDE);
        fast_memset(taso3 + STRIDE * y, 0, STRIDE);
    } else {
        fast_memset(taso0 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso0 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
        fast_memset(taso1 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso1 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
        fast_memset(taso2 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso2 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
        fast_memset(taso3 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso3 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
    }
    paivita_alue(0, y, GAREA_WIDTH, 1);
}

static void lopeta_esittely(void) {
    char i;
    PYSAYTA_AANET();
    valmistele_ruutu_peli();
    alusta_tila(T_PANOS);
    pelikortit[0].nakyva = 1;
    pelikortit[0].selka = 1;
    pelikortit[0].x = PAKKA_X;
    pelikortit[0].y = PAKKA_Y;
    for (i = 1; i <= 5; ++i) {
        pelikortit[i].nakyva = 0;
    }
    if (!napit.panos && !napit.jako)
        toista_aani(AANI_ALOITA);
    paivita_kaikki_kortit();
    skip_input_read = 1;
}

static void esittely_pudota_korttia(int i, short x, short oy, short ny) {
    short py, ty, kh;
    if (ny <= -KORTTI_K && oy <= -KORTTI_K)
        return;
    if (ny < 0)
        py = 0, ty = -ny;
    else
        py = ny, ty = 0;
    if (ny > GAREA_HEIGHT)
        ny = GAREA_HEIGHT;
    if (ny >= 0) {
        short fy = oy;
        if (oy < 0) fy = 0;
        piirra_suorakulmio(x, fy, KORTTI_L, ny - oy, 0);
    }
    piirra_kuva_rajaa_y(x, py, KORTTI_L, KORTTI_K - ty,
        KORTTI_K, kortti_caches[i] + ty * (KORTTI_L / PPB),
        GAREA_HEIGHT);
    py = MAX(oy, 0);
    kh = KORTTI_K - ty + (ny - oy);
    kh = MIN(kh, GAREA_HEIGHT - py);
    if (kh > 0)
        paivita_alue(x, py, KORTTI_L, kh);
}

static void esittely1(void) {
    char i, any_card_visible = 0, stop;
    if (napit_kaikki.mika_tahansa) {
        lopeta_esittely();
        return;
    }
    demo_y += SPEED_MUL(1);
    stop = demo_y >= 400 && demo_y < 400 + 300;
    if (stop) return;

    for (i = 1; i <= 5; ++i) {
        short x, oy, ny;
        if (!pelikortit[i].nakyva) continue;
        x = pelikortit[i].x, oy = pelikortit[i].y;
        pelikortit[i].y = ny = oy + SPEED_MUL(1);

        // siirrä korttia alaspäin
        esittely_pudota_korttia(i, x, oy, ny);
        if (ny == GAREA_HEIGHT)
            pelikortit[i].nakyva = 0;
        any_card_visible = 1;
    }
    if (!any_card_visible) {
        // arvo uudet kortit
        alusta_pakka();
        for (i = 52; i > 0; --i)
            sekoita_pakka(i);
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 1;
            pelikortit[i].kortti = jaa_kortti();
            pelikortit[i].y = -KORTTI_L - (4 + (rand() % 8)) * 16;
            piirra_kortti(0, 0, pelikortit[i].kortti);
            sailyta_kuva(0, 0, KORTTI_L, KORTTI_K, kortti_caches[i]);
        }
        pyyhi_kortti(0, 0);
        demo_y = 0;
        if (!inactive--)
            alusta_tila(T_ESIT2);
    }
}

static void esittely2(void) {
    char i, any_card_visible = 0, stop;
    if (napit_kaikki.mika_tahansa) {
        lopeta_esittely();
        return;
    }
    if (demo_y < PAKKA_Y) {
        short oy = demo_y, ny;
        demo_y += SPEED_MUL(1);
        if (demo_y >= PAKKA_Y) {
            demo_y = PAKKA_Y;
        }
        ny = demo_y;
        for (i = 1; i <= 5; ++i)
            if (pelikortit[i].nakyva)
                esittely_pudota_korttia(i, pelikortit[i].x, oy, ny);
        if (demo_y == PAKKA_Y) {
            inactive = 720;
            pelikortit[0].x = -KORTTI_L * 2;
            piirra_selka(0, CARDAREA_HEIGHT - KORTTI_K);
            sailyta_kuva(0, CARDAREA_HEIGHT - KORTTI_K,
                                KORTTI_L, KORTTI_K, kortti_cache);
            piirra_suorakulmio(0, CARDAREA_HEIGHT - KORTTI_K,
                                KORTTI_L, KORTTI_K, 0);
            demo_panos = 0;
        }
    }
    if (demo_y != PAKKA_Y) return;

    if (!inactive) {
        // kerää kortit
        short ox = pelikortit[0].x, x = ox, x1, x2, w;
        if (x >= GAREA_WIDTH * 2 || tickskip >= 4) {
            demo_hand = (demo_hand + 1) % kasi_lkm;
            alusta_tila(rand() & 1 ? T_ESIT0 : T_ESIT1);
            return;
        }
        // pakka liikkuu oikealle
        x += SPEED_MUL(8);
        pelikortit[0].x = x;
        if (x > GAREA_WIDTH) {
            return;
        }
        w = KORTTI_L;
        if (x < 0)
            x1 = 0, x2 = -x, w += x;
        else
            x1 = x, x2 = 0;
        if (x + w > GAREA_WIDTH)
            w = GAREA_WIDTH - x;
        if (ox >= 0) {
            piirra_suorakulmio(ox, PAKKA_Y, x - ox, KORTTI_K, 0);
        }
        if (w > 0) {
            piirra_kuva_rajaa(x1, PAKKA_Y, w, KORTTI_K,
                        x2, 0, KORTTI_L, KORTTI_K,
                        kortti_cache);
            if (ox >= 0)
                paivita_alue(ox, PAKKA_Y, w + (x - ox), KORTTI_K);
            else
                paivita_alue(0, PAKKA_Y, w, KORTTI_K);
        } else if (x >= GAREA_WIDTH) {
            paivita_alue(ox, PAKKA_Y, x - ox, KORTTI_K);
        }

    } else {
        if (inactive >= 660 && inactive <= 660 + tickskip) {
            // esittele panos
            piirra_teksti_keski(540, 260, 14, 1,
                        english ? "BET" : "PANOS", 1);
            piirra_teksti_oikea(560, 300, 14, 0, "@", 1);
        }
        if (demo_panos < maksimipanos && inactive <= 660 + tickskip
                && (inactive - 60) % 120 <= tickskip) {
            // esittele voitot
            short x = POYTA_X(1), y = PAKKA_Y + KORTTI_K + 64, kerroin;
            ++demo_panos;
            piirra_suorakulmio(500, 300, 40, 12, 0);
            piirra_luku_oikea(540, 300, 14, 1, demo_panos, 1);
            paivita_alue(x, y, 400, 12 * kasi_lkm);
            for (i = 0; i < kasi_lkm; ++i) {
                piirra_suorakulmio(x, y, 400, 12,
                    i == demo_hand ? 2 : 0);
                piirra_teksti(x, y, 14,
                    i == demo_hand, kadet[i].nimi, 0);
                piirra_teksti_oikea(x + 400, y, 14, 0, "@", 0);
                kerroin = (jokeri_saatavilla &&
                            demo_panos >= jokeri_minimipanos
                            ? kadet[i].kerroin_jokeri 
                            : kadet[i].kerroin);
                if (kerroin)
                    piirra_luku_oikea(x + 384, y, 14, 1, 
                        demo_panos * kerroin, 0);
                else
                    piirra_teksti_oikea(x + 384, y, 14, 1, "-", 0);
                y += 12;
            }
        }
        inactive -= SPEED_MUL(1);
        if (inactive < 0)
            inactive = 0;
        if (!inactive) {
            // pyyhi panokset
            piirra_suorakulmio(0, PAKKA_Y + KORTTI_K + 64,
                    GAREA_WIDTH, 128, 0);
            paivita_alue(0, PAKKA_Y + KORTTI_K + 64,
                    GAREA_WIDTH, 128);
        }
    }
}

static int u4x = 0, u4y = 0;
static long ufrac = 0;

static void piirra_kasi_voitot(int kasi, short y, int vari) {
    short kerroin = jokeri ? kadet[kasi].kerroin_jokeri : kadet[kasi].kerroin;
    piirra_teksti(240, y, vari, 0, kadet[kasi].nimi, 0);
    piirra_teksti(576, y, vari, 0, "@", 0);
    if (kerroin)
        piirra_luku_oikea(572, y, vari, 1, panos * kerroin, 0);
    else
        piirra_teksti_oikea(572, y, vari, 1, "-", 0);
}

static void piirra_kadet(char hohda_aina) {
    short i, hohda = hohda_aina || (anim & 32), y = 48;
    piirra_suorakulmio(240, 48, 352, kasi_lkm * 12, 0);
    if (!hohda && voitto_kasi >= 0)
        piirra_suorakulmio(240, y + 12 * voitto_kasi, 352, 12, 2);
    for (i = 0; i < kasi_lkm; ++i) {
        piirra_kasi_voitot(i, y, (hohda && voitto_kasi == i) ? 4 : 14);
        y += 12;
    }
    paivita_alue(240, 48, 352, kasi_lkm * 12);
}

static void piirra_kadet_uudelleen(void) {
    short i, hohda = anim & 32, y = 48, kerroin;
    if (voitto_kasi < 0) return;
    i = voitto_kasi;
    y += 12 * i;
    piirra_suorakulmio(240, y, 352, 12, !hohda ? 2 : 0);
    piirra_kasi_voitot(i, y, hohda ? 4 : 14);
    paivita_alue(240, y, 352, 12);
}

static void piilota_kadet(void) {
    piirra_suorakulmio(240, 48, 352, kasi_lkm * 12, 0);
    paivita_alue(240, 48, 352, kasi_lkm * 12);
}

static void vilkuta_voittokatta(void) {
    short i, j, y;
    int o, pstride = STRIDE - 352 / 8;
    unsigned char *p1, *p2, *p3;
    if (voitto_kasi < 0) return;
    y = 48 + voitto_kasi * 12;
    o = y * STRIDE + 240 / 8;
    p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;
    for (i = 0; i < 12; ++i) {
        for (j = 0; j < 352 / 8; ++j) {
            *p3++ = (*p3 ^ 0xFF) & *p2++;
            *p1++ = *p1 ^ 0xFF;
        }
        p1 += pstride, p2 += pstride, p3 += pstride;
    }
    paivita_alue(240, y, 352, 12);
}

void lisaa_panos(unsigned panos) {
    if (!lisarahat_ok || panos >= 10000) return;
    lisarahat += panos;
    pelit += panos;
    paivita_ylapalkki();
    if (!musan_toisto)
        toista_aani(AANI_ALOITA);
}

void alusta_tila(enum Pelitila t) {
    switch (t) {
    case T_RAHAA: {
        int i;
        valmistele_ruutu_peli();
        pelikortit[0].nakyva = 1;
        pelikortit[0].selka = 1;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 0;
        }
        paivita_kaikki_kortit();
        kaikki_valot_pois();
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        piilota_kadet();
        inactive = 0;
    } break;
    case T_PANOS: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.panos, 1);
        if (voitot > 0)
            VALO_PAALLE(valot.voitot);
        if (!pelit && !voitot && !voitto) {
            alusta_tila(T_KONKKA);
            return;
        } else if (panos > pelit + voitot) {
            panos = pelit + voitot;
            paivita_ylapalkki();
        } else if (pelit + voitot > 999999) {
            alusta_tila(T_HUOLTO);
            return;
        }
        VALO_VILKKU(valot.jako, 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        piirra_kadet(0);
        inactive = 0;
    } break;
    case T_VSIIRTO: {
        oli_paavoitto = voitto * 2 >= paavoitto;
        voitto_disp = voitto;
        palswap = 0;
        update_voitto_hitaus(voitto, oli_paavoitto);
        if (tila == T_VOITTO)
            piirra_kadet(1);
    } break;
    case T_JAKO1: {
        int i;
        // poista liila väri alapalkista
        if (alapalkki_fade)
            alapalkki_fade = 9;
        for (i = 1; i <= 5; ++i)
            pelikortit[i].selka = 1;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        PYSAYTA_AANET();
        paivita_kaikki_kortit();
        valintoja_nakyvissa = 0;
        card0zoom = 1;
    } break;
    case T_JAKO2: {
        jokeri = jokeri_saatavilla && panos >= jokeri_minimipanos;
        tuplaus_ctr = 0;
        tuplaus = 0;
        card0zoom = 1;
        // jokeri on aina pakassa viimeisenä (53.) korttina,
        // jos ei sekoiteta sinne asti niin se ei päädy peliin
        shuffle_i = jokeri ? 52 : 51;
        p0 = p1 = p2 = p0d = p1d = p2d = 0;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        toista_aani(AANI_SEKOITA);
    } break;
    case T_JAKO3: {
        pelikortit[0].x = PAKKA_X, pelikortit[0].y = PAKKA_Y;
        jaa_i = 1, jaa_ctr = 0;
    } break;
    case T_VALITSE: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.valitse_1, 1);
        VALO_VILKKU(valot.valitse_2, 1);
        VALO_VILKKU(valot.valitse_3, 1);
        VALO_VILKKU(valot.valitse_4, 1);
        VALO_VILKKU(valot.valitse_5, 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        fast_memset(valinnat, 0, sizeof(valinnat));
        valintoja = 0;
        yks_kerrallaan = 0;
        piirra_kadet(0);
    } break;
    case T_UJAKO0: {
        kaikki_valot_pois();
        paivita_palkki();
        tuplaus = 0;
        paivita_kaikki_kortit();
        yks_kerrallaan = 1;
    } break;
    case T_UJAKO1: {
        int i;
        paivita_palkki();
        pudotettavat = 0;
        jaa_i = 1, jaa_ctr = 0;
        for (i = 0; i < 5; ++i) {
            pudotettavat |= (~valinnat[i] & 1) << i;
            if (!valinnat[i])
                pelikortit[i + 1].selka = 1;
        }
        paivita_kaikki_kortit();
        if (!pudotettavat) {
            fast_memset(valinnat, 0, sizeof(valinnat));
            arvioi_kasi();
            return;
        } else
            toista_aani(AANI_KORTTIEN_PUDOTUS);
    } break;
    case T_UJAKO2: {
        tuplaus = 0;
        pelikortit[0].x = PAKKA_X, pelikortit[0].y = PAKKA_Y;
        jaa_i = 1, jaa_ctr = 0, jaa_n = 1;
    } break;
    case T_UJAKO3: {
        tuplaus = 0;
        card0zoom = 1;
        koputus_anim = 0;
        toista_musiikki_oletus(MUSA_KOPUTUS);
        fast_memset(valinnat, 0, sizeof(valinnat));
    } break;
    case T_UJAKO4: {
        u4x = pelikortit[0].x;
        u4y = pelikortit[0].y;
        ufrac = 0;
        PYSAYTA_AANET();
        toista_aani(AANI_KORTTI_LIUKUU);
    } break;
    case T_TUPLA1: {
        int i, j = 0;
        for (i = 1; i <= 5; ++i)
            pelikortit[i].selka = 1, j |= pelikortit[i].nakyva;
        voitto_kasi = -1;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        voitto_disp = voitto;
        piilota_kadet();
        paivita_kaikki_kortit();
        paivita_alapalkki();
        card0zoom = 1;
    } break;
    case T_TUPLA2: {
        voitto *= 2;
        update_voitto_hitaus(voitto, 0);
    } break;
    case T_TUPLA3: {
        card0zoom = 1;
        shuffle_i = jokeri ? 52 : 51;
        p0 = p1 = p2 = p0d = p1d = p2d = 0;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        pelikortit[1].nakyva = 0;
        pelikortit[2].nakyva = 0;
        pelikortit[4].nakyva = 0;
        pelikortit[5].nakyva = 0;
        toista_aani(AANI_SEKOITA);
    } break;
    case T_TUPLA4: {
        card0zoom = 1;
        tuplaus = 1;
        pelikortit[0].x = PAKKA_X, pelikortit[0].y = PAKKA_Y;
        jaa_i = 0;
        tuplaus_banner_x = 50;
        kaikki_valot_pois();
        paivita_alapalkki();
        VALO_VILKKU(valot.tuplaus_pieni, 1);
        VALO_VILKKU(valot.tuplaus_suuri, 2);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        /* toista_musiikki(MUSA_TUPLAUS, 120 + tuplaus_ctr * 24); */
        {
            int vj = voitto, vk = 0;
            while (vj * 2 <= paavoitto && vk < 5)
                vj *= 2, ++vk;
            toista_musiikki(MUSA_TUPLAUS, 240 - vk * 24);
        }
        musan_toisto = 1;
    } break;
    case T_TUPLAUS:
        card0zoom = 0;
        break;
    case T_TUPLAOK: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.voitot, 1);
        if (voitto * 2 <= paavoitto)
            VALO_VILKKU(valot.tuplaus, 1);
        else
            piirra_teksti_keski(320, 144, 15, 0,
                    (english ? tuplaus_onnittelut_e : tuplaus_onnittelut)
                    [rand() % ONNITTELU_N], 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        paivita_alapalkki();
        ++tuplaus_ctr;
        /* toista_musiikki(MUSA_VOITTO, 96 + tuplaus_ctr * 24); */
        toista_musiikki(MUSA_KOPUTUS, 144 + tuplaus_ctr * 20);
        musan_toisto = 0;
    } break;
    case T_TUPLAEI: {
        toista_musiikki_oletus(AANI_TUPLAUS_HAVITTY);
        musan_toisto = 0;
        voitto = voitto_disp = 0;
        paivita_alapalkki();
        kaikki_valot_pois();
        paivita_palkki();
    } break;
    case T_VOITTO: {
        kaikki_valot_pois();
        anim = 0;
        piirra_kadet(0);
        paivita_palkki();
        fast_memset(valinnat, 0, sizeof(valinnat));
        musan_toisto = 0;
        voitto_vilkku = 0;
        if (tila != T_VOITTO)
            alapalkki_fade = !valintoja_nakyvissa;
    } break;
    case T_ESIT0: {
        char i, y, x;
        const unsigned char *p = logokartta;
        valmistele_ruutu_esittely();
        kortti_caches[1] = kortti_cache;
        kortti_caches[2] = kortti_cache1;
        kortti_caches[3] = kortti_cache2;
        kortti_caches[4] = kortti_cache3;
        kortti_caches[5] = kortti_cache4;
        alapalkki_fade = 0;
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 0;
            pelikortit[i].selka = 0;
            pelikortit[i].x = POYTA_X(i);
        }
        for (i = 0; i < 25; ++i) {
            piirra_selka((rand() % (GAREA_WIDTH - KORTTI_L - 64) + 32) & ~7,
                        (rand() % (GAREA_HEIGHT - KORTTI_K - 64) + 32) & ~7);
        }
        piirra_teksti(GAREA_WIDTH + 16, 16,
            15, 1, "VIDEOPOKERI-DOS/386", 0);
        piirra_teksti_oikea(GAREA_WIDTH - 16, GAREA_HEIGHT - 32,
            15, 1, "ORIGINAL 1986 RAY", 0);
        piirra_teksti_oikea(GAREA_WIDTH - 16, GAREA_HEIGHT - 16,
            15, 1, "DOS 2021-2025 RISTIJÄTKÄ", 0);
        for (y = 0; y < 12; ++y) {
            for (x = 0; x < 36;) {
                unsigned char c = *p++;
                for (i = 0; i < 8; ++i) {
                    if (c & 0x80)
                        piirra_suorakulmio(32 + x * 16, 80 + y * 16, 16, 16, 10);
                    c <<= 1;
                    ++x;
                }
            }
        }
        paivita_alue(0, 0, GAREA_WIDTH, GAREA_HEIGHT);
        inactive = 600;
    } break;
    case T_ESIT1: {
        char i;
        valmistele_ruutu_esittely();
        kaikki_valot_pois();
        piirra_palkki_napit();
        demo_y = 0;
        inactive = 1 + (rand() % 4);
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 0;
            pelikortit[i].selka = 0;
            pelikortit[i].x = POYTA_X(i);
        }
    } break;
    case T_ESIT2: {
        const kortti_t *esittelykortit = &kadet[demo_hand].esittely->kortit;
        char i;
        kortti_t k;
        valmistele_ruutu_esittely();
        demo_y = -KORTTI_K - 32;
        inactive = 1;
        for (i = 1; i <= 5; ++i) {
            k = esittelykortit[i - 1];
            if (k == K_SELKA) {
                piirra_selka(0, 0);
            } else {
                piirra_kortti(0, 0, k);
            }
            sailyta_kuva(0, 0, KORTTI_L, KORTTI_K, kortti_caches[i]);
        }
        piirra_suorakulmio(0, 0, KORTTI_L, KORTTI_K, 0);
    } break;
    case T_KONKKA: {
        assert(!voitot && !voitto);
        jaa_i = 0;
        toista_aani(AANI_PANOS1 + (panos % 5));
        kaikki_valot_pois();
        piirra_palkki_napit();
        paivita_alue(0, GAREA_HEIGHT,
                     PLANE_WIDTH, PLANE_HEIGHT - GAREA_HEIGHT);
        vaihda_keltaiseen_palettiin();
        if (!lisarahat_ok) {
            alapalkki_fade = 0;
            piirra_alapalkki();
            piirra_teksti_keski(320, ALAPALKKI_Y + 10, 1, 0,
                            english ? "GAME OVER" : "PEEAA", 0);
        }
    } break;
    case T_HUOLTO: {
        kaikki_valot_pois();
        paivita_valot();
        alapalkki_fade = 0;
        piirra_alapalkki();
        piirra_teksti_keski(320, ALAPALKKI_Y + 10, 1, 0,
                          english ? "GET THE DEALER" : "HAE PELINHOITAJA", 0);
        vaihda_punaiseen_palettiin();
        toista_musiikki_oletus(MUSA_TUPLAUS);
        musan_toisto = 1;
        lisarahat_ok = 0;
    } break;
    }
    tila = t;
    anim = 0;
    new_mode = 1;
}

static void aja_peli_internal(void) {
    switch (tila) {
    case T_RAHAA:
        if (anim >= 8)
            alusta_tila(T_PANOS);
        break;
    case T_PANOS: {
        if (napit_kaikki.mika_tahansa)
            inactive = 0;
        else
            inactive += SPEED_MUL(1);
        if (napit.panos) {
            panos = (panos % maksimipanos) + 1;
            if (panos > pelit + voitot)
                panos = 1;
            toista_aani(AANI_PANOS1 + ((panos - 1) % 5));
            paivita_ylapalkki();
            jokeri = jokeri_saatavilla && panos >= jokeri_minimipanos;
            piirra_kadet(0);
            VALO_VILKKU(valot.jako, 1);
        }
        if (pelit + voitot >= panos && napit.jako) {
            kaikki_valot_pois();
            VALO_PAALLE(valot.jako);
            paivita_palkki();
            piilota_kadet();
            if (pelit <= panos) {
                voitot -= panos - pelit;
                pelit = 0;
            } else
                pelit -= panos;
            paivita_ylapalkki();
            paivita_alapalkki();
            alusta_pakka();
            alusta_tila(pelikortit[TUPLAUS_KORTTI_I].nakyva ? T_JAKO1 : T_JAKO2);
        }
        if (voitot && napit.voitot)
            lopeta();
        if (inactive >= 1800) {
            kaikki_valot_pois();
            valot_efekti();
            piirra_palkki_napit();
            alusta_tila(T_ESIT0);
        }
        break;
    }
    case T_JAKO1: /* kerää kortit */
        if (keraa_kortit())
            alusta_tila(T_JAKO2);
        break;
    case T_JAKO2: /* sekoita */
        if (pakan_sekoitus())
            alusta_tila(T_JAKO3);
        break;
    case T_JAKO3: /* jaa kortit */
        jaa_kortit();
        break;
    case T_TUPLA1: /* kerää kortit */
        if (keraa_kortit())
            alusta_tila(T_TUPLA2);
        break;
    case T_TUPLA2: /* panos tuplataan */
        if (voitto_disp < voitto && (anim % voitto_hitaus) <= tickskip) {
            voitto_disp += laske_kohti(voitto_disp, voitto);
            toista_aani(AANI_VOITONMAKSU);
            paivita_alapalkki_voitto();
        } else if (voitto_disp == voitto)
            alusta_tila(T_TUPLA3);
        break;
    case T_TUPLA3: /* sekoita */
        if (pakan_sekoitus())
            alusta_tila(T_TUPLA4);
        break;
    case T_TUPLA4: /* jaa kortti */
        jaa_tuplaus_kortti();
        paivita_tuplaus_alapalkki();
        break;
    case T_TUPLAUS: { /* arvaa */
        if (napit.tuplaus_pieni || napit.tuplaus_suuri) {
            int oikein = 0;
            kortti_t tuplaus_kortti = jaa_kortti();
            pelikortit[TUPLAUS_KORTTI_I].kortti = tuplaus_kortti;
            if (napit.tuplaus_pieni)
                oikein = KARVO(tuplaus_kortti) < 6;
            if (napit.tuplaus_suuri)
                oikein = KARVO(tuplaus_kortti) > 6;
            oikein |= tuplaus_kortti == K_JOKERI;
            pelikortit[TUPLAUS_KORTTI_I].selka = 0;
            PAIVITA_KORTTI(TUPLAUS_KORTTI_I);
            alusta_tila(oikein ? T_TUPLAOK : T_TUPLAEI);
        }
        paivita_tuplaus_alapalkki();
        break;
    }
    case T_TUPLAEI:
        if (anim >= 10) {
            if (alapalkki_fade == 8)
                alapalkki_fade = 9;
            else if (!alapalkki_fade)
                alusta_tila((pelit | voitot) ? T_PANOS : T_KONKKA);
        }
        break;
    case T_VALITSE: /* valitse kortit */
        if (napit.valitse_1) valitse_kortti(1);
        if (napit.valitse_2) valitse_kortti(2);
        if (napit.valitse_3) valitse_kortti(3);
        if (napit.valitse_4) valitse_kortti(4);
        if (napit.valitse_5) valitse_kortti(5);
        if (napit.jako && valintoja) {
            paivita_palkki();
            piilota_kadet();
            paivita_alapalkki();
            alusta_tila(T_UJAKO0);
        }
        break;
    case T_UJAKO0: /* odota ennen korttien pudotusta */
        if (anim >= 10) alusta_tila(T_UJAKO1);
        break;
    case T_UJAKO1: /* pudota kortit */
        pudota_kortit();
        paivita_alapalkki();
        break;
    case T_UJAKO2: /* jaa uudet */
        jaa_uudet_kortit();
        break;
    case T_UJAKO3: { /* tinnitys jäävistyy */
        int i, a, y = pelikortit[0].y;
        // laske pakan uusi paikka
        for (i = 0; i <= tickskip; ++i) {
            a = koputus_anim >> 1;
            ufrac += a * a + (koputus_anim >= 120 ?
                                (koputus_anim - 120) << 7 : 1);
            pelikortit[0].y += ufrac >> 12;
            ufrac &= 0xFFF;
            ++koputus_anim;
        }
        if (y != pelikortit[0].y) {
            // päivitä näyttöä
            short x = pelikortit[0].x;
            short y1 = pelikortit[0].y;
            piirra_suorakulmio2(x, y, KORTTI_L, y1 - y, 0);
            if (y < POYTA_Y + KORTTI_K) {
                // piirrä alla olevaa korttia vähitellen
                const struct Pelikortti *pk = &pelikortit[jaa_i];
                short y2 = y1 + 8;
                short yo = y - pk->y;
                piirra_kuva_rajaa_y(pk->x, y, KORTTI_L, KORTTI_K - yo,
                            KORTTI_K, kortti_cache + yo * KORTTI_L / PPB, y2);
            }
            piirra_selka(x, y1);
            paivita_alue(x, y, KORTTI_L, y1 - y + KORTTI_K);
            if (y1 >= GAREA_HEIGHT) {
                // pakka on pudonnut ruudulta täysin
                PAIVITA_KORTTI(jaa_i);
                pelikortit_v[0] = pelikortit[0];
                if (tickskip > 2) /* älä vedä UJAKO4:sta hitailla koneilla */
                    arvioi_kasi();
                else
                    alusta_tila(T_UJAKO4);
            }
        }
        break;
    }
    case T_UJAKO4: { /* pakka lentää */
        int x = pelikortit[0].x, y = pelikortit[0].y;
        if (ANIM_LE(8)) {
            pelikortit[0].nakyva = 1;
            pelikortit[0].x = lerp(u4x, PAKKA_X, 0, anim, 8);
            pelikortit[0].y = lerp(u4y, PAKKA_Y, 0, anim, 8);
        }
        PAIVITA_KORTTI(0);
        if (x < POYTA_X(1) + KORTTI_L)
            PAIVITA_KORTTI(1);
        if (x >= POYTA_X(2) - KORTTI_L && x < POYTA_X(2) + KORTTI_L)
            PAIVITA_KORTTI(2);
        if (x >= POYTA_X(3) - KORTTI_L && x < POYTA_X(3) + KORTTI_L)
            PAIVITA_KORTTI(3);
        if (x >= POYTA_X(4) - KORTTI_L && x < POYTA_X(4) + KORTTI_L)
            PAIVITA_KORTTI(4);
        if (x >= POYTA_X(5) - KORTTI_L && x < POYTA_X(5) + KORTTI_L)
            PAIVITA_KORTTI(5);
        if (anim >= 9 + tickskip)
            arvioi_kasi();
        break;
    }
    case T_VOITTO: {
        if (!voitto_vilkku) {
            VALO_VILKKU(valot.voitot, 1);
            if (voitto * 2 <= paavoitto)
                VALO_VILKKU(valot.tuplaus, 1);
            VALO_VILKKU_ALUSTA();
            voitto_vilkku = 1;
            anim = 0;
            paivita_palkki();
        } else if ((anim & 31) <= tickskip) {
            vilkuta_voittokatta();
        }
        if (napit.voitot || voitto * 2 > paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_VSIIRTO);
        }
        if (napit.tuplaus && voitto * 2 <= paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_TUPLA1);
        } else if (voitto_disp < voitto && (anim % voitto_hitaus) <= tickskip) {
            voitto_disp += laske_kohti(voitto_disp, voitto);
            toista_aani(AANI_VOITONMAKSU);
            paivita_alapalkki_voitto();
        } else if (voitto_disp == voitto && !musan_toisto) {
            toista_musiikki(MUSA_VOITTO, 100 + tuplaus_ctr * 20);
            musan_toisto = 1;
        }
        break;
    }
    case T_TUPLAOK: {
        unsigned dur = 180 - tuplaus_ctr * 6;
        if (napit.tuplaus && voitto * 2 <= paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_TUPLA1);
            break;
        }
        if (napit.voitot || voitto * 2 > paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_VSIIRTO);
            break;
        }
        if (ANIM_EQ(dur)) {
            toista_musiikki(MUSA_VOITTO, 100 + tuplaus_ctr * 20);
            musan_toisto = 1;
        }
        break;
    }
    case T_VSIIRTO: {
        if (oli_paavoitto && anim % 8 <= tickskip) {
            switch (palswap) {
            case 0: vaihda_liilaan_palettiin(); break;
            case 1: vaihda_vihreaan_palettiin(); break;
            }
            palswap = (palswap + 1) & 1;
        }
        if (anim % voitto_hitaus > tickskip) return;
        if (!voitto) {
            if (alapalkki_fade)
                alapalkki_fade = 9;
            if (oli_paavoitto) {
                piirra_suorakulmio(160, 144, 480, 16, 0);
                paivita_alue(160, 144, 480, 16);
                palswap = 0;
                vaihda_liilaan_palettiin();
                paivita_kaikki_kortit();
                toista_musiikki_oletus(MUSA_PAAVOITTO);
            }
            paivita_ylapalkki();
            paivita_alapalkki();
            voitto_kasi = -1;
            alusta_tila(oli_paavoitto ? T_VLOPPU : T_PANOS);
            oli_paavoitto = 0;
        } else {
            short kasvatus = laske_kohti(voitot, voitot + voitto);
            voitot += kasvatus;
            voitto -= kasvatus;
            voitto_disp = voitto;
            toista_aani(AANI_VOITONMAKSU);
            paivita_ylapalkki_voitot();
            paivita_alapalkki_voitto();
        }
        break;
    }
    case T_VLOPPU: {
        if (anim >= 8) {
            piirra_kadet(0);
            vaihda_normaaliin_palettiin();
            alusta_tila(T_PANOS);
        }
        break;
    }
    case T_KONKKA:
        if (anim >= 300 && !lisarahat_ok)
            lopeta();
        else if (pelit > 0) {
            vaihda_normaaliin_palettiin();
            alusta_tila(T_RAHAA);
        }
        break;
    case T_ESIT0: {
        if (napit_kaikki.mika_tahansa) {
            lopeta_esittely();
            return;
        }
        if (inactive <= 0)
            alusta_tila(T_ESIT1);
        else
            inactive -= SPEED_MUL(1);
        break;
    }
    case T_ESIT1:
        esittely1();
        break;
    case T_ESIT2:
        esittely2();
        break;
    case T_HUOLTO:
        break;
    }
}

void aja_peli(void) {
    ++tick;
    if (tickskip && tickskip_i++ < tickskip)
        return;
    tickskip_i = 0;
    if (alapalkki_vapaaksi) {
        if (valintoja_nakyvissa) {
            paivita_valinnat();
            return;
        }
        alapalkki_vapaaksi = 0;
    }
    if (alapalkki_fade & 7) {
        if (!(tick & 1)) {
            short i;
            for (i = 0; i <= tickskip && (alapalkki_fade & 7); ++i)
                alapalkki_fade = (alapalkki_fade + 1) & 15;
            paivita_alapalkki();
        }
        return;
    }

    if (skip_input_read)
        skip_input_read = 0;
    else
        lue_napit();
    if (napit_kaikki.lopeta) {
        lopeta();
        return;
    }
    if (napit_kaikki.lisaa_panos_1)
        lisaa_panos(1);
    if (napit_kaikki.lisaa_panos_5)
        lisaa_panos(5);
    aja_peli_internal();
    paivita_valinnat();
    paivita_kortit();
    if (!new_mode)
        anim += SPEED_MUL(1);
    new_mode = 0;
}
