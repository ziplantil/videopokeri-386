
#include <stdlib.h>
#include "SOITTO.H"
#include "AANET.H"

#pragma data_seg ( "aanet" );

#define KANAVAT_LOPPU NULL

#define AANIKOMENTO(k, x) (((k) << 8) | (x))

// yksikkö tempolla 120 = 1/960 sek. maksimiviive: 32767 (120 bpm: yli 34 sek)
//    ts. kokonuotti (yksi tahti) = 480
#define VIIVE(n) n
// älä tee mitään. jos tarvitsee pitkiä viiveitä, tämä on kätevä
#define ODOTA(v) VIIVE(v), AANIKOMENTO(COMMAND_NOOP, 0)
// n = 1-127, C#-0...G-10, C-4 = 48
#define PAALLE(v, n) VIIVE(v), AANIKOMENTO(COMMAND_NOTE_ON, (n & 0x7F))
// nuotti pois
#define POIS(v) VIIVE(v), AANIKOMENTO(COMMAND_NOTE_OFF, 0)
// soittimen vaihto. n = 0-127
#define SOITIN(v, n) VIIVE(v), AANIKOMENTO(COMMAND_INSTRUMENT, (n & 0x7F))
// kaikki kanavat takaisin alkuun. pistä ekalle kanavalle
#define ALKUUN(v) VIIVE(v), AANIKOMENTO(COMMAND_RESTART, 0)
// lopeta tämä kanava
#define LOPPU(v) VIIVE(v), AANIKOMENTO(COMMAND_END, 0)

#define NC_(o) ( 0 + (o) * 12)
#define NCs(o) ( 1 + (o) * 12)
#define ND_(o) ( 2 + (o) * 12)
#define NDs(o) ( 3 + (o) * 12)
#define NE_(o) ( 4 + (o) * 12)
#define NF_(o) ( 5 + (o) * 12)
#define NFs(o) ( 6 + (o) * 12)
#define NG_(o) ( 7 + (o) * 12)
#define NGs(o) ( 8 + (o) * 12)
#define NA_(o) ( 9 + (o) * 12)
#define NAs(o) (10 + (o) * 12)
#define NB_(o) (11 + (o) * 12)

#define AANI_KANAVIA_1(nimi) const unsigned short * nimi [] = { \
        nimi##0, KANAVAT_LOPPU\
    };
#define AANI_KANAVIA_2(nimi) const unsigned short * nimi [] = { \
        nimi##0, nimi##1, KANAVAT_LOPPU\
    };
#define AANI_KANAVIA_3(nimi) const unsigned short * nimi [] = { \
        nimi##0, nimi##1, nimi##2, KANAVAT_LOPPU\
    };
#define AANI_KANAVIA_4(nimi) const unsigned short * nimi [] = { \
        nimi##0, nimi##1, nimi##2, nimi##3, KANAVAT_LOPPU\
    };

/****
    soittimet
        0:      kantti (voitto)
        1:      basso (voitto)
        2:      plim (voitto)
        3:      kop
        4:      kantti2 (tuplaus)
        5:      basso2 (tuplaus)
        6:      panosnuotti
        7:      rumpu0              (voitto)
        8:      rumpu1              (voitto)
        9:      tuplaus hävitty #1
        10:     sekoitus
        11:     liuku
        12:     korttien pudotus
        13:     kortti asettuu
        14:     kortti kääntyy
        15:     valinta
        16:     maksu
        17:     rumpu2              (tuplaus)
        18:     tuplaus hävitty #2
        19:     kantti3             (päävoitto)
        20:     pim                 (päävoitto)
        21:     rumpu3              (päävoitto)
        22:     tiuku               (päävoitto)
        23:     rumpu4              (päävoitto)
        24:     plim
        25:     keno hävitty #1
        26:     keno hävitty #2
    ***/

const unsigned short *A_tyhja[] = { KANAVAT_LOPPU };

