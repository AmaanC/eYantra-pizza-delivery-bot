#ifndef TIMER_H_
#define TIMER_H_

void Timer5Init();
void Timer3Init();
void UpdateDisplay();
void VictoryTime();
void FreezeDisplay();
char RegisterCallback(void (*fn)(), short int delay);
short int GetCurrentTime();
void ResetTime();
void InitTimer();

int GetCallbackTime();
#endif