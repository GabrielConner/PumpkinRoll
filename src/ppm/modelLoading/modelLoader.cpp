#include "pPack/modelLoader.h"
#include "pPack/model.h"
#include "pPack/vector.h"

#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>

#include "stb_image.h"

using namespace ::pPack;



namespace {

ModelObject* LoadNode(ModelScene* ModelScene, const aiNode* node);
Mesh* LoadMesh(aiMesh* aMesh);
Material* LoadMaterial(const std::string& location, const aiScene* ModelScene, aiMaterial* aMaterial);
ModelScene* LoadScene(const std::string& location, const aiScene* aModelScene);

}; // namespace



namespace pPack {


ModelScene* LoadModel(std::string location) {
  Assimp::Importer importer;
  const aiScene* aModelScene = importer.ReadFile(location, aiProcess_Triangulate | aiProcess_SortByPType | aiProcess_GenNormals);
  if (!aModelScene || aModelScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    return nullptr;

  ModelScene* ModelScene = LoadScene(location, aModelScene);

  return ModelScene;
}

} // namespace pPack







namespace {

ModelObject* LoadNode(ModelScene* ModelScene, const aiNode* node) {
  ModelObject* obj = new ModelObject();

  if (node->mNumChildren > 0)
    obj->children = new ModelObject*[node->mNumChildren];
  if (node->mNumMeshes > 0)
    obj->meshIndices = new uint32_t[node->mNumMeshes];

  obj->childrenCount = node->mNumChildren;
  obj->meshCount = node->mNumMeshes;


  for (uint32_t x = 0; x < 4; x++)
    for (uint32_t y = 0; y < 4; y++)
      obj->relativeTransformation[x][y] = node->mTransformation[x][y];

  for (uint32_t i = 0; i < node->mNumMeshes; i++)
    obj->meshIndices[i] = node->mMeshes[i];

  for (uint32_t i = 0; i < node->mNumChildren; i++)
    obj->children[i] = LoadNode(ModelScene, node->mChildren[i]);

  return obj;
}



Mesh* LoadMesh(aiMesh* aMesh) {
  if (!(aMesh->mPrimitiveTypes | aiPrimitiveType::aiPrimitiveType_TRIANGLE) || aMesh->mNumFaces == 0 || aMesh->mNumVertices == 0)
    return nullptr;

  Vertex* vertexArray = new Vertex[aMesh->mNumVertices];

  for (uint32_t i = 0; i < aMesh->mNumVertices; i++) {
    vertexArray[i].position = Vector3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);
    vertexArray[i].normal = Vector3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);

    if (aMesh->mColors[0] != nullptr)
      vertexArray[i].color = Vector4(aMesh->mColors[0][i].r, aMesh->mColors[0][i].g, aMesh->mColors[0][i].b, aMesh->mColors[0][i].a);
    else
      vertexArray[i].color = Vector4();

    if (aMesh->mTextureCoords[0] != nullptr)
      vertexArray[i].uv = Vector2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);
    else
      vertexArray[i].uv = Vector2();
  }

  Mesh* mesh = new Mesh();

  mesh->vertexCount = aMesh->mNumFaces * 3;
  mesh->vertices = new Vertex[mesh->vertexCount];
  mesh->materialIndex = aMesh->mMaterialIndex;

  for (uint32_t i = 0, j = 0; i < aMesh->mNumFaces; i++, j += 3) {
    mesh->vertices[j] = vertexArray[aMesh->mFaces[i].mIndices[0]];
    mesh->vertices[j + 1] = vertexArray[aMesh->mFaces[i].mIndices[1]];
    mesh->vertices[j + 2] = vertexArray[aMesh->mFaces[i].mIndices[2]];
  }

  delete[](vertexArray);
  return mesh;
}



