#include "msp430f2132.h"
#include "stdint.h"
#include "time.h"
#include "stdlib.h"

#define DATA BIT0 
#define CLK BIT1 
#define CHOT BIT2 
#define UP BIT3 
#define RIGHT BIT4 
#define LEFT BIT5 
#define DOWN BIT6
#define START BIT7

int8_t x[64];
int8_t y[64];

int FIRST = 1;
int point;
int8_t mode = 3; // mode1: up    mode2: down      mode3: right     mode4: left
int8_t start = 0;
int8_t SCORE = 0;
int16_t quethang[8] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
int8_t MATRIX [8][8] = { {1, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0},
                      {0, 0, 0, 0, 0, 0, 0, 0}};
/*int8_t MATRIX [8][8] = { {1, 0, 0, 0, 0, 0, 0, 0},
                      {1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1},
                      {1, 1, 1, 1, 1, 1, 1, 1}};*/
void _CLK();
void _CHOT();
void _PRINT();
void _SNAKE();
void _LONG();
void _RESET();

void main( void )
{
  WDTCTL = WDTPW + WDTHOLD;// Stop watchdog timer to prevent time out reset
  x[0] = 0;
  y[0] = 0;
  P1DIR |= DATA + CLK + CHOT;
  P1REN |= (UP + DOWN + LEFT + RIGHT + START);
  P1IE  |= (UP + DOWN + LEFT + RIGHT + START);
  P1IES |= (UP + DOWN + LEFT + RIGHT + START);
  P1IFG &= ~(UP + DOWN + LEFT + RIGHT + START);
  _BIS_SR(GIE);
  int toadoy, toadox;
  srand((unsigned)time(NULL));
  while(1) {
    if(start == 1){
      if(point != SCORE || FIRST == 1) {
        FIRST = 0;
        toadoy = rand() % 8;
        toadox = rand() % 8;
        while(MATRIX[toadoy][toadox]==1 || toadoy > 7 || toadox > 7) {
          toadoy = rand() % 8;
          toadox = rand() % 8;
        }
      }
      point = SCORE;
      MATRIX[toadoy][toadox] = 1;
      _SNAKE();
      if(SCORE==63)start=0;
    }
  }
}

void _SNAKE() {
  
  MATRIX[y[SCORE]][x[SCORE]] = 0;
  
  if(mode==1) { //up
    if(y[0]-1>=0) {
      
      if(MATRIX[y[0]-1][x[0]] == 1)SCORE+=1;
      MATRIX[y[0]-1][x[0]] = 1;
      _LONG();
      y[0] = y[0] - 1;
    } else {
      
      if(MATRIX[7][x[0]] == 1)SCORE+=1;
      MATRIX[7][x[0]] = 1;
      _LONG();
      y[0] = 7;
    }
  }
  
  if(mode==2) { //down
    if(y[0]+1<8) {
      
      if(MATRIX[y[0]+1][x[0]] == 1)SCORE+=1;
      MATRIX[y[0]+1][x[0]] = 1;
      _LONG();
      y[0] = y[0] + 1;
      
    } else { 
      
      if(MATRIX[0][x[0]] == 1)SCORE+=1;
      MATRIX[0][x[0]] = 1;
      _LONG();
      y[0] = 0;
    }
  }
  
  if(mode==3) { //right
    if(x[0]+1<8) {
      
      if(MATRIX[y[0]][x[0]+1] == 1)SCORE+=1;
      MATRIX[y[0]][x[0]+1] = 1;
      _LONG();
      x[0] = x[0] + 1;
    } else {
      
      if(MATRIX[y[0]][0] == 1)SCORE+=1;
      MATRIX[y[0]][0] = 1;
      _LONG();
      x[0] = 0;
    }
  }    
    
  if(mode==4) { //left
    if(x[0]-1>=0) {
      
      if(MATRIX[y[0]][x[0]-1] == 1)SCORE+=1;
      MATRIX[y[0]][x[0]-1] = 1;
      _LONG();
      x[0] = x[0] - 1;
    } else {
      
      if(MATRIX[y[0]][7] == 1)SCORE+=1;
      MATRIX[y[0]][7] = 1;
      _LONG();
      x[0] = 7;  
    }
  }
  
  
  for(int i = 1; i <= SCORE; i++) {
    if(x[0]==x[i] && y[0]==y[i]) {
       _RESET();
       start = 0;
    }
  }
    for(int i = 0; i < 40; i++)
    _PRINT();
}

void _PRINT() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if(MATRIX[i][j]==1)
        P1OUT |= DATA;
      else
        P1OUT &= ~DATA;
      _CLK();
    }
    for(int j = 0; j < 8; j++) {
      if((quethang[i]>>j)&0x01==0x01)
        P1OUT |= DATA;
      else
        P1OUT &= ~DATA;
      _CLK();
    }
    _CHOT();
  }
}

void _CLK() {
  P1OUT |= CLK;
  P1OUT &= ~CLK;
}

void _CHOT() {
  P1OUT |= CHOT;
  P1OUT &= ~CHOT;
}

void _LONG() {
  for(int i = SCORE; i > 0; i--) {
     x[i] = x[i-1];
     y[i] = y[i-1];
  }
}

void _RESET() {
  for(int i = 0; i < 8; i++) {
    for(int j = 0; j < 8; j++) {
      if(i==0 && j==0)
        MATRIX[i][j] = 1;
      else
        MATRIX[i][j] = 0;
    }
  }
  for(int i = 0; i < 64; i++) {
    x[i] = 0;
    y[i] = 0;
  }
  
  mode = 3;
  start = 0;
  
  SCORE = 0;
  FIRST = 1;
}

#pragma vector = PORT1_VECTOR
__interrupt void _MOVE(void) {
  
  if(P1IFG&UP){
    if(mode==2 && SCORE!=0) {
      _RESET();
      P1IFG &= ~UP;
      return;
    }
    mode = 1;
    P1IFG &= ~UP;
  }

  if(P1IFG&DOWN){
    if(mode==1 && SCORE!=0) {
      _RESET();
      P1IFG &= ~DOWN;
      return;
    }
    mode = 2;
    P1IFG &= ~DOWN;
  }
  
  if(P1IFG&RIGHT){
    if(mode==4 && SCORE!=0) {
      _RESET();
      P1IFG &= ~RIGHT;
      return;
    }
    mode = 3;
    P1IFG &= ~RIGHT;
  }
  
  if(P1IFG&LEFT){
    if(mode==3 && SCORE!=0) {
      _RESET();
      P1IFG &= ~LEFT;
      return;
    }
    mode = 4;
    P1IFG &= ~LEFT;
  }
  
  if(P1IFG&START) {
    _RESET();
    start = 1;
    P1IFG &= ~START;
  }
}