const unsigned short A_Mvoitto0[] = {
    SOITIN(0, 0),
    PAALLE(  0, NC_(4)), POIS(120),
    PAALLE(120, NG_(3)), POIS(120),
    PAALLE(120, NA_(3)), POIS(120),
    PAALLE(120, NB_(3)), POIS(120),
    ALKUUN(120),
};
const unsigned short A_Mvoitto1[] = {
    SOITIN(0, 1),
    PAALLE(  0, NC_(3)), POIS( 60),
    PAALLE(180, NG_(2)), POIS(180),
    PAALLE( 60, NA_(2)), POIS( 60),
    PAALLE(180, NB_(2)), POIS(180),
    LOPPU(240)
};
const unsigned short A_Mvoitto2[] = {
    SOITIN(0, 2),
    PAALLE(120, NC_(5)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE(180, NA_(4)), POIS(60),
    PAALLE(180, NB_(4)), POIS(60),
    LOPPU(240)
};
const unsigned short A_Mvoitto3[] = {
    SOITIN(0, 7),
    PAALLE(15, NC_(0)), POIS(105), 
    PAALLE(15, NC_(0)), POIS(105),
    SOITIN(0, 8),
    PAALLE(15, NC_(0)), POIS(105),
    SOITIN(0, 7),
    PAALLE(15, NC_(0)), POIS(105),
    PAALLE(15, NC_(0)), POIS(105), 
    PAALLE(15, NC_(0)), POIS(105),
    SOITIN(0, 8),
    PAALLE(15, NC_(0)), POIS(105), 
    PAALLE(15, NC_(0)), POIS(105),
    LOPPU(240)
};
AANI_KANAVIA_3(A_Mvoitto);

const unsigned short A_Mtuplaus0[] = {
    SOITIN(0, 4),
    PAALLE( 0, NC_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NA_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NG_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NF_(4)), POIS(60),
             ODOTA(120),          PAALLE(60, NF_(4)), POIS(60),
    PAALLE(60, NE_(4)), POIS(60), PAALLE(60, ND_(4)), POIS(60),

    PAALLE(60, NC_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NA_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NG_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NF_(4)), POIS(60),
             ODOTA(120),          PAALLE(60, NF_(4)), POIS(60),
    PAALLE(60, NE_(4)), POIS(60), PAALLE(60, ND_(4)), POIS(60),
    
    PAALLE(60, NC_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NA_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NG_(4)), POIS(60), PAALLE(60, NG_(4)), POIS(60),
    PAALLE(60, NFs(4)), POIS(60), PAALLE(60, NF_(4)), POIS(60),
             ODOTA(120),          PAALLE(60, NF_(4)), POIS(60),
    PAALLE(60, NE_(4)), POIS(60), PAALLE(60, ND_(4)), POIS(60),

    PAALLE( 60, NG_(3)), POIS(60),
    PAALLE(180, NG_(3)), POIS(60),
    PAALLE(180, NG_(3)), POIS(60),
    PAALLE(180, NG_(3)), POIS(60),
    PAALLE(180, NG_(3)), POIS(60),
    PAALLE(180, NG_(3)), POIS(60),
    PAALLE(180, NG_(3)), POIS(60),
    PAALLE( 60, NA_(3)), POIS(60),
    PAALLE( 60, NB_(3)), POIS(60),
    PAALLE( 60, NG_(3)), POIS(60),

    ALKUUN(60),
};
const unsigned short A_Mtuplaus1[] = {
    SOITIN(0, 5),
    PAALLE( 0, NC_(3)), POIS(60),         ODOTA(120),         
    PAALLE(60, NFs(2)), POIS(60), PAALLE(60, NG_(2)), POIS(60),
    PAALLE(60, NC_(3)), POIS(60),         ODOTA(120),         
    PAALLE(60, NFs(2)), POIS(60), PAALLE(60, NG_(2)), POIS(60),
             ODOTA(120),                  ODOTA(120),         
    PAALLE(60, NE_(2)), POIS(60), PAALLE(60, NF_(2)), POIS(60),
             ODOTA(120),          PAALLE(60, NB_(2)), POIS(60),
    PAALLE(60, NB_(2)), POIS(60), PAALLE(60, NB_(2)), POIS(60),

    PAALLE( 60, NC_(3)), POIS(60),         ODOTA(120),         
    PAALLE(60, NFs(2)), POIS(60), PAALLE(60, NG_(2)), POIS(60),
    PAALLE(60, NC_(3)), POIS(60),         ODOTA(120),         
    PAALLE(60, NFs(2)), POIS(60), PAALLE(60, NG_(2)), POIS(60),
             ODOTA(120),                  ODOTA(120),         
    PAALLE(60, NE_(2)), POIS(60), PAALLE(60, NF_(2)), POIS(60),
             ODOTA(120),          PAALLE(60, NB_(2)), POIS(60),
    PAALLE(60, NB_(2)), POIS(60), PAALLE(60, NB_(2)), POIS(60),
    
    PAALLE( 60, NC_(3)), POIS(60),         ODOTA(120),         
    PAALLE(60, NFs(2)), POIS(60), PAALLE(60, NG_(2)), POIS(60),
    PAALLE(60, NC_(3)), POIS(60),         ODOTA(120),         
    PAALLE(60, NFs(2)), POIS(60), PAALLE(60, NG_(2)), POIS(60),
             ODOTA(120),                  ODOTA(120),         
    PAALLE(60, NE_(2)), POIS(60), PAALLE(60, NF_(2)), POIS(60),
             ODOTA(120),          PAALLE(60, NB_(2)), POIS(60),
    PAALLE(60, NB_(2)), POIS(60), PAALLE(60, NB_(2)), POIS(60),

    PAALLE(60, NG_(2)), POIS(60),
    LOPPU(240)
};
const unsigned short A_Mtuplaus2[] = {
    SOITIN(0, 17),
    
             ODOTA(120),                   ODOTA(120),          
    PAALLE(0, NC_(0)),  POIS(60),          ODOTA(120),           
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60), PAALLE(60, NC_(0)), POIS(60), 
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           

             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60), PAALLE(60, NC_(0)), POIS(60), 
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           

             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60), PAALLE(60, NC_(0)), POIS(60), 
             ODOTA(120),                   ODOTA(120),          
    PAALLE(60, NC_(0)), POIS(60),          ODOTA(120),           

            ODOTA(720),          
    PAALLE(60, NC_(0)), POIS(60), 

    LOPPU(240)
};
const unsigned short A_Mtuplaus3[] = {
    SOITIN(0, 4),
    PAALLE( 0, NC_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NA_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NG_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NF_(5)), POIS(60),
             ODOTA(120),          PAALLE(60, NF_(5)), POIS(60),
    PAALLE(60, NE_(5)), POIS(60), PAALLE(60, ND_(5)), POIS(60),

    PAALLE(60, NC_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NA_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NG_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NF_(5)), POIS(60),
             ODOTA(120),          PAALLE(60, NF_(5)), POIS(60),
    PAALLE(60, NE_(5)), POIS(60), PAALLE(60, ND_(5)), POIS(60),
    
    PAALLE(60, NC_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NA_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NG_(5)), POIS(60), PAALLE(60, NG_(5)), POIS(60),
    PAALLE(60, NFs(5)), POIS(60), PAALLE(60, NF_(5)), POIS(60),
             ODOTA(120),          PAALLE(60, NF_(5)), POIS(60),
    PAALLE(60, NE_(5)), POIS(60), PAALLE(60, ND_(5)), POIS(60),

    PAALLE( 60, NG_(4)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE(180, NG_(4)), POIS(60),
    PAALLE( 60, NA_(4)), POIS(60),
    PAALLE( 60, NB_(4)), POIS(60),
    PAALLE( 60, NG_(4)), POIS(60),

    ALKUUN(60),
};

AANI_KANAVIA_4(A_Mtuplaus);

const unsigned short A_Mtsajaja0[] = {
    SOITIN(0, 19),
    PAALLE( 0, NC_(3)), POIS(360),
    PAALLE( 0, NDs(3)), POIS(120),
    PAALLE( 0, NAs(2)), POIS(240),
    ODOTA(240),
    
    PAALLE( 0, NC_(3)), POIS(360),
    PAALLE( 0, NDs(3)), POIS(120),
    PAALLE( 0, NAs(2)), POIS(240),
    ODOTA(240),
      
    PAALLE( 0, NC_(4)), POIS(240),
    PAALLE( 0, NDs(4)), POIS(120),
    PAALLE( 0, NAs(4)), POIS(120),
    PAALLE( 0, NF_(4)), POIS(90),
    PAALLE( 0, ND_(4)), POIS(90),
    PAALLE( 0, NAs(3)), POIS(60),
    ODOTA(240),
    
    PAALLE( 0, NG_(4)), POIS(240),
    PAALLE( 0, NAs(4)), POIS(240),
    PAALLE( 0, NA_(4)), POIS(480),

    LOPPU(480)
};

const unsigned short A_Mtsajaja1[] = {
    SOITIN(0, 22),
    ODOTA(780),
    PAALLE( 0, NC_(0)), POIS(30),
    PAALLE(30, NC_(0)), POIS(30),
    PAALLE(30, NC_(0)), POIS(30),
    ODOTA(30),
    
    ODOTA(780),
    PAALLE( 0, NC_(0)), POIS(30),
    PAALLE(30, NC_(0)), POIS(30),
    PAALLE(30, NC_(0)), POIS(30),
    ODOTA(30),
      
    SOITIN(0, 19),
    PAALLE( 0, NC_(3)), POIS(360),
    PAALLE( 0, NDs(3)), POIS(120),
    PAALLE( 0, ND_(3)), POIS(240),
    ODOTA(240),
    
    PAALLE( 0, NB_(2)), POIS(480),
    PAALLE( 0, ND_(4)), POIS(480),

    LOPPU(480)
};

const unsigned short A_Mtsajaja2[] = {
    SOITIN(0, 20),
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE(165, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
      
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NB_(2)), POIS(15),
    PAALLE(105, NC_(3)), POIS(15),
    PAALLE( 15, NAs(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NAs(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
    
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE(165, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
      
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NB_(2)), POIS(15),
    PAALLE(105, NC_(3)), POIS(15),
    PAALLE( 15, NAs(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NAs(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
    
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE(165, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
      
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NB_(2)), POIS(15),
    PAALLE(105, NC_(3)), POIS(15),
    PAALLE( 15, NAs(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NAs(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
    
    PAALLE( 60, NC_(3)), POIS(15),
    PAALLE( 45, NF_(2)), POIS(15),
    PAALLE( 15, NG_(2)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE(165, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    PAALLE( 15, NC_(3)), POIS(15),
    ODOTA(45),
    
    LOPPU(480)
};

const unsigned short A_Mtsajaja3[] = {
    SOITIN(0, 21),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE(  0, NC_(0)), POIS(30),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    SOITIN(0, 23),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 30, NC_(0)), POIS(30),
    PAALLE( 30, NC_(0)), POIS(30),
    ODOTA(30),

    SOITIN(0, 21),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE(  0, NC_(0)), POIS(30),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    SOITIN(0, 23),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 30, NC_(0)), POIS(30),
    PAALLE( 30, NC_(0)), POIS(30),
    ODOTA(30),

    SOITIN(0, 21),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE(  0, NC_(0)), POIS(30),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    SOITIN(0, 23),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 30, NC_(0)), POIS(30),
    PAALLE(  0, NC_(0)), POIS(30),
    PAALLE(  0, NC_(0)), POIS(30),
    ODOTA(30),
    
    SOITIN(0, 21),
    PAALLE( 60, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE( 90, NC_(0)), POIS(30),
    PAALLE(  0, NC_(0)), POIS(30),
    PAALLE( 60, NC_(0)), POIS(30),
    LOPPU(480)
};

AANI_KANAVIA_4(A_Mtsajaja);

const unsigned short A_Mkonkka0[] = {
    SOITIN(0, 6), PAALLE(0, NC_(4)), POIS(240), LOPPU(240)
};
const unsigned short A_Mkonkka1[] = {
    SOITIN(0, 6), PAALLE(0, NE_(4)), POIS(240), LOPPU(240)
};
const unsigned short A_Mkonkka2[] = {
    SOITIN(0, 6), PAALLE(0, NG_(4)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_3(A_Mkonkka);

const unsigned short A_Mkoputus0[] = {
    SOITIN(0, 3),
    PAALLE(  0, NG_(0)), POIS( 60),
    PAALLE( 75, NA_(0)), POIS( 60),
    PAALLE(285, NG_(0)), POIS( 60),
    PAALLE( 75, NA_(0)), POIS( 60),
    PAALLE(285, NG_(0)), POIS( 60),
    PAALLE( 75, NA_(0)), POIS( 60),
    PAALLE(285, NG_(0)), POIS( 60),
    PAALLE( 75, NA_(0)), POIS( 60),
    ALKUUN(285)
};
AANI_KANAVIA_1(A_Mkoputus);

const unsigned short A_Asekoita0[] = {
    SOITIN(0, 10),
    PAALLE(0, NF_(0)), POIS(180),
    PAALLE(60, NE_(0)), POIS(180),
    PAALLE(60, ND_(0)), POIS(180),
    PAALLE(60, NC_(0)), POIS(180),
    LOPPU(60) 
};
AANI_KANAVIA_1(A_Asekoita);

const unsigned short A_Aliuku0[] = {
    SOITIN(0, 11),
    PAALLE(0, NC_(0)), POIS(30), LOPPU(240)
};
AANI_KANAVIA_1(A_Aliuku);

const unsigned short A_Apudotus0[] = {
    SOITIN(0, 12),
    PAALLE(0, NC_(0)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_1(A_Apudotus);

const unsigned short A_Akaantyy0[] = {
    SOITIN(0, 14),
    PAALLE(0, NC_(0)), POIS(15), LOPPU(240)
};
AANI_KANAVIA_1(A_Akaantyy);

const unsigned short A_Avalinta0[] = {
    SOITIN(0, 15),
    PAALLE(0, NC_(0)), POIS(30), LOPPU(240)
};
AANI_KANAVIA_1(A_Avalinta);

const unsigned short A_Aasettuu0[] = {
    SOITIN(0, 13),
    PAALLE(0, NC_(0)), POIS(30), LOPPU(240)
};
AANI_KANAVIA_1(A_Aasettuu);

const unsigned short A_Amaksu0[] = {
    SOITIN(0, 16),
    PAALLE(0, NC_(0)), POIS(45), LOPPU(240)
};
AANI_KANAVIA_1(A_Amaksu);

const unsigned short A_Aperkele0[] = {
    SOITIN(0, 9), PAALLE(0, NA_(0)), POIS(60),
    SOITIN(0, 18), PAALLE(60, NA_(0)), POIS(120), LOPPU(480) 
};
AANI_KANAVIA_1(A_Aperkele);

const unsigned short A_Aaloita0[] = {
    SOITIN(0, 6), PAALLE(0, NC_(4)), POIS(240), LOPPU(240)
};
const unsigned short A_Aaloita1[] = {
    SOITIN(0, 6), PAALLE(0, NE_(4)), POIS(240), LOPPU(240)
};
const unsigned short A_Aaloita2[] = {
    SOITIN(0, 6), PAALLE(0, NG_(4)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_3(A_Aaloita);

const unsigned short A_Apanos10[] = {
    SOITIN(0, 6), PAALLE(0, NC_(5)), POIS(240), LOPPU(240)
};
const unsigned short A_Apanos11[] = {
    SOITIN(0, 6), PAALLE(0, NF_(4)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_2(A_Apanos1);

const unsigned short A_Apanos20[] = {
    SOITIN(0, 6), PAALLE(0, ND_(5)), POIS(240), LOPPU(240)
};
const unsigned short A_Apanos21[] = {
    SOITIN(0, 6), PAALLE(0, NG_(4)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_2(A_Apanos2);

const unsigned short A_Apanos30[] = {
    SOITIN(0, 6), PAALLE(0, NF_(5)), POIS(240), LOPPU(240)
};
const unsigned short A_Apanos31[] = {
    SOITIN(0, 6), PAALLE(0, NAs(4)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_2(A_Apanos3);

const unsigned short A_Apanos40[] = {
    SOITIN(0, 6), PAALLE(0, NG_(5)), POIS(240), LOPPU(240)
};
const unsigned short A_Apanos41[] = {
    SOITIN(0, 6), PAALLE(0, NC_(5)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_2(A_Apanos4);

const unsigned short A_Apanos50[] = {
    SOITIN(0, 6), PAALLE(0, NA_(5)), POIS(240), LOPPU(240)
};
const unsigned short A_Apanos51[] = {
    SOITIN(0, 6), PAALLE(0, ND_(5)), POIS(240), LOPPU(240)
};
AANI_KANAVIA_2(A_Apanos5);

const unsigned short A_Akenoohi0[] = {
    SOITIN(0, 25), PAALLE(0, NA_(0)), POIS(60),
    SOITIN(0, 26), PAALLE(60, NA_(0)), POIS(120), LOPPU(480) 
};
AANI_KANAVIA_1(A_Akenoohi);
