#include "ch.h"
#include "hal.h"

#include "easter_egg.h"
#include "display.h"
#include "rand.h"

static uint8_t gamescreen[5];
static char charbuf[16] = "SCORE:";

static void game_set_pixel(int x, int y){
    gamescreen[4-x] |= 1 << (4-y);
}

static void game_clear(void){
    int i;
    for(i=0; i<5; i++){
        gamescreen[i] = 0;
    }
}

static void game_show_score(int s){
    char buf[8];
    int i = 0;
    if(s==0){
        buf[i++] = '0';
    }else{
        while(s > 0){
            buf[i++] = 48 + (s % 10);
            s /= 10;
        }
    }
    int j;
    for(j=0; j<i; j++){
        charbuf[j+6] = buf[i-(j+1)];
    }
    charbuf[j+6] = 0; // null-terminated
}

void easter_egg_start(){
    int height = 2;
    int pipex = 10;
    int pipey = 2;
    int score = 0;
    bool running = true;
    bool released = false;
    int dropcounter = 0;
    int scrollcounter = 0;
    
    while(running){
        // Move up if held, else drop
        if(palReadLine(LINE_SWITCH)){
            if(released){
                height -= 1;
                if(height < 0){
                    height = 0;
                }
            }
            dropcounter = 0;
            released = false;
        }else{
            if(dropcounter > 3){
                dropcounter = 0;
                height += 1;
                if(height > 4){
                    running = false;
                }
            }
            dropcounter++;
            released = true;
        }
        
        if(scrollcounter == 2){ // Scroll right every 2 ticks
            scrollcounter = 0;
            pipex--;
            
            if(pipex == 1){ // Pipe is aligned with player
                if ( (height != pipey) && (height != (pipey+1)) ){
                    running = false;
                }else{
                    score++;
                }
            }
            
            if(pipex < 0){
                pipey = rand() % 4;
                pipex = 10;
            }
        }else{
            scrollcounter++;
        }
        
        game_clear();
        
        // Render the pipe
        if(pipex <= 4){
            gamescreen[4-pipex] = ~(3<<(3-pipey));
        }
        
        // Render the player
        game_set_pixel(1, height);
        
        // Display the screen
        display_copy(gamescreen);
        
        chThdSleepMilliseconds(100);
    }
    
    // Show score and exit
    game_show_score(score);
    display_clear();
    display_scroll_text(charbuf);
    while(!display_scroll()){
        chThdSleepMilliseconds(80);
    }
}
