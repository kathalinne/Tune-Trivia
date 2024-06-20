#include "timerISR-Fixed.h"
#include "helper.h"
#include "periph.h"
#include "serialATmega.h"
#include "LCD.h"
#include "irAVR.h"
#include "music.h"
#include "eeprom.h"
#include "stdlib.h"

#define REMOTE_ZERO         0xFF6897
#define REMOTE_PLAY_PAUSE   0xFF02FD
#define REMOTE_DOWN         0xFFE01F
#define REMOTE_UP           0xFF906F
#define REMOTE_FAST_FORWARD 0xFFC23D
#define REMOTE_REVERSE      0xFF22DD

#define PLAYLIST  1


#define NUM_TASKS 9

typedef struct _task{
	signed 	 char state; 		//Task's current state
	unsigned long period; 		//Task period
	unsigned long elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;

//IR_REMOTE 1
const unsigned long REMOTE_PERIOD = 500; 
//WELCOME 2
const unsigned long WELCOME_PERIOD = 500;
//MENU 3
const unsigned long MENU_PERIOD = 500;
//CD 4
const unsigned long CD_PERIOD = 1000; //
//MUSIC_PLAYER 5
const unsigned long MUSICPLAYER_PERIOD = 100; 
//POINTS_AWARDED 6
const unsigned long POINTSAWARDED_PERIOD = 500; //
//GUESS_SONG 7
const unsigned long GUESS_SONG_PERIOD = 500;
//CUSTOM_CHAR 8
const unsigned long CUSTOMCHAR_PERIOD = 2000;
//HIGH_SCORE 9
const unsigned long HIGHSCORE_PERIOD = 500;
//CURRENT_SCORE 9
//const unsigned long CURRENT_SCORE_PERIOD = 500;

const unsigned long GCD_PERIOD = 100;

task tasks[NUM_TASKS]; // declared task array with 5 tasks

bool HIGH(int);

void TimerISR() {
	for ( unsigned int i = 0; i < NUM_TASKS; i++ ) {                   // Iterate through each task in the task array
		if ( tasks[i].elapsedTime == tasks[i].period ) {           // Check if the task is ready to tick
			tasks[i].state = tasks[i].TickFct(tasks[i].state); // Tick and set the next state for this task
			tasks[i].elapsedTime = 0;                          // Reset the elapsed time for the next tick
		}
		tasks[i].elapsedTime += GCD_PERIOD;                        // Increment the elapsed time by GCD_PERIOD
	}
}

enum IRremote{IRStart, Remote_Readings};                                                          
enum Welcoming{WelcomeStart, Welcome};                                                            
enum Menu{MenuStart, MenuIdle, Game_Round, Past_Highest_Score};
enum CountDown{CntDownStart, CD_Idle, CD_1, CD_2, CD_3, CD_4};                                                                                       //
enum MusicPlayer{MusicPlayerStart, MusicPlayerWait, S1};                                          
enum PointsAwardedDisplay{PointsAwardedStart, Pts_Idle, No_Pts, P1_Pts, P2_Pts};
enum GuessSongTitle{GuessSongStart, GuessSongIdle, GuessSong1, GuessSong2};
enum CustomCharDisplay{CustomCharStart, CustomWait, CustomDisplay};
enum HighScoreDisplay{HighScoreStart, HighScoreIdle, HighScoreDisplay};  
//enum CurrentScoreDisplay{CurrentScorStart, CurrentScoreIdle, CurrentScoreDisplay}

//Global variables:

unsigned char pause = 0;
unsigned char gameLevel = 1;


unsigned char playerOnePts = 0;
unsigned char playerTwoPts = 0;

unsigned char readyToGuess = 0;

unsigned char songBusy = 0;
unsigned int output1 = 0;

unsigned char scoresIndex = 9;
char* scores[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};


//IR_REMOTE 1
decode_results d;
unsigned long input = 0;

//WELCOME 2
unsigned char gameOn = 0;

//MENU 3
unsigned char eeprom_display = 0;
unsigned char currentScoreDisplay = 0;
unsigned char nextRound = 0;
unsigned char countDownReady = 0;
unsigned char roundBusy = 0;

//CD 4
unsigned char songReady = 0;

//MUSIC_PLAYER 5
unsigned char endOfSong = 0;
unsigned char guessTitleP1 = 0;
unsigned char guessTitleP2 = 0;
unsigned char musicStopped = 0;

//fix
unsigned char availablePoints = 0;

//POINTS_AWARDED 6
unsigned char nextRound = 0;
unsigned char gameOverDisplay = 0;

//GUESS_SONG 7
unsigned char guessedSongTitle = 0;
unsigned char choice = 0;

//CUSTOM_CHAR 8

//HIGH_SCORE 9

//CURRENT_SCORE 10

//GAME_OVER_DISPLAY 11
unsigned char gameOver = 0;



int TickFct_CustomCharDisplay(int);

int TickFct_IRremote(int state){
  switch(state){
    case IRStart:
      state = Remote_Readings;
    break;

    case Remote_Readings:
    break;

    default:
      state = IRStart;
    break;
  }
  switch(state){
    case IRStart:
    break;

    case Remote_Readings:
      if (IRdecode(&d)) {
          //if (IRdecode(&d)) {
            input = d.value;
            IRresume();
          //} 
        //serial_println(input);
      } else {
        input = 0;
      }

      //serial_println(input);
    break;
  }
    return state;
}

unsigned char points = 15;
int TickFct_Welcoming(int state){
  switch(state){
    case WelcomeStart:
      state = Welcome;
    break;

    case Welcome:
    break;

    default:
    state = WelcomeStart;
    break;
  }

  switch(state){
    
    case WelcomeStart:
    break;

    case Welcome:
    if(!gameOn && input == REMOTE_PLAY_PAUSE){
      lcd_clear();
      gameOn = 1;
    } 
    else if(gameOver){
      gameOn = 0;
    }
    else if(!gameOn){
      lcd_clear();
      lcd_goto_xy(0,0);
      lcd_write_str("Welcome! Press");
      lcd_goto_xy(1,0);
      lcd_write_str(">|| to begin!");
    }
    break;
  }

  return state;
}

unsigned currentScoresBusy = 0;

int TickFct_Menu(int state){
  switch(state){
    case MenuStart:
      state = MenuIdle;
    break;

    case MenuIdle:
      if(gameOn){
        state = Game_Round;
      }
    break;

    case Game_Round:
      if(input == 0xFFE01F){
        state = Past_Highest_Score;
      } else if(input == 0xFF6897){
        lcd_clear();
        state = MenuIdle;
      }
      //serial_println(d.value, 16);
    break;

    case Past_Highest_Score:
      //IRdecode(&d);
      if(input == 0xFF906F){
        state = Game_Round;
      } else if(input == 0xFF6897){
        lcd_clear();
        state = MenuIdle;
      }
      //serial_println(d.value);
    break;

    default:
    state = MenuStart;
    break;
  }

  switch(state){
    case MenuStart:
    break;

    case MenuIdle:
      //serial_println("in Menu Idle");
    break;

    case Game_Round:
      if(!roundBusy){
        lcd_clear();
        lcd_goto_xy(0,0);
        lcd_write_str("Press >>| to");
        lcd_goto_xy(1,0);
        lcd_write_str("continue       V");
      }
      if(input == REMOTE_FAST_FORWARD){
        lcd_clear();
        roundBusy = 1;
        countDownReady = 1;
      }
      if(songReady){
        countDownReady = 0;
      }
    break;

    case Past_Highest_Score:
      lcd_clear();
      if(!eeprom_display){
          lcd_goto_xy(0,0);
          lcd_write_str("Past >>|       ^");
          lcd_goto_xy(1,0);
          lcd_write_str("highest score");
      }

      if(input == REMOTE_FAST_FORWARD){
        eeprom_display = 1;
      } else if(input == REMOTE_REVERSE){
        eeprom_display = 0;
      }
    break;
  }

  return state;
}

int TickFct_MusicPlayer(int state){
    switch (state){
        case MusicPlayerStart:
            state = MusicPlayerWait;
        break;

        case MusicPlayerWait:
            if(songReady && !(guessTitleP1 || guessTitleP2)){
                if(gameLevel == 1){
                    state = S1;
                }
            }

        break;

        case S1:
            if( guessTitleP1 ||  guessTitleP2 ){
                OCR1A = 0;
                songReady = 0;
                state = MusicPlayerWait;
            }
        break;

        break;

    default:
        state = MusicPlayerStart;
    break;
    }

    switch(state){
        static unsigned char P1;
        static unsigned char P2;
        case MusicPlayerStart:
        break;

        case MusicPlayerWait:
            OCR1A = 0;
            if(nextRound || gameOver){
              endOfSong = 0;
              guessedSongTitle = 0;
            }
        break;

        case S1:
            static unsigned char i = 0;
            static unsigned char pointCountDown1 = 96;
            P1 = ADC_read(0);
            P2 = ADC_read(1);
            //once the index instead
            if(i == 96){
                //serial_println("EOS is 0");
                endOfSong = 1;
                OCR1A = 0;
                availablePoints = output1;
                serial_println("available Points");
                serial_println(availablePoints);
            } else {
                //serial_println("Index");
                //serial_println(i);
                //serial_println(maryHadALittleLamb[i]);
                ICR1 = maryHadALittleLamb[i];
                OCR1A = maryHadALittleLamb[i]/2;
                // serial_println("iterating for:");
                // serial_println(i);
                i++;
            }
            if(input == REMOTE_ZERO){
              i = 0;
            }

            if(pointCountDown1 == 0){
              pointCountDown1 = 96;
            } else {
              output1 = map_value(0, 96, 0, 9, pointCountDown1);
              serial_println(output1);
              pointCountDown1--;
            }

            if( !endOfSong && (HIGH(P1)) ){
                //playerOnePts = scoresIndex;
                // EEPROM_write(0x00, playerOnePts);
                guessTitleP1 = 1;
                songReady = 0;
                //serial_println("Player 1 got:");
                //serial_println(playerOnePts);
                //gameLevel++;
            } else if( !endOfSong && (HIGH(P2)) ){
                //playerTwoPts = scoresIndex;
                guessTitleP2 = 1;
                songReady = 0;
                //EEPROM_write(0x00, playerTwoPts);
                // availablePoints = 15;
                //serial_println("Player 2 got:");
                //serial_println(playerTwoPts);
                //gameLevel++;
            }

        break;
    }

    return state;
}


unsigned char cnt = 0;
int TickFct_PointsAwardedDisplay(int state){
  switch(state){
    case PointsAwardedStart:
      state = Pts_Idle;
    break;
    
    case Pts_Idle:
      if(endOfSong){
        state = No_Pts;
      }

      if(guessedSongTitle && guessTitleP1){
        state = P1_Pts;
      } else if(guessedSongTitle && guessTitleP2){
        state = P2_Pts;
      }

    break;
    
    case No_Pts:
      if(cnt == 10){
        cnt = 0;
        lcd_clear();
        gameOver = 1;
        state = Pts_Idle;
      }
    break;
    
    case P1_Pts:
      if(cnt == 10){
        cnt = 0;
        lcd_clear();
        gameOver = 1;
        state = Pts_Idle;
      }
    break;
    
    case P2_Pts:
      if(cnt == 10){
           cnt = 0;
        lcd_clear();
        gameOver = 1;
        state = Pts_Idle;
      }
    break;

    default:
    state = PointsAwardedStart;
    break;
  }

  switch(state){
    case PointsAwardedStart:
    break;
    
    case Pts_Idle:
    break;
    
    case No_Pts:
      lcd_clear();
      lcd_goto_xy(0,0);
      lcd_write_str("No points");
      lcd_goto_xy(1,0);
      lcd_write_str("awarded :(");
      cnt++;
    break;
    
    case P1_Pts:
      lcd_clear();
      lcd_goto_xy(0,0);
      lcd_write_str("Player 1 got");
      lcd_goto_xy(0,13);
      lcd_write_str(scores[output1]);
      if(output1 > EEPROM_read(0x00)){
         EEPROM_write(0x00, output1);
      }
      lcd_goto_xy(1,0);
      lcd_write_str("more points!");
      
      cnt++;
    break;
    
    case P2_Pts:
      //char str2[100];
      //itoa(availablePoints, str2, 10);
      lcd_clear();
      lcd_goto_xy(0,0);
      lcd_write_str("Player 2 got");
      lcd_goto_xy(0,13);
      lcd_write_str(scores[output1]);
       if(output1 > EEPROM_read(0x00)){
         EEPROM_write(0x00, output1);
      }
      lcd_goto_xy(1,0);
      lcd_write_str("more points!");
      cnt++;
    break;

  }
  return state;
}

int TickFct_HighScoreDisplay(int state){
  switch(state){

    case HighScoreStart:
      state = HighScoreIdle;
    break;

    case HighScoreIdle:;
      if(eeprom_display){
        state = HighScoreDisplay;
      }
    break;

    case HighScoreDisplay:
      if(!eeprom_display){
        lcd_clear();
        state = HighScoreIdle;
      }
    break;

    default:
    state = HighScoreStart;
    break;
  }

  switch(state){

    case HighScoreStart:
    break;

    case HighScoreIdle:
    break;

    case HighScoreDisplay:
    if(eeprom_display){
      lcd_clear();
      unsigned char lastScore;
      lastScore = EEPROM_read(0x00);
      serial_println(lastScore);
      lcd_write_str(scores[lastScore]);
    }
    break;

  }
  return state;
}


unsigned int count = 4;
int TickFct_CountDown(int state){
    switch(state){
      case CntDownStart:
        state = CD_Idle;
      break;
      
      case CD_Idle:
        if(countDownReady){
          state = CD_1;
        }
        if(songBusy){
          songReady = 0;
        }
      break;

      case CD_1:
        if(count == 3){
          state = CD_2;
        }
      break;

      case CD_2:
        if(count == 2){
          state = CD_3;
        }
      break; 

      case CD_3:
        if(count == 1){
          state = CD_4;
        }
      break;

      case CD_4:
        if(count == 0){
          lcd_clear();
          count = 4;
          songReady = 1;
          state = CD_Idle;
        }
      break;

      default:
      state = CntDownStart;
      break;
    }

    switch(state){
      case CntDownStart:
      break;
      
      case CD_Idle:
      break;

      case CD_1:
        lcd_clear();
        lcd_goto_xy(0,0);
        lcd_write_str("Game starts in:");
        count--;
      break;

      case CD_2:
        lcd_clear();
        lcd_goto_xy(0,0);
        lcd_write_str("Game starts in:");
        lcd_goto_xy(1,3);
        lcd_write_str("3");
        count--;
      break; 

      case CD_3:
        lcd_clear();
        lcd_goto_xy(0,0);
        lcd_write_str("Game starts in:");
        lcd_goto_xy(1,3);
        lcd_write_str("3");
        lcd_goto_xy(1,7);
        lcd_write_str("2");
        count--;
      break;

      case CD_4:
        lcd_clear();
        lcd_goto_xy(0,0);
        lcd_write_str("Game starts in:");
        lcd_goto_xy(1,3);
        lcd_write_str("3");
        lcd_goto_xy(1,7);
        lcd_write_str("2");
        lcd_goto_xy(1,11);
        lcd_write_str("1");
        count--;
      break;
    }

    return state;
}

int TickFct_GuessSong(int state){
  static unsigned char counter = 0;
  switch(state){
    case GuessSongStart:
      state = GuessSongIdle;
    break;

    case GuessSongIdle:
      if((guessTitleP1 || guessTitleP2)){
        if(gameLevel == 1){
          state = GuessSong1;
        }
      }
    break;

    case GuessSong1:
      if(guessedSongTitle){
        lcd_clear();
        state = GuessSongIdle;
      }
    break;

    case GuessSong2:
      if(guessedSongTitle){
        lcd_clear();
        state = GuessSongIdle;
    }
    break;

    default:
      state = GuessSongStart;
    break;
  }
  switch(state){
    case GuessSongStart:
    break;

    case GuessSongIdle:
    break;

    case GuessSong1:
      if(choice == 0){
          lcd_goto_xy(0,0);
          lcd_write_str("1. Happy     >>|");
          lcd_goto_xy(1,0);
          lcd_write_str("birthday");
        } else if(choice == 1){
          lcd_goto_xy(0,0);
          lcd_write_str("2. Mary had   >>|");
          lcd_goto_xy(1,0);
          lcd_write_str("a little lamb");
        } else if(choice == 2){
          lcd_goto_xy(0,0);
          lcd_write_str("3. Itsy      >>|");
          lcd_goto_xy(1,0);
          lcd_write_str("Bitsy Spider");
        }

        if(input == REMOTE_FAST_FORWARD){
          if(choice == 3){
            choice = 0;
          } else {
            choice++;
          }
        }
        if(input == REMOTE_PLAY_PAUSE && choice == 1){
          if(guessTitleP1){
            playerOnePts += output1;

          } else if(guessTitleP2){
            playerTwoPts += output1;

          }
          guessedSongTitle = 1;
        }
    break;

    case GuessSong2:

      if(choice == 0){
        lcd_goto_xy(0,0);
        lcd_write_str("1.Star      >>|");
        lcd_goto_xy(1,0);
        lcd_write_str("spangled banner");
      } else if(choice == 1){
        lcd_goto_xy(0,0);
        lcd_write_str("2.Seven      >>|");
        lcd_goto_xy(1,0);
        lcd_write_str("nation army");
      } else if(choice == 2){
        lcd_goto_xy(0,0);
        lcd_write_str("3.Smells     >>|");
        lcd_goto_xy(1,0);
        lcd_write_str("Like Teen Spirit");
      }

      if(input == REMOTE_FAST_FORWARD){
        if(choice == 3){
          choice = 0;
        } else {
          choice++;
        }
      }
    break;

  }
  return state;
}


int main(void){

    DDRB = 0xEF;
    PORTB = 0x10;

    DDRC = 0x00;
    PORTC = 0xFF;

    DDRD = 0xFC;
    PORTD = 0x00;


    ADC_init();   // initializes ADC
   // init_sonar(); // initializes sonar
    lcd_init();
    IRinit(&PORTB, &PINB, 4);


    //Initializing the buzzer timer/pwm (timer1)
    TCCR1A |= (1 << WGM11) | (1 << COM1A1); //COM1A1 sets it to channel A
    TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); //CS11 sets the prescaler to be 8

    unsigned char i = 0;

    tasks[i].state = IRStart;
    tasks[i].period = REMOTE_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_IRremote; 		//Task tick function

    i++;
    tasks[i].state = WelcomeStart;
    tasks[i].period = WELCOME_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Welcoming; 		//Task tick function

    i++;
    tasks[i].state = MenuStart;
    tasks[i].period = MENU_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_Menu; 		//Task tick function

    i++;
    tasks[i].state = HighScoreStart;
    tasks[i].period = HIGHSCORE_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_HighScoreDisplay; 		//Task tick function

    i++;
    tasks[i].state = CntDownStart;
    tasks[i].period = CD_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_CountDown; 		//Task tick function

    i++;
    tasks[i].state = MusicPlayerStart;
    tasks[i].period = MUSICPLAYER_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_MusicPlayer; 		//Task tick function

    i++;
    tasks[i].state = PointsAwardedStart;
    tasks[i].period = POINTSAWARDED_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_PointsAwardedDisplay; 		//Task tick function

    i++;
    tasks[i].state = GuessSongStart;
    tasks[i].period = GUESS_SONG_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
    tasks[i].TickFct = &TickFct_GuessSong;

    i++;
    tasks[i].state = CustomCharStart;
    tasks[i].period = CUSTOMCHAR_PERIOD;
    tasks[i].elapsedTime = tasks[i].period;
	  tasks[i].TickFct = &TickFct_CustomCharDisplay; 		//Task tick function

    TimerSet(GCD_PERIOD);
    TimerOn();
    serial_init(9600);

    while (1) {}

  return 0;
}

