#pragma once
#include "interfaces/misc/UnLoadable.hpp"
#include "implementation/engine/GameObject.hpp"
#include "implementation/rendering/Light.hpp"
#include "interfaces/rendering/Camera.hpp"
#include "interfaces/misc/ConstantBuffer.hpp"

#include <future>
#include <vector>
#include <algorithm>

namespace orbit
{

    struct SceneShaderInfo
    {
        Vector4f ambientLight;
        Vector4f cameraPosition;
        uint32_t numLights;
        float    gameTime;
    };

    // Interface for scenes in the game engine
    // A scene is an area in the game, that the player can interact
    // with. All entities are registered in the scene
    class ISceneBase
    {
    private:
        std::vector<std::shared_ptr<GameObject>>                                   m_objectsVector;
        std::unordered_map<std::string, std::shared_ptr<GameObject>>               m_objectsMap;
        SPtr<IConstantBufferBase<Matrix4f, Matrix4f, SceneShaderInfo, Light[100]>> m_sceneBuffer;
        CameraPtr                                                                  m_camera;
        bool                                                                       m_loaded = false;
        uint32_t                                                                   m_numLights = 0u;
        uint32_t                                                                   m_numDisabledLights = 0u;
    public:
        virtual bool Load();
        virtual void Unload();
        // @method: Called whenever this scene is entered
        virtual void OnEnter() = 0;
        // @method: Called whenever this scene is left
        virtual void OnLeave() = 0;
        // @method: Updates all the objects in the scene
        virtual void Update(const Time& dt);
        // @method: Adds an object to the scene
        bool AddObject(const std::string& identifier, GObjectPtr object);
        // @method: Tries to find an object by its identifier.
        // @return: nullptr if the object could not be found
        GObjectPtr FindObject(const std::string& identifier);
        // @method: Removes an object from the scene
        // @return: The removed Game Object
        GObjectPtr RemoveObject(const std::string& identifier);
        // @method: Sets the scene's camera
        void SetCamera(CameraPtr camera) { m_camera = camera; }
        // @return: Returns the scene's camera
        CameraPtr GetCamera() const { return m_camera; }
        // @return: True when the scene is loaded
        bool IsLoaded() const { return m_loaded; }
        // @brief: Adds a new light to the scene
        Light* AddLight(LightPtr light);
        // @brief: Removes a light from the scene
        void RemoveLight(Light* ptr);
    };

    // A Scene that doesn't unload its resources when left. This might
    // be useful when there are small scenes that are entered and left a lot
    // to keep loading times managable.
    class KeepAliveScene : public ISceneBase
    {
    public:
        // @see class SceneBase
        virtual void OnEnter() override { if (!IsLoaded()) Load(); }
        // @see class SceneBase
        virtual void OnLeave() override {}
    };

    // A scene that automatically loads its resources when entered
    // and unloads them when left. This is useful for larger scenes that
    // you want to be unloaded and loaded automatically.
    class UnloadScene : public ISceneBase
    {
    public:
        // @see class SceneBase
        virtual void OnEnter() override { Load(); }
        // @see class SceneBase
        virtual void OnLeave() override { Unload(); }
    };

    // Loads a scene automatically and asynchronously and unloads
    // it when left. 
    class AsyncScene : public ISceneBase
    {
    private:
        std::vector<std::future<bool>> m_dummy0;
        std::vector<std::future<void>> m_dummy1;
    public:
        // @see class SceneBase
        virtual void OnEnter() override 
        { 
            m_dummy0.emplace_back(std::async(&AsyncScene::Load, this));
            m_dummy0.erase(
                std::remove_if(m_dummy0.begin(), m_dummy0.end(), [](const auto& it) { return it.valid(); }), 
                m_dummy0.end()
            );
        }
        // @see class SceneBase
        virtual void OnLeave() override 
        { 
            m_dummy1.emplace_back(std::async(&AsyncScene::Unload, this));
            m_dummy1.erase(
                std::remove_if(m_dummy1.begin(), m_dummy1.end(), [](const auto& it) { return it.valid(); }),
                m_dummy1.end()
            );
        }
    };

}