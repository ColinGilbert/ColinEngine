#ifndef CALLBACKS_H
#define CALLBACKS_H

float WindowWidth, WindowHeight;
void ExitApp();
void StartRender();
void StopRender();
void Destroy();
void DrawFrame();
float Time();
void KeyEvent( int code, int state );
void MouseEvent( int btn, float x, float y, short event_type);
// void GenerateTicks();

#endif
