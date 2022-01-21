#pragma once
#include "interfaces/engine/SceneBase.hpp"

class ViewerScene : public orbit::AsyncScene
{
private:

public:
    ViewerScene();
    virtual bool Load() override;
};
