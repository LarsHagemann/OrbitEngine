#include "implementation/engine/SceneManager.hpp"

namespace orbit
{

    void SceneManager::EnterScene(const std::string& id)
    {
        if (m_currentSceneId == id)
            return; // Nothing to do
        
        auto it = m_scenes.find(id);
        if (it != m_scenes.end())
        {
            m_currentSceneId = id;
            if (m_currentScene != nullptr)
                m_currentScene->OnLeave();
            
            m_currentScene = it->second;
            m_currentScene->OnEnter();
        }
    }

    void SceneManager::RegisterScene(const std::string& id, std::shared_ptr<ISceneBase> scene)
    {
        m_scenes.emplace(id, scene);
    }

}