bool HIGH(int value){
    if( (value >= 250) && (value <= 255) ){
        return true;
    } else {
        return false;
    }
}

int TickFct_CustomCharDisplay(int state){
    static unsigned char cnt = 0;
    switch(state){
        case CustomCharStart:
            state = CustomWait;
        break;

        case CustomWait:
            if(songReady){
                state = CustomDisplay;
            }
        break;

        case CustomDisplay:
        if(!songReady){
            lcd_clear();
            state = CustomWait;
        }
        break;

    default:
    state = CustomCharStart;
    break;
    }

    switch(state){
        case CustomCharStart:
        break;

        case CustomWait:
            lcd_clear();
            //serial_println("in custom wait");
        break;

        case CustomDisplay:

             unsigned char down_music_note[8] = {
                0b00000,
                0b00000,
                0b00000,
                0b01111,
                0b01001,
                0b01001,
                0b11011,
                0b11011
            };


            unsigned char up_music_note[8] = {
                0b01111,
                0b01001,
                0b01001,
                0b11011,
                0b11011,
                0b00000,
                0b00000,
                0b00000
            };


            cnt ++;
            if(cnt%2 == 0){

                lcd_create_custom_char(0, down_music_note);
                // // Display the custom character at the first position of the first row
                lcd_set_cursor(0, 0);
                lcd_data(0); // The address of our custom character


                lcd_create_custom_char(0, up_music_note);
                // // Display the custom character at the first position of the first row
                lcd_set_cursor(0, 1);
                lcd_data(0); // The address of our custom character


                lcd_create_custom_char(0, down_music_note);
                // // Display the custom character at the first position of the first row
                lcd_set_cursor(0, 2);
                lcd_data(0); // The address of our custom character

            } else {

                lcd_create_custom_char(0, up_music_note);
                // // Display the custom character at the first position of the first row
                lcd_set_cursor(0, 0);
                lcd_data(0); // The address of our custom character


                lcd_create_custom_char(0, down_music_note);
                // // Display the custom character at the first position of the first row
                lcd_set_cursor(0, 1);
                lcd_data(0); // The address of our custom character


                lcd_create_custom_char(0, up_music_note);
                // // Display the custom character at the first position of the first row
                lcd_set_cursor(0, 2);
                lcd_data(0); // The address of our custom character
                //serial_println("printing custom char");

            }

        break;
    }

    return state;
}