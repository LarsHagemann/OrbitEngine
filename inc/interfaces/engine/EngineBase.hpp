#pragma once
#include "implementation/engine/Allocator.hpp"
#include "implementation/engine/SceneManager.hpp"
#include "implementation/engine/ResourceManager.hpp"
#include "implementation/engine/PhysxEngine.hpp"
#include "implementation/misc/Time.hpp"

#include "interfaces/rendering/Renderer.hpp"

#include <string>
#include <random>

namespace orbit
{

    class IEngineBase : public ResourceManager, public SceneManager, public Allocator, public PhysxEngine
    {
    private:
        Clock m_frameClock;
        Time m_lastFrametime;
        bool m_vsyncEnabled = false;
        std::mt19937 m_randomEngine;
    protected:
        uint32_t m_numThreads = 0;
        std::shared_ptr<IRenderer> m_renderer;
    protected:
        virtual void Clear() = 0;
        virtual void Display() = 0;
        virtual bool IsRunning() = 0;
        virtual void OnResize() = 0;
    public: 
        uint32_t GetParallelRenderCount() const { return m_numThreads; }
        void Update();
        void Run();
        void EnableVSync(bool enable = true) { m_vsyncEnabled = enable; }
        bool IsVsynced() const { return m_vsyncEnabled; }

        std::shared_ptr<IRenderer> Renderer() const { return m_renderer; }
        template<class Type, class Distribution>
        Type NextRandomValue(const Distribution& dist)
        {
            return dist(m_randomEngine);
        }
    };

}