Material* LoadMaterial(const std::string& location, const aiScene* ModelScene, aiMaterial* aMaterial) {
  Material* material = new Material();
  uint8_t diffuseCount = aMaterial->GetTextureCount(aiTextureType_DIFFUSE) ? 1 : 0;
  uint8_t metallicCount = aMaterial->GetTextureCount(aiTextureType_METALNESS) ? 1 : 0;
  uint8_t roughnessCount = aMaterial->GetTextureCount(aiTextureType_SHININESS) ? 1 : 0;
  uint8_t total = diffuseCount + metallicCount + diffuseCount;

  if (total > 0)
    material->textures = new UnknownTexture[total];
  else
    material->textures = nullptr;
  material->textureCount = total;

  aiString str;
  aiColor4D color;

  int texIndex = 0;

  if (diffuseCount) {
    aMaterial->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), str);
    std::pair<const aiTexture*, int> info = ModelScene->GetEmbeddedTextureAndIndex(str.C_Str());
    material->textures[texIndex].type = TextureType::ALBEDO;

    if (info.first) {
      material->textures[texIndex].location = TextureLocation::INTERNAL;
      material->textures[texIndex].textureIndex = info.second;
    } else {
      Texture* tex = new Texture();

      material->textures[texIndex].location = TextureLocation::EXTERNAL;
      material->textures[texIndex].texture = tex;

      int width = 0, height = 0, channels = 0;
      stbi_uc* data = stbi_load((location + str.C_Str()).c_str(), &width, &height, &channels, 0);
      if (data) {
        tex->width = width;
        tex->height = height;
        tex->channels = channels;

        memcpy(tex->data, data, (size_t)width * height * channels);

        stbi_image_free(data);
      }
    }

    texIndex++;
  } else {
    aiGetMaterialColor(aMaterial, AI_MATKEY_COLOR_DIFFUSE, &color);
    material->albedo = Vector4(color.r, color.g, color.b, color.a);
  }


  if (metallicCount) {
    aMaterial->Get(AI_MATKEY_TEXTURE_REFLECTION(0), str);
    std::pair<const aiTexture*, int> info = ModelScene->GetEmbeddedTextureAndIndex(str.C_Str());
    material->textures[texIndex].type = TextureType::METALLIC;

    if (info.first) {
      material->textures[texIndex].location = TextureLocation::INTERNAL;
      material->textures[texIndex].textureIndex = info.second;
    } else {
      Texture* tex = new Texture();

      material->textures[texIndex].location = TextureLocation::EXTERNAL;
      material->textures[texIndex].texture = tex;

      int width = 0, height = 0, channels = 0;
      stbi_uc* data = stbi_load(str.C_Str(), &width, &height, &channels, 0);
      if (data) {
        tex->width = width;
        tex->height = height;
        tex->channels = channels;

        memcpy(tex->data, data, (size_t)width * height * channels);

        stbi_image_free(data);
      }
    }

    texIndex++;
  } else
    aiGetMaterialFloat(aMaterial, AI_MATKEY_REFLECTIVITY, &material->metallic);


  if (roughnessCount) {
    aMaterial->Get(AI_MATKEY_TEXTURE_SHININESS(0), str);
    std::pair<const aiTexture*, int> info = ModelScene->GetEmbeddedTextureAndIndex(str.C_Str());
    material->textures[texIndex].type = TextureType::ROUGHNESS;

    if (info.first) {
      material->textures[texIndex].location = TextureLocation::INTERNAL;
      material->textures[texIndex].textureIndex = info.second;
    } else {
      Texture* tex = new Texture();

      material->textures[texIndex].location = TextureLocation::EXTERNAL;
      material->textures[texIndex].texture = tex;

      int width = 0, height = 0, channels = 0;
      stbi_uc* data = stbi_load(str.C_Str(), &width, &height, &channels, 0);
      if (data) {
        tex->width = width;
        tex->height = height;
        tex->channels = channels;

        memcpy(tex->data, data, (size_t)width * height * channels);

        stbi_image_free(data);
      }
    }
  } else
    aiGetMaterialFloat(aMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &material->metallic);

  return material;
}



ModelScene* LoadScene(const std::string& location, const aiScene* aModelScene) {
  ModelScene* scene = new ModelScene();
  if (aModelScene->mNumMeshes > 0)
    scene->meshes = new Mesh*[aModelScene->mNumMeshes] { 0 };
  if (aModelScene->mNumMaterials > 0)
    scene->materials = new Material*[aModelScene->mNumMaterials] { 0 };
  if (aModelScene->mNumTextures > 0)
    scene->textures = new Texture*[aModelScene->mNumTextures] { 0 };


  scene->meshCount = aModelScene->mNumMeshes;
  scene->materialCount = aModelScene->mNumMeshes;
  scene->textureCount = aModelScene->mNumTextures;

  for (uint32_t i = 0; i < aModelScene->mNumMeshes; i++)
    scene->meshes[i] = LoadMesh(aModelScene->mMeshes[i]);

  for (uint32_t i = 0; i < aModelScene->mNumMaterials; i++)
    scene->materials[i] = LoadMaterial(location, aModelScene, aModelScene->mMaterials[i]);


  for (uint32_t i = 0; i < aModelScene->mNumTextures; i++) {
    Texture* tex = new Texture();
    scene->textures[i] = tex;

    tex->width = aModelScene->mTextures[i]->mWidth;
    tex->height = aModelScene->mTextures[i]->mHeight;
    tex->channels = 4;
    memcpy(tex->data, aModelScene->mTextures[i]->pcData, (tex->height ? tex->height : 1) * tex->width * tex->channels);
  }

  scene->rootModelObject = LoadNode(scene, aModelScene->mRootNode);
  return scene;
}

}; // namespace