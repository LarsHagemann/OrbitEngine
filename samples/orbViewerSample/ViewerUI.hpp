#pragma once
#include "implementation/engine/GameObject.hpp"

#include "3DView.hpp"
#include "FileParser.hpp"

class ViewerUI : public orbit::GameObject
{
private:
    FileParser m_parser;
    std::string m_loaded;
private:
    void OpenFileDialogue();
    void LoadGeometry(const std::string& id);
    void LoadMaterial(const std::string& id);
    void LoadTexture(const std::string& id);
public:
    bool LoadImpl(std::ifstream*) override { return true; }
	void UnloadImpl() override {}

    virtual void Init() override;
    virtual void Update(const orbit::Time& dt) override;
};
