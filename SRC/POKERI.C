
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "POKERI.H"
#include "PELI.H"
#include "KENO.H"
#include "RUUTU.H"
#include "SOITTO.H"
#include "POYTA.H"
#include "AIKA.H"
#include "VALOT.H"

static const char *VERSION =
    "VIDEOPOKERI-DOS 386 RISTIJÄTKÄ 2024 v2.2 2025-10-13";

int english = 0;
int keno = 1;

static short frameskip_i = 0;
static int soundtest = 0;
static int configured_skip = -1;
static int alkurahat = 0;
static int sbauto = 0;
void waste_cycles(void);

#pragma aux waste_cycles = \
    "push ebp" \
    "mov esi,0000046ch" \
    "mov edi,[esi]" \
    "mov ebp,000af000h" \
    "mov ebx,50" \
    "loop2:" \
    "mov ecx,8192" \
    "loop1:" \
    "inc eax" \
    "xor eax,edx" \
    "xor edx,eax" \
    "xor eax,edx" \
    "xor edx,eax" \
    "mov [ebp],eax" \
    "loop loop1" \
    "mov eax,[esi]" \
    "sub eax,edi" \
    "cmp eax,40" \
    "jg end2" \
    "dec ebx" \
    "jnz loop2" \
    "end2:" \
    "pop ebp" \
    modify exact [eax ebx ecx edx esi edi]

static int cpu_slowness(void) {
    unsigned int u = *(unsigned int*)0x46c, u2;
    waste_cycles();
    u2 = *(unsigned int*)0x46c;
    return u2 - u;
}

static unsigned configure_skip_auto(unsigned slowness) {
    if (slowness >= 40)
        return 3;
    else
        return slowness / 12;
}

void debug_print_(const char *f, int line, const char *s, ...) {
    va_list args;
    va_start(args, s);
    printf("%s:%d: ", f, line);
    vprintf(s, args);
    va_end(args);
    puts("");
}

void debug_abort_(const char *f, int line, const char *s, ...) {
    union REGS r;
    va_list args;

    r.x.eax = 0x0003;
    int386(0x10, &r, &r);

    va_start(args, s);
    printf("%s:%d: ", f, line);
    vprintf(s, args);
    va_end(args);
    puts("");
    abort();
}

static void asetus_strtoul(const char *arvo, int *ulos) {
    char *endptr;
    *ulos = strtoul(arvo, &endptr, 0);
    if (arvo == endptr) *ulos = -1;
}

struct AsetuksetKertoimet {
    /* järjestys: ks. poyta.h KASI_ARVIO_* */
    int kertoimet[MAX_KASI_LKM];
};

struct AsetuksetKenoKertoimet {
    int kertoimet[25];
};

struct Asetukset {
    const char *nimi;
    int paavoitto;
    int maksimipanos;
    int jokeri_minimipanos;
    struct AsetuksetKertoimet kertoimet;
    struct AsetuksetKertoimet kertoimet_jokeri;    
};

struct AsetuksetKeno {
    const char *nimi;
    int paavoitto;
    int maksimipanos;
    int valintoja;
    int jokeri_minimipanos;
    struct AsetuksetKenoKertoimet kertoimet;
};

static char malli[16] = "";

static void try_sbauto(void) {
    const char *s = getenv("BLASTER");
    if (!s) return;

    for (;;) {
        unsigned n, v;
        char c;
        if (sscanf(s, " %[A-Z]%x%n", &c, &v, &n) < 2)
            break;

        switch (c) {
        case 'A':
            soittolaite_sbportti = v;
            break;
        case 'I':
            if (!(v & ~15))
                soittolaite_sbirq = v;
            break;
        case 'D':
            if (!(v & ~7))
                soittolaite_sbdma = v;
            break;
        }
        s += n;
    }
}

