#include "interfaces/engine/SceneBase.hpp"
#include "implementation/engine/Engine.hpp"
#include "implementation/backends/impl/ConstantBufferImpl.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbit
{

    bool ISceneBase::Load()
    {
        m_sceneBuffer = std::make_shared<ConstantBuffer<Matrix4f, Matrix4f, SceneShaderInfo, Light[100]>>();
        m_loaded = true;
        for (auto object : m_objectsVector)
            m_loaded |= object->Load();
        ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Scene loaded");
        return m_loaded;
    }

    void ISceneBase::Unload()
    {
        m_sceneBuffer = nullptr;
        for (auto object : m_objectsVector)
            object->Unload();
        ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Scene unloaded");
        m_loaded = false;
    }

    void ISceneBase::Update(const Time& dt)
    {
        std::vector<std::future<void>> results{};
        results.reserve(ENGINE->GetParallelRenderCount());

        auto c = m_camera->GetTransform()->GetCombinedTranslation();

        *m_sceneBuffer->GetPointerToObject<0>() = m_camera->GetViewMatrix();
        *m_sceneBuffer->GetPointerToObject<1>() = m_camera->GetProjectionMatrix();
         m_sceneBuffer->GetPointerToObject<2>()->numLights = m_numLights;
         m_sceneBuffer->GetPointerToObject<2>()->ambientLight = { .3f, .3f, .3f, 1.f };
         m_sceneBuffer->GetPointerToObject<2>()->gameTime = 0.f;
         m_sceneBuffer->GetPointerToObject<2>()->cameraPosition = Vector4f{
            c.x(),
            c.y(),
            c.z(),
            1.f
        };
        m_sceneBuffer->UpdateBuffer();
        m_sceneBuffer->BindBuffer(0, { BindShaderType::PixelShader, BindShaderType::VertexShader });

        const auto workPerThread = std::max(static_cast<size_t>(1u), m_objectsVector.size() / ENGINE->GetParallelRenderCount());
		for (auto i = 0U; i < ENGINE->GetParallelRenderCount(); ++i)
		{
			if (i * workPerThread >= m_objectsVector.size())
				break;
			
			auto beginIdx = i * workPerThread;
			auto endIdx =
				(i + 1) * workPerThread < m_objectsVector.size()
				? (i + 1) * workPerThread
				: m_objectsVector.size();

			auto begin = m_objectsVector.begin() + beginIdx;
			auto end = m_objectsVector.begin() + endIdx;

			//results.emplace_back(std::async([&](std::vector<GObjectPtr>::iterator begin, std::vector<GObjectPtr>::iterator end) {
                std::for_each(begin, end, [&](GObjectPtr object) {
                    if (object->IsLoaded() && object->IsActive())
                    {
                        object->Draw();
                        object->Update(dt);
                        object->PhysicsUpdate(dt.asMilliseconds());
                    }
                });
            //}, begin, end));
		}

		for (auto i = 0U; i < results.size(); ++i)
			results.at(i).wait();
    }

    bool ISceneBase::AddObject(const std::string& identifier, GObjectPtr object)
    {
        object->SetIdentifier(identifier);
        auto it = m_objectsMap.find(identifier);
        if (it != m_objectsMap.end())
        {
            ORBIT_ERROR("Object with identifier '%s' does already exist!", identifier.c_str());
            return false;
        }

        m_objectsVector.emplace_back(object);
        m_objectsMap.emplace(identifier, object);
        object->Init();
        return true;
    }

    GObjectPtr ISceneBase::FindObject(const std::string& identifier)
    {
        auto it = m_objectsMap.find(identifier);
        if (it == m_objectsMap.end())
            return nullptr;
        
        return it->second;
    }

    GObjectPtr ISceneBase::RemoveObject(const std::string& identifier)
    {
        auto it = m_objectsMap.find(identifier);
        if (it == m_objectsMap.end())
            return nullptr;
        
        m_objectsMap.erase(it);
        return it->second;
    }

    void ISceneBase::RemoveLight(Light* light)
    {
        auto index = (*m_sceneBuffer->GetPointerToObject<3>()) - light;
        if (index == m_numLights)
            return;
        
        (*m_sceneBuffer->GetPointerToObject<3>())[index]._enabled = false;
        ++m_numDisabledLights;
    }

    Light* ISceneBase::AddLight(LightPtr light) 
    { 
        light->_enabled = true;
        if (m_numDisabledLights > 0u)
        {
            for (auto i = 0u; i < m_numLights; ++i)
            {
                if (!(*m_sceneBuffer->GetPointerToObject<3>())[i]._enabled)
                {
                    (*m_sceneBuffer->GetPointerToObject<3>())[i] = *light;
                    --m_numDisabledLights;
                    return &(*m_sceneBuffer->GetPointerToObject<3>())[i];
                }
            }
        }
        assert(m_numLights < 100 && "You cannot allocate more than 100 lights at the moment!");
        (*m_sceneBuffer->GetPointerToObject<3>())[m_numLights] = *light;
        return &(*m_sceneBuffer->GetPointerToObject<3>())[m_numLights++];
    }

}