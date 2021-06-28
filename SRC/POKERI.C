
#include <dos.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "POKERI.H"
#include "PELI.H"
#include "RUUTU.H"
#include "SOITTO.H"
#include "POYTA.H"
#include "AIKA.H"
#include "VALOT.H"

static const char *VERSION =
    "VIDEOPOKERI-DOS 386 RISTIJÄTKÄ 2021 v1.4 2021-06-28";

static short frameskip_i = 0;
static int soundtest = 0;
static int configured_skip = -1;
int english = 0;
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

int cpu_slowness(void) {
    unsigned int u = *(unsigned int*)0x46c, u2;
    waste_cycles();
    u2 = *(unsigned int*)0x46c;
    return u2 - u;
}

unsigned configure_skip_auto(unsigned slowness) {
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

void asetus_strtoul(const char *arvo, int *ulos) {
    char *endptr;
    *ulos = strtoul(arvo, &endptr, 0);
    if (arvo == endptr) *ulos = -1;
}

int kertoimet_100a_N[] = { 0, 20, 15, 10, 5, 4, 2, 2 };
int kertoimet_100a_J[] = { 20, 20, 15, 10, 5, 4, 2, 2 };

int kertoimet_100b_N[] = { 0, 20, 15, 10, 5, 4, 3, 3 };
int kertoimet_100b_J[] = { 20, 20, 15, 10, 5, 4, 2, 2 };

int kertoimet_200_N[] = { 0, 30, 15, 10, 5, 4, 3, 3 };
int kertoimet_200_J[] = { 40, 30, 15, 10, 5, 4, 2, 2 };

int kertoimet_1000b_N[] = { 0, 200, 20, 10, 5, 4, 3, 3 };
int kertoimet_1000b_J[] = { 0, 200, 20, 10, 5, 4, 3, 3 };

int kertoimet_1000j_N[] = { 0, 30, 15, 10, 5, 4, 3, 3 };
int kertoimet_1000j_J[] = { 40, 30, 15, 10, 5, 4, 2, 2 };

#define KERTOIMET(n, j) do {                    \
        int i;                                  \
        for (i = 0; i < KASI_LKM; ++i)          \
            kadet[i].kerroin = n[i],            \
            kadet[i].kerroin_jokeri = j[i];     \
    } while (0)

void alusta_kieli(void) {
    short i = 0;
    if (english) {
        kadet[i++].nimi = "FIVE OF A KIND";
        kadet[i++].nimi = "STRAIGHT FLUSH";
        kadet[i++].nimi = "FOUR OF A KIND";
        kadet[i++].nimi = "FULL HOUSE";
        kadet[i++].nimi = "FLUSH";
        kadet[i++].nimi = "STRAIGHT";
        kadet[i++].nimi = "THREE OF A KIND";
        kadet[i++].nimi = "TWO PAIRS";
    }
}

