#include "interfaces/engine/SceneBase.hpp"

class SpaceScene : public orbit::AsyncScene
{
private:

public:
    SpaceScene();
    virtual bool Load() override;
};
