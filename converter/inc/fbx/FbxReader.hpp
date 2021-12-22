#pragma once
#include "orb/OrbIntermediate.hpp"
#include "Reader.hpp"
#include "FbxTypes.hpp"

#include <filesystem>

namespace orbtool
{

    namespace fs = std::filesystem;

    class FbxReader : public Reader
    {
    private:
        bool m_warnOnQuads = true;
        FBXInterType m_fbx;
    private:
        static Quaterniond QuatFromEuler(Vector3d euler);

        std::unordered_map<int64_t, std::shared_ptr<FBXInterModel>>::iterator TryInsert(std::shared_ptr<FBXModel> model);

        void FBXGeometryAppendToMesh(const FBXGeometry* geometry, OrbMesh* mesh);
        void LoadPositions(const FBXGeometry* geometry, OrbMesh* mesh);
        void LoadNormals(const FBXGeometry* geometry, OrbMesh* mesh);
        void LoadTangents(const FBXGeometry* geometry, OrbMesh* mesh);
        void LoadUVs(const FBXGeometry* geometry, OrbMesh* mesh);
        void CleanupGeometry(OrbMesh* mesh);

        static MappingInformationType LoadMIT(const FBXNode* mitNode);
        static ReferenceInformationType LoadRIT(const FBXNode* ritNode);
        static void LoadFBXData(const FBXNode* root, FBXData* data);
        static void LoadGeometries(const FBXNode* objectsNode, FBXData* data);
        static void LoadMaterials(const FBXNode* objectsNode, FBXData* data);
        static void LoadModels(const FBXNode* objectsNode, FBXData* data);
        static void LoadConnections(const FBXNode* connectionsNode, FBXData* data);
        static void LoadTextures(const FBXNode* objectsNode, FBXData* data);
        static void LoadAttributes(const FBXNode* objectsNode, FBXData* data);
        static void GetFBXGeometry(FBXGeometry* geometry, const FBXNode* geometryNode);
        static void GetFBXMaterial(FBXMaterial* material, const FBXNode* materialNode);
        void FBXToIntermediate(const FBXData* data);

        void ConnectModelModel(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXModel> m1);
        void ConnectModelAttribute(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXAttribute> attr);
        void ConnectModelGeometry(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXGeometry> geom);
        void ConnectModelMaterial(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXMaterial> mat);
        static void ConnectMaterialTexture(std::shared_ptr<FBXMaterial> mat, std::shared_ptr<FBXTexture> tex, const std::string& channel);
    public:
        bool ReadFile(const fs::path& filepath, OrbIntermediate* intermediate) override;
        void WarnOnQuads(bool warn = true) { m_warnOnQuads = warn; }
    };

}