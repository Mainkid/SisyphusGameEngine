#pragma once
#include <memory>

class GameComponent
{
    friend class PropertiesWidget;
public:
    GameComponent() = default;
    

protected:
    virtual void Render() = 0;

};

//TODO: Разобраться с наследованием GameComponent
