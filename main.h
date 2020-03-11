
#ifndef main.h
#define main.h


#define DISPBSY 13
#define DISPDWR 15

const int databus[8] = {14,27,26,25,5,17,16,4};
static const unsigned long Cursor_interval = 300;
unsigned long lastRefreshTime = 0;

// Globals
int mode=0;
int data=0;
int position=0;
int dimlevel=0;
int cursormode=0;
char  character[40];
bool toggle=0;
bool State=0;
bool LastState=0;

void NewDataInterrupt();
void ParseData(int data);
void ReadBus();
void UpdateDisplay();
void DrawCursor();

#endif /* main.h */