int lue_asetus(const char *nimi, const char *arvo) {
    if (!strcmp(nimi, "english")) {
        asetus_strtoul(arvo, &english);
    } else if (!strcmp(nimi, "audio")) {
        if (!strcmp(arvo, "pc"))
            soittolaite = 1;
        else if (!strcmp(arvo, "tandy"))
            soittolaite = 2;
        else if (!strcmp(arvo, "adlib") || !strcmp(arvo, "fm"))
            soittolaite = 3;
        else if (!strcmp(arvo, "cms") || !strcmp(arvo, "gb"))
            soittolaite = 4;
        else if (!strcmp(arvo, "sb"))
            soittolaite = 5;
        else if (!strcmp(arvo, "innova"))
            soittolaite = 6;
        else if (!strcmp(arvo, "mpu") || !strcmp(arvo, "midi"))
            soittolaite = 7;
        else if (!strcmp(arvo, "mt32"))
            soittolaite = 8;
        else if (!*arvo || !strcmp(arvo, "-") || !strcmp(arvo, "none"))
            soittolaite = 0;
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
    } else if (!strcmp(nimi, "soundtest")) {
        asetus_strtoul(arvo, &soundtest);
    } else if (!strcmp(nimi, "malli")) {
        if (!*arvo || !strcmp(arvo, "100a")) {
            maksimipanos = 5;
            paavoitto = 100;
            jokeri_saatavilla = 1;
            jokeri_minimipanos = 3;
            KERTOIMET(kertoimet_100a_N, kertoimet_100a_J);
        } else if (!strcmp(arvo, "100b")) {
            maksimipanos = 5;
            paavoitto = 100;
            jokeri_saatavilla = 1;
            jokeri_minimipanos = 3;
            KERTOIMET(kertoimet_100b_N, kertoimet_100b_J);
        } else if (!strcmp(arvo, "200")) {
            maksimipanos = 5;
            paavoitto = 200;
            jokeri_saatavilla = 1;
            jokeri_minimipanos = 3;
            KERTOIMET(kertoimet_200_N, kertoimet_200_J);
        } else if (!strcmp(arvo, "1000b")) {
            maksimipanos = 5;
            paavoitto = 1000;
            jokeri_saatavilla = 0;
            jokeri_minimipanos = 100;
            KERTOIMET(kertoimet_1000b_N, kertoimet_1000b_J);
        } else if (!strcmp(arvo, "1000j")) {
            maksimipanos = 20;
            paavoitto = 1000;
            jokeri_saatavilla = 1;
            jokeri_minimipanos = 8;
            KERTOIMET(kertoimet_1000j_N, kertoimet_1000j_J);
        } else {
            if (english) {
                printf("Config error: unrecognized model: '%s'\n", arvo);
                puts("Allowed:      100a 100b 200a 1000b 1000j");
            } else {
                printf("Määrittelyvirhe: tuntematon malli: '%s'\n", arvo);
                puts("Sallitut:     100a 100b 200a 1000b 1000j");
            }
            return 1;
        }
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

int lue_asetusrivi(char *line) {
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

int lue_asetukset(char *overrides[], int overrides_n) {
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
    return fail;
}

void pokeri(int saldo) {
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

void print_fix(int f) {
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

void intro(void) {
    puts("\n"
         "======================================\n"
         " VIDEOPOKERI-DOS -by- RISTIJÄTKÄ 2021 \n"
         "======================================\n");
}

void clearstdin(void) {
    static int c;
    do {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int main(int argc, char *argv[]) {
    int n = 0, tila, saldo, aani_ok;
    unsigned long sz;

    intro();
    if (lue_asetukset(argv + 1, argc - 1))
        return EXIT_FAILURE;

    n = alusta_ruutu(&sz);
    if (n) {
        if (english) {
            printf("Not enough free memory on the computer.\n");
            printf("There must be at least %d KB of free memory.\n",
                        (sz + 999) / 1000);
        } else {
            printf("Tietokoneessa ei ole tarpeeksi vapaata muistia.\n");
            printf("Muistia pitää olla vapaata vähintään %d kt.\n",
                        (sz + 999) / 1000);
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
        printf("Sound device: %s\n", soittolaitteen_nimi(soittolaite));
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
            puts("Failed to enable audio, the game will be silent");
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
#if !NDEBUG
    n = 1, saldo = 50;
#else
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
        if (n && (saldo > 1000000 || strlen(of) > 9)) {
            if (english) {
                puts("Come on. Nobody has that much money.");
                puts("If you did, you wouldn't be playing this.");
            } else {
                puts("Älä puhu paskaa");
            }
            saldo = -1;
        }
    } while (!n || saldo < 0);
#endif

    if (n == EOF)
        return EXIT_SUCCESS;

    alusta_kieli();
    if (saldo > 0) {
        tila = ruudun_vaihto(0x12, 1);
        if (tila >= 0) {
            pokeri(saldo);
            lopeta_soitto();
            ruudun_palautus(tila);
            intro();
            if (english) {
                printf("You started with %u mk.\n", saldo);
                if (pelit)      printf("You now have %u mk.\n", pelit);
                else            puts("You are now broke.");
                if (pelit > saldo)
                    printf("You won %u mk!\n", pelit - saldo);
                else if (pelit < saldo)
                    printf("You lost %u mk...\n", saldo - pelit);
                else
                    puts("You are now back where you started.");
            } else {
                printf("Alkurahasi oli %u mk.\n", saldo);
                if (pelit)      printf("Lopputuloksesi %u mk.\n", pelit);
                else            puts("Menetit kaikki rahasi.");
                if (pelit > saldo)
                    printf("Teit %u mk voittoa!\n", pelit - saldo);
                else if (pelit < saldo)
                    printf("Teit %u mk tappiota...\n", saldo - pelit);
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
