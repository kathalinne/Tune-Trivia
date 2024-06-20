#ifndef MUSIC_H_
#define MUSIC_H_


#define C       1048//786//262//4000 
#define D       1176//882//294//4500 
#define E       1320//990//330//5000 
#define G       1568//1176//392//6000 
#define B       1000

#define E3      820
#define G3      1372
#define D3      1606
#define C3      1040
#define B3      1107

#define CSHARP  554//1108//277
#define A       440//880//220
#define GSHARP  830//1660//415

#define L_F      349 //1396//2094 //349
#define L_A      440 //1760//2640 //440
#define L_E      330 //1316//1974 //329
#define L_D      293// 1172//293

#define PAUSE 0
int maryHadALittleLamb[] = {E, E, PAUSE, D,             //4
                            D, PAUSE, C, C,             //8
                            PAUSE, D, D, PAUSE,         //12
                            E, E, PAUSE, E,             //16
                            E, PAUSE, E, E,             //20
                            E, E, E, E,                 //24
                            PAUSE, D, D, PAUSE,         //28
                            D, D, PAUSE, D,             //32
                            D, D, D, D,                 //36
                            D, PAUSE, E, E,             //40
                            PAUSE, G, G, PAUSE,         //44
                            G, G, G, G,                 //48
                            G, G, PAUSE, E,             //52
                            E, PAUSE, D, D,             //56
                            PAUSE, C, C, PAUSE,         //60
                            D, D, PAUSE, E,             //64
                            E, PAUSE, E, E,             //68
                            PAUSE, E, E, PAUSE,         //72
                            E, E, PAUSE, D,             //76
                            D, PAUSE, D, D,             //80
                            PAUSE, E, E, PAUSE,         //84
                            E, E, PAUSE, E,             //88
                            E, E, E, E,
                            E, E, E, E};                //96

int sevenNationArmy[] = {   E3, E3, E3, E3, 
                            PAUSE, E3, E3, E3, 
                            G3, G3, G3, PAUSE, 
                            E3, E3, PAUSE, D3, 
                            D3, D3, PAUSE, C3, 
                            C3, C3, C3, C3, 
                            C3, B3, B3, B3, 
                            B3, B3, PAUSE, /*repeat*/E3, 
                            E3, E3, E3, PAUSE, 
                            E3, E3, E3, G3, 
                            G3, G3, PAUSE, E3, 
                            E3, PAUSE, D3, D3, 
                            D3, PAUSE, C3, C3, 
                            C3, C3, C3, C3, 
                            B3, B3, B3, B3, 
                            B3, PAUSE, /*next*/E3, E3, 
                            E3, E3, E3, PAUSE, 
                            E3, E3, E3, G3, 
                            G3, G3, PAUSE, E3, 
                            E3, PAUSE, D3, D3, 
                            D3, PAUSE, C3, C3,
                            C3, PAUSE, D3, D3, 
                            D3, PAUSE, C3, C3, 
                            C3, B3, B3, B3};     //96

#endif