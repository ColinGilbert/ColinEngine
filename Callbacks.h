#ifndef CALLBACKS_H
#define CALLBACKS_H

extern "C"
{
	float WindowWidth, WindowHeight;
	void ExitApp();
	void OnStart();
	void OnStop();
	void OnDestroy();
	void OnDrawFrame();
	void OnTimer( float delta );
	void OnKey( int code, bool state );
	void OnMouseDown( int btn, float x, float y);
	void OnMouseMove( float x, float y );
	void OnMouseUp( int btn, i float x, float y );
//	void GenerateTicks();
}

#endif
