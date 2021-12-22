#pragma once
#include "interfaces/engine/SceneBase.hpp"

#include <unordered_map>
#include <string_view>
#include <memory>

namespace orbit
{

    // The scene manager keeps track of all the scenes you have created.
    // It will communicate with the engine to render the correct active scene. And to
    // enter and leave them accordingly.
    class SceneManager
    {
    private:
        // @member: ID of the currently active scene
        std::string_view m_currentSceneId;
        // @member: Pointer to the current scene
        std::shared_ptr<ISceneBase> m_currentScene;
        // @member: Hashmap of all registered scene
        std::unordered_map<std::string, std::shared_ptr<ISceneBase>> m_scenes;
    public:
        // @method: Enter a specific scene (for example when changing regions)
        void EnterScene(const std::string& id);
        // @method: Register a new scene to the manager
        void RegisterScene(const std::string& id, std::shared_ptr<ISceneBase> scene);

        // @return: The currently active scene
        std::shared_ptr<ISceneBase> GetCurrentScene() const { return m_currentScene; }
    };

}