static int lue_asetus(const char *nimi, const char *arvo) {
    if (!strcmp(nimi, "english")) {
        asetus_strtoul(arvo, &english);
    } else if (!strcmp(nimi, "audio")) {
        if (!strcmp(arvo, "pc")) 
            soittolaite = SOITTOLAITE_PC;
        else if (!strcmp(arvo, "tandy")) 
            soittolaite = SOITTOLAITE_TANDY;
        else if (!strcmp(arvo, "adlib") || !strcmp(arvo, "fm")) 
            soittolaite = SOITTOLAITE_ADLIB;
        else if (!strcmp(arvo, "cms") || !strcmp(arvo, "gb")) 
            soittolaite = SOITTOLAITE_CMS;
        else if (!strcmp(arvo, "sb")) 
            soittolaite = SOITTOLAITE_SB;
        else if (!strcmp(arvo, "innova")) 
            soittolaite = SOITTOLAITE_INNOVA;
        else if (!strcmp(arvo, "mpu") || !strcmp(arvo, "midi")) 
            soittolaite = SOITTOLAITE_MPU;
        else if (!strcmp(arvo, "mt32")) 
            soittolaite = SOITTOLAITE_MT32;
#if 0
        else if (!strcmp(arvo, "csm1")) 
            soittolaite = SOITTOLAITE_CSM1;
#endif
        else if (!*arvo || !strcmp(arvo, "-") || !strcmp(arvo, "none"))
            soittolaite = SOITTOLAITE_NONE;
        else {
            if (english) {
                printf("Config error: unrecognized audio device: '%s'\n", arvo);
                puts("Allowed:      - pc tandy adlib sb cms innova mpu mt32");
            } else {
                printf("Määrittelyvirhe: tuntematon äänilaite: '%s'\n", arvo);
                puts("Sallitut:     - pc tandy adlib sb cms innova mpu mt32");
            }
            return 1;
        }
    } else if (!strcmp(nimi, "tandyport")) {
        asetus_strtoul(arvo, &soittolaite_tandyportti);
    } else if (!strcmp(nimi, "fmport")) {
        asetus_strtoul(arvo, &soittolaite_fmportti);
    } else if (!strcmp(nimi, "cmsport")) {
        asetus_strtoul(arvo, &soittolaite_cmsportti);
    } else if (!strcmp(nimi, "sbauto")) {
        asetus_strtoul(arvo, &sbauto);
    } else if (!strcmp(nimi, "sbport")) {
        asetus_strtoul(arvo, &soittolaite_sbportti);
    } else if (!strcmp(nimi, "sbirq")) {
        asetus_strtoul(arvo, &soittolaite_sbirq);
        if (soittolaite_sbirq < 0 || soittolaite_sbirq > 15) {
            if (english) {
                printf("Config error: invalid IRQ setting: '%s'\n", arvo);
            } else {
                printf("Määrittelyvirhe: virheellinen IRQ-arvo: '%s'\n", arvo);
            }
            return 1;
        }
    } else if (!strcmp(nimi, "sbdma")) {
        asetus_strtoul(arvo, &soittolaite_sbdma);
        if (soittolaite_sbdma < 1 || soittolaite_sbdma > 3) {
            if (english) {
                printf("Config error: invalid DMA setting: '%s'\n", arvo);
            } else {
                printf("Määrittelyvirhe: virheellinen DMA-arvo: '%s'\n", arvo);
            }
            return 1;
        }
    } else if (!strcmp(nimi, "innovaport")) {
        asetus_strtoul(arvo, &soittolaite_sidportti);
    } else if (!strcmp(nimi, "mpuport")) {
        asetus_strtoul(arvo, &soittolaite_mpuportti);
    } else if (!strcmp(nimi, "csm1port")) {
        asetus_strtoul(arvo, &soittolaite_csm1portti);
    } else if (!strcmp(nimi, "soundtest")) {
        asetus_strtoul(arvo, &soundtest);
    } else if (!strcmp(nimi, "malli")) {
        strncpy(malli, arvo, sizeof(malli) - 1);
    } else if (!strcmp(nimi, "rahat")) {
        asetus_strtoul(arvo, &alkurahat);
    } else if (!strcmp(nimi, "lisapanokset")) {
        asetus_strtoul(arvo, &lisarahat_ok);
    } else if (!strcmp(nimi, "skip")) {
        if (!*arvo || !strcmp(arvo, "auto")) {
            configured_skip = -1;
        } else {
            asetus_strtoul(arvo, &configured_skip);
            if (configured_skip < 0 || configured_skip > 7) {
                if (english) {
                    printf("Config error: invalid skip setting: '%d'\n",
                                configured_skip);
                } else {
                    printf("Määrittelyvirhe: virheellinen skip-arvo: '%d'\n",
                                configured_skip);
                }
                return 1;
            }
        }
    }
    return 0;
}

