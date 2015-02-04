#ifndef CALLBACKS_H
#define CALLBACKS_H

extern float WindowWidth, WindowHeight;
extern void ExitApp();
extern void OnStart();
extern void OnStop();
extern void OnDestroy();
extern void OnDrawFrame();
extern void OnTimer( float delta );
extern void OnKey( int code, bool state );
extern void OnMouseDown( int btn, float x, float y);
extern void OnMouseMove( float x, float y );
extern void OnMouseUp( int btn, i float x, float y );
//extern void GenerateTicks();

#endif
