#include "interfaces/engine/EngineBase.hpp"
#include <thread>
#include <chrono>

namespace orbit
{

    void IEngineBase::Update()
    {
        auto scene = GetCurrentScene();
        if (scene != nullptr && scene->IsLoaded())
            scene->Update(m_lastFrametime);
        UpdatePhysX();
#ifdef _DEBUG
        RMDrawDebug();
#endif
    }

    void IEngineBase::Run()
    {
        while (IsRunning())
        {
            Clear();
            Update();
            Display();

            m_lastFrametime = m_frameClock.Restart();
        }
    }

}