static int lue_asetusrivi(char *line) {
    char *eq, *tmp;
    eq = strchr(line, '=');
    if (eq) {
        *eq++ = '\0';
        
        tmp = strchr(eq, '\n');
        if (tmp) *tmp = '\0';

        tmp = eq + strlen(eq);
        while (isspace(*--tmp)) *tmp = '\0';

        if (lue_asetus(line, eq)) {
            return 1;
        }
    }
    return 0;
}

static int lue_asetukset(char *overrides[], int overrides_n) {
    FILE *f = fopen("POKERI.CFG", "r");
    int i, fail = 0;
    if (f) {
        char *eq, c, *tmp, line[81];
        while (fgets(line, sizeof(line), f)) {
            if (line[0] == '#') continue;
            if (lue_asetusrivi(line)) {
                fail = 1;
                break;
            }
        }
        fclose(f);
    }
    for (i = 0; i < overrides_n; ++i) {
        char *line = overrides[i];
        if (lue_asetusrivi(line)) {
            fail = 1;
            break;
        }
    }
    if (soittolaite == SOITTOLAITE_SB && sbauto) try_sbauto();
    return fail;
}

static struct Asetukset malli_asetukset[] = {
    { "100a",      100,  5,  3, 
        { {   0,   0,  20, 15, 10, 5, 4, 2, 2, 0 } },
        { {   0,  20,  20, 15, 10, 5, 4, 2, 2, 0 } }},
    { "100b",      100,  5,  3, 
        { {   0,   0,  20, 15, 10, 5, 4, 3, 3, 0 } },
        { {   0,  20,  20, 15, 10, 5, 4, 2, 2, 0 } }},
    { "200",       200,  5,  3, 
        { {   0,   0,  30, 15, 10, 5, 4, 3, 3, 0 } },
        { {   0,  40,  30, 15, 10, 5, 4, 2, 2, 0 } }},
    { "1000b",    1000,  5, -1, 
        { {   0,   0, 200, 20, 10, 5, 4, 3, 3, 0 } },
        { {   0,   0, 200, 20, 10, 5, 4, 3, 3, 0 } }},
    { "1000j",    1000, 20,  8, 
        { {   0,   0,  30, 15, 10, 5, 4, 3, 3, 0 } },
        { {   0,  40,  30, 15, 10, 5, 4, 2, 2, 0 } }},
};

static struct AsetuksetKeno malli_keno_asetukset[] = {
    { "100k",      100,  5, 5,  3,
        { {  4,  0, 0, 0, 0,  5,  2, 0, 0, 0, 12,  4, 1, 0, 0,
            15,  5, 2, 1, 0, 20, 15, 4, 3, 0 } }},
    { "200k",      200,  5, 5,  1,
        { {  4,  0, 0, 0, 0,  5,  2, 0, 0, 0, 18,  8, 0, 0, 0,
            30, 12, 4, 0, 0, 40, 18, 9, 2, 0 } }},
};

