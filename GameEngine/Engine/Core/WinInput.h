#pragma once
#include <windows.h>
#include <unordered_set>
#include "Keys.h"
#include "../DirectXSDK/Delegates.h"

class EngineCore;

class WinInput
{
public:
    WinInput(EngineCore* _engineCore);
    ~WinInput();


    void AddPressedKey(Keys key);
    void RemovePressedKey(Keys key);
    bool IsKeyDown(Keys key);

    void OnMouseMove(int x, int y);

    //Доделать 

    bool ProcessMessages();

    void GetInput();


    POINT GetMousePos();
    POINT GetRawOffset();

    MulticastDelegate<const Keys> WheelMove;
    MulticastDelegate<POINT> RawOffsetEvent;

private:

    EngineCore* engineCore;
    HWND windowHWND;
    std::unordered_set<Keys>* keys;
    POINT mousePos;
    POINT prevPos;
    POINT rawOffset;
    bool rawInputInitialized = false;


};