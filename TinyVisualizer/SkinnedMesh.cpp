#include "SkinnedMesh.h"
#include "MeshShape.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate|aiProcess_GenNormals|aiProcess_JoinIdenticalVertices)

namespace DRAWER {
Eigen::Matrix<GLfloat,4,4> toEigen(const aiMatrix4x4& m) {
  Eigen::Matrix<GLfloat,4,4> ret;
  ret(0,0)=m.a1;
  ret(0,1)=m.a2;
  ret(0,2)=m.a3;
  ret(0,3)=m.a4;

  ret(1,0)=m.b1;
  ret(1,1)=m.b2;
  ret(1,2)=m.b3;
  ret(1,3)=m.b4;

  ret(2,0)=m.c1;
  ret(2,1)=m.c2;
  ret(2,2)=m.c3;
  ret(2,3)=m.c4;

  ret(3,0)=m.d1;
  ret(3,1)=m.d2;
  ret(3,2)=m.d3;
  ret(3,3)=m.d4;
  return ret;
}
std::string getDirFromFilename(const std::string& filename) {
  //extract the directory part from the file name
  std::string::size_type slashIndex;
#ifdef _WIN64
  slashIndex=filename.find_last_of("\\");
  if(slashIndex==-1)
    slashIndex=filename.find_last_of("/");
#else
  slashIndex=filename.find_last_of("/");
#endif
  std::string dir;
  if(slashIndex==std::string::npos)
    dir=".";
  else if(slashIndex==0)
    dir="/";
  else dir=filename.substr(0,slashIndex);
  return dir;
}
const char* getShortFilename(const char* filename) {
  const char* lastSlash=strrchr(filename,'/');
  if(lastSlash==nullptr)
    lastSlash=strrchr(filename,'\\');
  const char* shortFilename=lastSlash!=nullptr?lastSlash+1:filename;
  return shortFilename;
}
std::pair<const aiTexture*,int> getEmbeddedTextureAndIndex(const aiScene* scene,const char* filename) {
  if(nullptr==filename)
    return std::make_pair(nullptr,-1);
  //lookup using texture ID (if referenced like: "*1", "*2", etc.)
  if('*'==*filename) {
    int index=std::atoi(filename+1);
    if(0>index || scene->mNumTextures<=static_cast<unsigned>(index))
      return std::make_pair(nullptr,-1);
    return std::make_pair(scene->mTextures[index],index);
  }
  //lookup using filename
  const char* shortFilename=getShortFilename(filename);
  if(nullptr==shortFilename)
    return std::make_pair(nullptr,-1);
  /*for(unsigned int i=0; i<scene->mNumTextures; i++) {
    const char* shortTextureFilename=getShortFilename(scene->mTextures[i]->mFilename.C_Str());
    if(strcmp(shortTextureFilename,shortFilename)==0)
      return std::make_pair(scene->mTextures[i],i);
  }*/
  return std::make_pair(nullptr,-1);
}
std::shared_ptr<Texture> loadTexture(const aiScene* scene,const aiMaterial* mat,const char* dir) {
  if(mat->GetTextureCount(aiTextureType_DIFFUSE)>0) {
    aiString path;
    if(mat->GetTexture(aiTextureType_DIFFUSE,0,&path,NULL,NULL,NULL,NULL,NULL)==AI_SUCCESS) {
      const aiTexture* tex=getEmbeddedTextureAndIndex(scene,path.C_Str()).first;
      if(tex)
        return Texture::load(*tex);
      else {
        std::string p(path.data);
        for(unsigned int i=0; i<p.length(); i++)
          if(p[i]=='\\')
            p[i]='/';
        if(p.substr(0,2)==".\\")
          p=p.substr(2,p.size()-2);
        return Texture::load(std::string(dir)+"/"+p);
      }
    }
  }
  return NULL;
}
void loadMaterial(std::shared_ptr<MeshShape> mesh,const aiScene* scene,const aiMaterial* mat,const char* dir) {
  aiColor3D ambientColor(0.0f,0.0f,0.0f);
  if(mat->Get(AI_MATKEY_COLOR_AMBIENT,ambientColor)==AI_SUCCESS)
    mesh->setColorAmbient(GL_TRIANGLES,ambientColor.r,ambientColor.g,ambientColor.b);
  else mesh->setColorAmbient(GL_TRIANGLES,1,1,1);

  aiColor3D diffuseColor(0.0f,0.0f,0.0f);
  if(mat->Get(AI_MATKEY_COLOR_AMBIENT,diffuseColor)==AI_SUCCESS)
    mesh->setColor(GL_TRIANGLES,diffuseColor.r,diffuseColor.g,diffuseColor.b);
  else mesh->setColor(GL_TRIANGLES,1,1,1);

  aiColor3D specularColor(0.0f,0.0f,0.0f);
  if(mat->Get(AI_MATKEY_COLOR_SPECULAR,specularColor)==AI_SUCCESS)
    mesh->setColorSpecular(GL_TRIANGLES,specularColor.r,specularColor.g,specularColor.b);
  else mesh->setColorSpecular(GL_TRIANGLES,0,0,0);

  //static int id=0;
  std::shared_ptr<Texture> tex=loadTexture(scene,mat,dir);
  //tex->save(std::to_string(id++)+".png");
  mesh->setTexture(tex);
}
SkinnedMeshShape::SkinnedMeshShape(const std::string& filename) {
  Assimp::Importer importer;
  const aiScene* scene=importer.ReadFile(filename.c_str(),ASSIMP_LOAD_FLAGS);
  aiMatrix4x4 inverseTrans=scene->mRootNode->mTransformation;
  inverseTrans=inverseTrans.Inverse();
  setLocalTransform(toEigen(inverseTrans));
  //init mesh
  const aiVector3D zero3D(0.0f,0.0f,0.0f);
  for(unsigned int mid=0; mid<scene->mNumMeshes; mid++) {
    std::shared_ptr<MeshShape> meshShape(new MeshShape);
    const aiMesh* mesh=scene->mMeshes[mid];
    //vertex
    for(unsigned int i=0; i<mesh->mNumVertices; i++) {
      const aiVector3D& pPos=mesh->mVertices[i];
      const aiVector3D& pTexCoord=mesh->HasTextureCoords(0)?mesh->mTextureCoords[0][i]:zero3D;
      Eigen::Matrix<GLfloat,-1,1> tc=Eigen::Matrix<GLfloat,2,1>(pTexCoord[0],pTexCoord[1]);
      meshShape->addVertex(Eigen::Matrix<GLfloat,3,1>(pPos.x,pPos.y,pPos.z),&tc);
      if(mesh->mNormals) {
        const aiVector3D& pNormal=mesh->mNormals[i];
        meshShape->setNormal(i,Eigen::Matrix<GLfloat,3,1>(pNormal.x,pNormal.y,pNormal.z));
      } else meshShape->setNormal(i,Eigen::Matrix<GLfloat,3,1>(0,1,0));
    }
    //index
    for(unsigned int i=0; i<mesh->mNumFaces; i++) {
      const aiFace& face=mesh->mFaces[i];
      meshShape->addIndex(Eigen::Matrix<GLuint,3,1>(face.mIndices[0],face.mIndices[1],face.mIndices[2]));
    }
    meshShape->setMode(GL_TRIANGLES);
    //bones
    std::vector<std::vector<GLint>> boneId(mesh->mNumVertices);
    std::vector<std::vector<GLfloat>> boneWeight(mesh->mNumVertices);
    for(unsigned int bid=0; bid<mesh->mNumBones; bid++) {
      const aiBone* bone=mesh->mBones[bid];
      for(unsigned int i=0; i<bone->mNumWeights; i++) {
        const aiVertexWeight& w=bone->mWeights[i];
        boneId[w.mVertexId].push_back(bid);
        boneWeight[w.mVertexId].push_back(w.mWeight);
      }
    }
    //material
    loadMaterial(meshShape,scene,scene->mMaterials[mesh->mMaterialIndex],getDirFromFilename(filename).c_str());
    addShape(meshShape);
  }
}
}