static int alusta_asetukset(void) {
    int i, malleja = sizeof(malli_asetukset) / sizeof(malli_asetukset[0]);
    for (i = 0; i < malleja; ++i) {
        if (!*malli || !strcmp(malli, malli_asetukset[i].nimi)) {
            const struct Asetukset *asetus = &malli_asetukset[i];
            keno = 0;
            paavoitto = asetus->paavoitto;
            maksimipanos = asetus->maksimipanos;
            jokeri_minimipanos = asetus->jokeri_minimipanos;
            if (jokeri_minimipanos < 0 || jokeri_minimipanos > maksimipanos) {
                jokeri_minimipanos = maksimipanos + 1;
                jokeri_saatavilla = 0;
            } else
                jokeri_saatavilla = 1;
            if (alusta_kadet(english, asetus->kertoimet.kertoimet,
                         asetus->kertoimet_jokeri.kertoimet)) {
                puts(english ? "Invalid multipliers"
                             : "Kertoimet virheellisiä");
                return 1;
            }
            if (maksimipanos <= 1 || maksimipanos >= 100) {
                puts(english ? "Invalid maximum bet"
                             : "Virheellinen maksimipanos");
                return 1;
            }
            if (paavoitto <= maksimipanos || paavoitto >= 10000) {
                puts(english ? "Invalid jackpot"
                             : "Virheellinen päävoitto");
                return 1;
            }
            return 0;
        }
    }
    malleja = sizeof(malli_keno_asetukset) / sizeof(malli_keno_asetukset[0]);
    for (i = 0; i < malleja; ++i) {
        if (!*malli || !strcmp(malli, malli_keno_asetukset[i].nimi)) {
            int j, k;
            const struct AsetuksetKeno *asetus = &malli_keno_asetukset[i];
            keno = 1;
            paavoitto = asetus->paavoitto;
            maksimipanos = asetus->maksimipanos;
            jokeri_minimipanos = asetus->jokeri_minimipanos;
            if (jokeri_minimipanos < 0 || jokeri_minimipanos > maksimipanos) {
                jokeri_minimipanos = maksimipanos + 1;
                jokeri_saatavilla = 0;
            } else
                jokeri_saatavilla = 1;
            keno_valintoja_max = asetus->valintoja;
            keno_kertoimet_kaikki = asetus->kertoimet.kertoimet;
            if (keno_valintoja_max < 1) {
                puts(english ? "Invalid maximum selections"
                             : "Virheellinen maksimivalintamäärä");
                return 1;
            }
            if (maksimipanos <= 1 || maksimipanos >= 100) {
                puts(english ? "Invalid maximum bet"
                             : "Virheellinen maksimipanos");
                return 1;
            }
            if (paavoitto <= maksimipanos || paavoitto >= 10000) {
                puts(english ? "Invalid jackpot"
                             : "Virheellinen päävoitto");
                return 1;
            }
            for (j = 0; j < 5; ++j) {
                int z = 0;
                for (k = 0; k < 5; ++k) {
                    i = j * 5 + k;
                    if (keno_kertoimet_kaikki[i] < 0
                            || keno_kertoimet_kaikki[i] > 100
                            || (keno_kertoimet_kaikki[i] && z)) {
                        puts(english ? "Invalid multipliers"
                                    : "Virheelliset kertoimet");
                        return 1;
                    }
                    z = !keno_kertoimet_kaikki[i];
                }
            }
            return 0;
        }
    }
    return 1;
}

static void aja_pokeria(int saldo) {
    char vuoro = 0;
    alusta_valot();
    valmistele_ruutu();
    aloita_peli(saldo);
    while (jatka) {
        vuoro = frameskip_i >= frameskip;
        aja_peli();
        paivita_valot();
        if (vuoro) ruudun_paivitys();
        paivita_soitto();
        tahti();
        if (vuoro) ruudun_piirto();
        frameskip_i = vuoro ? 0 : frameskip_i + 1;
#if SLOMO
        microsleep(100000);
#endif
    }
}

static void aja_kenoa(int saldo) {
    char vuoro = 0;
    alusta_valot();
    valmistele_ruutu();
    keno_aloita_peli(saldo);
    while (jatka) {
        vuoro = frameskip_i >= frameskip;
        keno_aja_peli();
        paivita_valot();
        if (vuoro) ruudun_paivitys();
        paivita_soitto();
        tahti();
        if (vuoro) ruudun_piirto();
        frameskip_i = vuoro ? 0 : frameskip_i + 1;
#if SLOMO
        microsleep(100000);
#endif
    }
}

