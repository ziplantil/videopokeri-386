
#ifndef _SOITTO_H
#define _SOITTO_H

#include "POKERI.H"

#define COMMAND_NOOP 0
/* laiteajurin on käsiteltävä nämä */
#define COMMAND_NOTE_ON 1
#define COMMAND_NOTE_OFF 2
#define COMMAND_INSTRUMENT 3

/* nämä käsittelee äänimoottori, laiteajurin ei tarvitse */
#define COMMAND_RESTART 0x80
#define COMMAND_END 0xFF

/* käännetäänkö Sound Blasterin PCM-tuki (mukaan lukien PCM-äänet)? */
#define SB_PCM 1

#define SOITTOLAITE_NONE 0      /* ei mitään */
#define SOITTOLAITE_PC 1        /* sisäinen kaiutin */
#define SOITTOLAITE_TANDY 2     /* Tandy */
#define SOITTOLAITE_ADLIB 3     /* AdLib */
#define SOITTOLAITE_CMS 4       /* Game Blaster (CMS) */
#define SOITTOLAITE_SB 5        /* Sound Blaster */
#define SOITTOLAITE_INNOVA 6    /* Innovation SSI-2001 */
#define SOITTOLAITE_MPU 7       /* MPU-401, General MIDI */
#define SOITTOLAITE_MT32 8      /* MT-32 */
#if 0
#define SOITTOLAITE_CSM1 9      /* Covox Sound Master I */
#endif
    
extern int soittolaite;
/* Tandy/PCjr: I/O-portti, oletus 0xc0 */
extern int soittolaite_tandyportti;
/* AdLib: FM-portti, oletus 0x388 */
extern int soittolaite_fmportti;
/* CMS: portti, oletus 0x220 */
extern int soittolaite_cmsportti;
/* Sound Blaster: PCM-portti, oletus 0x220 */
extern int soittolaite_sbportti;
/* Sound Blaster: IRQ (oletuksena 5) */
extern int soittolaite_sbirq;
/* Sound Blaster: DMA (oletuksena 1) */
extern int soittolaite_sbdma;
/* Innovation SSI 2001: SID-portti, oletus 0x280 */
extern int soittolaite_sidportti;
/* MPU-401-portti, oletus 0x330 */
extern int soittolaite_mpuportti;
/* Covox Sound Master I -portti, oletus 0x220 */
extern int soittolaite_csm1portti;

extern void (*_soitto_paivita)(void);
extern void (*_soitto_komento)(short *, short, unsigned char, unsigned char);
extern void (*_soitto_toista)(int);
extern void (*_soitto_pysayta)(void);
extern void (*_soitto_lopeta)(void);

int alusta_soitto(void);
const char *soittolaitteen_nimi(int soittolaite);
void paivita_soitto(void);
void toista_musiikki(int aani, int tempo);
void toista_musiikki_oletus(int aani);
void toista_aani(int aani);
unsigned char toistaa_aanta(void);
void pysayta_aanet(void);
void lopeta_soitto(void);

#define MUSA_VOITTO 1
#define MUSA_TUPLAUS 2
#define MUSA_PAAVOITTO 3
#define MUSA_KOPUTUS 4
#define MUSA_KONKKA 5
#define MUSA_MUSIEN_LOPPU 6

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
#define AANI_KENO_HAVITTY 21
#define AANI_AANTEN_LOPPU 22

#endif
