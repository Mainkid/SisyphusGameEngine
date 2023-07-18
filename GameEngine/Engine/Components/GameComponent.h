#pragma once
#include <memory>

class GameComponent
{
public:
    GameComponent() {};
    virtual void Render() = 0;
    virtual void Update(float deltaSec) = 0;
    virtual void Initialize() = 0;

protected:
    //std::shared_ptr<Game> game;

};

//TODO: Разобраться с наследованием GameComponent