static void print_fix(int f) {
    char buf[8], *be = &buf[8];
    int i = 8;
    *--be = '\0';
    *--be = '0' + (f % 10);
    *--be = '.';
    f /= 10;
    do {
        *--be = '0' + (f % 10);
        f /= 10;
    } while (f);
    printf("%s", be);
}

static unsigned long estimate_free_space(unsigned long s) {
    unsigned long a = 0, m;
    void *p;
    while (a < s) {
        m = (a + s) >> 1;
        if (!m) return 0;
        p = malloc(m);
        if (p) {
            a = m + 1;
            free(p);
        } else {
            s = m - 1;
        }
    }
    return a;
}

static void intro(void) {
    puts("\n"
         "===========================================\n"
         " VIDEOPOKERI-DOS -by- RISTIJÄTKÄ 2024-2025 \n"
         "===========================================\n");
}

static void clearstdin(void) {
    static int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

const char *NIMIMERKIT =
    "KJL  JOLA  OH  KK  MZ  ES\n"
    "RM  PJP  KP  SL  TT  LM\n"
    "EL  JT  ML  TP";

int main(int argc, char *argv[]) {
    int n = 0, tila, saldo, aani_ok;
    unsigned long sz;

    intro();
    if (lue_asetukset(argv + 1, argc - 1))
        return EXIT_FAILURE;

    if (alusta_asetukset()) {
        int malleja = sizeof(malli_asetukset) / sizeof(malli_asetukset[0]);
        if (english) {
            printf("Config error: unrecognized model: '%s'\n", malli);
            printf("Allowed:     ");
        } else {
            printf("Määrittelyvirhe: tuntematon malli: '%s'\n", malli);
            printf("Sallitut:    ");
        }
        for (n = 0; n < malleja; ++n)
            printf(" %s", malli_asetukset[n].nimi);
        malleja = sizeof(malli_keno_asetukset) / sizeof(malli_keno_asetukset[0]);
        for (n = 0; n < malleja; ++n)
            printf(" %s", malli_keno_asetukset[n].nimi);
        putchar('\n');
        return 1;
    }

    n = alusta_ruutu(&sz);
    if (n) {
        unsigned long estimate;
        if (english) {
            printf("Not enough free memory on the computer.\n");
            printf("There must be at least %lu KB of free memory.\n",
                        (sz + 999) / 1000);
            estimate = estimate_free_space(sz);
            if (estimate)
                printf("There is an estimated %lu KB of free memory.\n",
                        (estimate + 999) / 1000);
            else
                puts("The application was not able to allocate any memory.");
        } else {
            printf("Tietokoneessa ei ole tarpeeksi vapaata muistia.\n");
            printf("Muistia pitää olla vapaata vähintään %lu Kt.\n",
                        (sz + 999) / 1000);
            estimate = estimate_free_space(sz);
            if (estimate)
                printf("Muistia on vapaana arviolta %lu Kt.\n",
                        (estimate + 999) / 1000);
            else
                puts("Ohjelma ei voinut varata muistia ollenkaan.");
        }
        return EXIT_FAILURE;
    }

    tila = ruudun_vaihto(0x12, 0);
    if (tila < 0) {
        if (english) {
            printf("Could not switch to a VGA screen mode, make sure\n"
                "that a VGA compatible graphics card is installed.\n");
        } else {
            printf("Ei voitu vaihtaa VGA-tilaan. Varmista, että\n"
                "tietokoneessa on VGA-yhteensopiva näyttökortti.\n");
        }
        return EXIT_FAILURE;
    }
    if (configured_skip < 0) {
        int slow;
        puts("");
        if (english) {
            puts("  Measuring CPU speed, please wait...");
        } else {
            puts("  Mitataan nopeutta...");
        }
        slow = cpu_slowness();
        configured_skip = configure_skip_auto(slow);
    }
    ruudun_palautus(tila);

    intro();
    frameskip = tickskip = configured_skip;
    if (english) {
        puts("Exit the game with Esc or Backspace\n");
        printf("Game speed: ");
        print_fix(600 / (tickskip + 1));
        printf(" frames per second\n");
        printf("Main buffer allocated memory: %lu bytes\n", sz);
        printf("Audio device: %s\n", soittolaitteen_nimi(soittolaite));
    } else {
        puts("Muista, pelistä pääsee pois Escillä tai askelpalauttimella!\n");
        printf("Pelin nopeus: ");
        print_fix(600 / (tickskip + 1));
        printf(" ruutua sekunnissa\n");
        printf("Pääpuskurin muistinvarauksen koko %lu tavua\n", sz);
        printf("Äänilaite: %s\n", soittolaitteen_nimi(soittolaite));
    }
    aani_ok = !alusta_soitto();
    if (!aani_ok) {
        if (english) {
            puts("Failed to initialize audio, game audio disabled");
        } else {
            puts("Äänen käyttöönotto epäonnistui, peli hiljenee");
        }
    }
    if (soundtest) {
        if (!aani_ok || !soittolaite) {
            if (english) {
                puts("No sound available");
            } else {
                puts("Ei ääntä");
            }
            return EXIT_FAILURE;
        }
        aanitesti();
        lopeta_soitto();
        return EXIT_SUCCESS;
    }
    puts("\n\n");
    
    if (alkurahat > 0 && alkurahat <= 10000) {
        n = 1;
        saldo = alkurahat;
        /* odota vähän aikaa että näkyy debuggitiedot */
        microsleep(1200000);
    } else {
        do {
            char of[11];
            if (english) {
                puts("How many marks will you gamble today? ");
            } else {
                puts("Montako markkaa tänään uhkapelaat? ");
            }
            n = scanf("%10s", of);
            n &= sscanf(of, "%9u", &saldo);
            clearstdin();
            if (n && (saldo > 10000 || strlen(of) > 5)) {
                if (english) {
                    puts("Come on. Nobody has that much money.");
                    puts("If you did, you wouldn't be playing this.");
                } else {
                    puts("Älä puhu paskaa");
                }
                saldo = -1;
            }
        } while (!n || saldo < 0);
    }

    if (n == EOF)
        return EXIT_SUCCESS;

    if (saldo > 0) {
        tila = ruudun_vaihto(0x12, 1);
        if (tila >= 0) {
            if (keno)
                aja_kenoa(saldo);
            else
                aja_pokeria(saldo);
            saldo += lisarahat;
            lopeta_soitto();
            ruudun_palautus(tila);
            intro();
            if (english) {
                printf("You bet a total of %d mk.\n", saldo);
                if (pelit)      printf("You now have %ld mk.\n", pelit);
                else            puts("You are now broke.");
                if (pelit > saldo)
                    printf("You won %ld mk!\n", pelit - saldo);
                else if (pelit < saldo)
                    printf("You lost %ld mk...\n", saldo - pelit);
                else
                    puts("You are now back where you started.");
            } else {
                printf("Panostit yhteensä %d mk.\n", saldo);
                if (pelit)      printf("Lopputuloksesi %ld mk.\n", pelit);
                else            puts("Menetit kaikki rahasi.");
                if (pelit > saldo)
                    printf("Teit %ld mk voittoa!\n", pelit - saldo);
                else if (pelit < saldo)
                    printf("Teit %ld mk tappiota...\n", saldo - pelit);
                else
                    puts("Erotus on pyöreä nolla.");
            }
        } else
            puts(english ? "Something went wrong" : "Jokin meni pieleen");
    } else
        puts(english ? "So be it." : "Hyvä on.");
    
    puts(english ? "Back to DOS...\n" : "Palataan DOSiin.\n");
    return EXIT_SUCCESS;
}
