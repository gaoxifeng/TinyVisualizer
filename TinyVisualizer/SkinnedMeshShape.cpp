#include "SkinnedMeshShape.h"
#include "MeshShape.h"
#include <assimp/postprocess.h>
#include <assimp/Exporter.hpp>
#include <iostream>

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate|aiProcess_GenNormals|aiProcess_JoinIdenticalVertices)

namespace DRAWER {
//assimp helper
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
Eigen::Matrix<GLfloat,3,3> toEigen(const aiMatrix3x3& m) {
  Eigen::Matrix<GLfloat,3,3> ret;
  ret(0,0)=m.a1;
  ret(0,1)=m.a2;
  ret(0,2)=m.a3;

  ret(1,0)=m.b1;
  ret(1,1)=m.b2;
  ret(1,2)=m.b3;

  ret(2,0)=m.c1;
  ret(2,1)=m.c2;
  ret(2,2)=m.c3;
  return ret;
}
Eigen::Matrix<GLfloat,3,3> toEigen(const aiQuaternion& q) {
  return toEigen(q.GetMatrix());
}
Eigen::Matrix<GLfloat,3,1> toEigen(const aiVector3D& m) {
  Eigen::Matrix<GLfloat,3,1> ret;
  ret[0]=m.x;
  ret[1]=m.y;
  ret[2]=m.z;
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
const aiNodeAnim* findNodeAnim(const aiAnimation& animation,const std::string& NodeName) {
  for(GLuint i=0; i<animation.mNumChannels; i++) {
    const aiNodeAnim* pNodeAnim=animation.mChannels[i];
    if(std::string(pNodeAnim->mNodeName.data) == NodeName)
      return pNodeAnim;
  }
  return NULL;
}
GLuint findScaling(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) {
  ASSERT(pNodeAnim->mNumScalingKeys>0)
  for(GLuint i=0; i<pNodeAnim->mNumScalingKeys-1; i++) {
    GLfloat t=(GLfloat)pNodeAnim->mScalingKeys[i+1].mTime;
    if(animationTimeTicks<t)
      return i;
  }
  return 0;
}
GLuint findRotation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) {
  ASSERT(pNodeAnim->mNumRotationKeys>0)
  for(GLuint i=0; i<pNodeAnim->mNumRotationKeys-1; i++) {
    GLfloat t=(GLfloat)pNodeAnim->mRotationKeys[i+1].mTime;
    if(animationTimeTicks<t)
      return i;
  }
  return 0;
}
GLuint findPosition(GLfloat animationTimeTicks, const aiNodeAnim* pNodeAnim) {
  for(GLuint i=0; i<pNodeAnim->mNumPositionKeys-1; i++) {
    GLfloat t=(GLfloat)pNodeAnim->mPositionKeys[i+1].mTime;
    if(animationTimeTicks<t)
      return i;
  }
  return 0;
}
//SkinnedMeshShape
SkinnedMeshShape::SkinnedMeshShape(const std::string& filename) {
  _scene=_importer.ReadFile(filename.c_str(),ASSIMP_LOAD_FLAGS);
  aiMatrix4x4 inverseTrans=_scene->mRootNode->mTransformation;
  inverseTrans=inverseTrans.Inverse();
  setLocalTransform(toEigen(inverseTrans));
  //init mesh
  const aiVector3D zero3D(0.0f,0.0f,0.0f);
  for(unsigned int mid=0; mid<_scene->mNumMeshes; mid++) {
    std::shared_ptr<MeshShape> meshShape(new MeshShape);
    const aiMesh* mesh=_scene->mMeshes[mid];
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
    //bone
    BoneData boneData;
    boneData._maxNrBone=4;  //we only support 4 bones
    std::vector<std::unordered_map<GLint,GLfloat>> boneInfo(mesh->mNumVertices);
    for(unsigned int bid=0; bid<mesh->mNumBones; bid++) {
      const aiBone* bone=mesh->mBones[bid];
      for(unsigned int i=0; i<bone->mNumWeights; i++) {
        const aiVertexWeight& w=bone->mWeights[i];
        if(w.mWeight!=0) {
          boneInfo[w.mVertexId][getBoneId(bone)]=w.mWeight;
          boneData._maxNrBone=std::max<GLint>(boneData._maxNrBone,boneInfo[w.mVertexId].size());
        }
      }
    }
    boneData._boneId.assign(mesh->mNumVertices*boneData._maxNrBone,-1);
    boneData._boneWeight.assign(mesh->mNumVertices*boneData._maxNrBone,0);
    for(unsigned int i=0; i<mesh->mNumVertices; i++) {
      GLint off=0;
      const auto& info=boneInfo[i];
      for(const auto& item:info) {
        boneData._boneId[i*boneData._maxNrBone+off]=item.first;
        boneData._boneWeight[i*boneData._maxNrBone+off]=item.second;
        off++;
      }
    }
    //material
    loadMaterial(meshShape,_scene,_scene->mMaterials[mesh->mMaterialIndex],getDirFromFilename(filename).c_str());
    addShape(meshShape);
    //for GPU transform-feedback, we need to set refMesh to draw points
    _refMeshes.push_back(std::shared_ptr<MeshShape>(new MeshShape(*meshShape)));
    _refMeshes.back()->setBoneData(boneData);
    _refMeshes.back()->setMode(GL_POINTS);
    _refMeshes.back()->clearIndex();
    for(int i=0; i<_refMeshes.back()->nrVertex(); i++)
      _refMeshes.back()->addIndexSingle(i);
  }
}
bool SkinnedMeshShape::write(const std::string& filename) const {
  Assimp::Exporter exporter;
  aiExportFormatDesc desc;
  desc.id=NULL;
  bool found=false;
  for(int i=0; i<(int)aiGetExportFormatCount(); i++) {
    desc=*aiGetExportFormatDescription(i);
    if(filename.find(desc.fileExtension)!=std::string::npos) {
      found=true;
      break;
    }
  }
  if(!found) {
    std::cout << "Cannot find matching file formating for exporting to: " << filename << " supported formats are: " << std::endl;
    for(int i=0; i<(int)aiGetExportFormatCount(); i++)
      std::cout << aiGetExportFormatDescription(i)->fileExtension << std::endl;
  }
  exporter.Export(_scene,desc.id,filename.c_str());
  return true;
}
void SkinnedMeshShape::setAnimatedFrame(GLuint index,GLfloat time,bool updateMesh) {
  ASSERT_MSGV(index<_scene->mNumAnimations,"Invalid animation index, maxId=%d!",index)
  Eigen::Matrix<GLfloat,4,4> identity=Eigen::Matrix<GLfloat,4,4>::Identity();
  GLfloat animationTimeTicks=calcAnimationTimeTicks(time,index);
  const aiAnimation& animation=*_scene->mAnimations[index];
  readNodeHierarchy(animationTimeTicks,_scene->mRootNode,identity,animation);
  //calc vertices
  for(GLuint i=0; updateMesh && i<(GLuint)_refMeshes.size(); i++)
    updateMeshVerticesGPU(std::dynamic_pointer_cast<MeshShape>(_shapes[i]),_refMeshes[i],_refMeshes[i]->getBoneData());
}
Eigen::Matrix<GLfloat,4,-1> SkinnedMeshShape::getBoneTransforms(int reserve) const {
  Eigen::Matrix<GLfloat,4,-1> ret;
  ret.resize(4,4*std::max<GLint>(reserve,_bones.size()));
  for(int i=0; i<(int)_bones.size(); i++)
    ret.template block<4,4>(0,i*4)=_bones[i]._finalTrans;
  return ret;
}
Eigen::Matrix<GLint,4,-1> SkinnedMeshShape::getBoneId(int id) const {
  const BoneData& bd=_refMeshes[id]->getBoneData();
  return Eigen::Map<const Eigen::Matrix<GLint,-1,-1>>(bd._boneId.data(),bd._maxNrBone,bd._boneId.size()/bd._maxNrBone);
}
Eigen::Matrix<GLfloat,4,-1> SkinnedMeshShape::getBoneWeight(int id) const {
  const BoneData& bd=_refMeshes[id]->getBoneData();
  return Eigen::Map<const Eigen::Matrix<GLfloat,-1,-1>>(bd._boneWeight.data(),bd._maxNrBone,bd._boneWeight.size()/bd._maxNrBone);
}
void SkinnedMeshShape::setBoneWeight(int id,Eigen::Matrix<GLfloat,4,-1> weight) {
  auto& w=_refMeshes[id]->getBoneData()._boneWeight;
  ASSERT_MSGV((int)w.size()==(int)weight.size(),"Incorrect bone weight size: %d!=%d",(int)w.size(),(int)weight.size())
  Eigen::Map<Eigen::Matrix<GLfloat,4,-1>>(w.data(),weight.rows(),weight.cols())=weight;
  //reset data structure inside aiScene
  const aiMesh* mesh=_scene->mMeshes[id];
  for(unsigned int bid=0; bid<mesh->mNumBones; bid++) {
    const aiBone* bone=mesh->mBones[bid];
    for(unsigned int i=0; i<bone->mNumWeights; i++) {
      aiVertexWeight& w=bone->mWeights[i];
      if(w.mWeight!=0)
        w.mWeight=_refMeshes[id]->getBoneData().findWeight(w.mVertexId,getBoneId(bone));
    }
  }
}
std::shared_ptr<MeshShape> SkinnedMeshShape::getMeshRef(int id) const {
  return _refMeshes[id];
}
std::shared_ptr<MeshShape> SkinnedMeshShape::getMesh(int id) const {
  return std::dynamic_pointer_cast<MeshShape>(_shapes[id]);
}
GLfloat SkinnedMeshShape::duration(GLuint index) const {
  GLfloat duration=0.0f;
  std::modf((GLfloat)_scene->mAnimations[index]->mDuration,&duration);
  return duration;
}
GLuint SkinnedMeshShape::nrAnimation() const {
  return _scene->mNumAnimations;
}
//helper
GLuint SkinnedMeshShape::getBoneId(const aiBone* bone) {
  std::string name(bone->mName.C_Str());
  auto it=_boneNameToIndexMap.find(name);
  if(it==_boneNameToIndexMap.end()) {
    GLuint bid=(GLuint)_boneNameToIndexMap.size();
    _bones.resize(bid+1);
    _bones[bid]._offsetTrans=toEigen(bone->mOffsetMatrix);
    _boneNameToIndexMap[name]=bid;
    return bid;
  } else return it->second;
}
GLfloat SkinnedMeshShape::calcAnimationTimeTicks(GLfloat time,GLint index) const {
  GLfloat ticksPerSecond=(GLfloat)(_scene->mAnimations[index]->mTicksPerSecond!=0?_scene->mAnimations[index]->mTicksPerSecond:25.0f);
  GLfloat timeInTicks=time*ticksPerSecond;
  //we need to use the integral part of mDuration for the total length of the animation
  GLfloat duration=0.0f;
  std::modf((GLfloat)_scene->mAnimations[index]->mDuration,&duration);
  GLfloat animationTimeTicks=fmod(timeInTicks,duration);
  return animationTimeTicks;
}
Eigen::Matrix<GLfloat,3,1> SkinnedMeshShape::calcInterpolatedScaling(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const {
  //we need at least two values to interpolate...
  if(pNodeAnim->mNumScalingKeys==1)
    return toEigen(pNodeAnim->mScalingKeys[0].mValue);
  GLuint scalingIndex=findScaling(animationTimeTicks,pNodeAnim);
  GLuint nextScalingIndex=scalingIndex+1;
  ASSERT(nextScalingIndex<pNodeAnim->mNumScalingKeys);
  GLfloat t1=(GLfloat)pNodeAnim->mScalingKeys[scalingIndex].mTime;
  if(t1>animationTimeTicks)
    return toEigen(pNodeAnim->mScalingKeys[scalingIndex].mValue);
  else {
    GLfloat t2=(GLfloat)pNodeAnim->mScalingKeys[nextScalingIndex].mTime;
    GLfloat deltaTime=t2-t1;
    GLfloat factor=(animationTimeTicks-(GLfloat)t1)/deltaTime;
    ASSERT(factor>=0.0f && factor<=1.0f);
    Eigen::Matrix<GLfloat,3,1> start=toEigen(pNodeAnim->mScalingKeys[scalingIndex].mValue);
    Eigen::Matrix<GLfloat,3,1> end=toEigen(pNodeAnim->mScalingKeys[nextScalingIndex].mValue);
    Eigen::Matrix<GLfloat,3,1> delta=end-start;
    return start+factor*delta;
  }
}
Eigen::Matrix<GLfloat,3,3> SkinnedMeshShape::calcInterpolatedRotation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const {
  //we need at least two values to interpolate...
  aiQuaternion out;
  if(pNodeAnim->mNumRotationKeys==1)
    return toEigen(pNodeAnim->mRotationKeys[0].mValue);
  GLuint rotationIndex=findRotation(animationTimeTicks,pNodeAnim);
  GLuint nextRotationIndex=rotationIndex+1;
  ASSERT(nextRotationIndex<pNodeAnim->mNumRotationKeys);
  GLfloat t1=(GLfloat)pNodeAnim->mRotationKeys[rotationIndex].mTime;
  if(t1>animationTimeTicks) {
    return toEigen(pNodeAnim->mRotationKeys[rotationIndex].mValue);
  } else {
    GLfloat t2=(GLfloat)pNodeAnim->mRotationKeys[nextRotationIndex].mTime;
    GLfloat deltaTime=t2-t1;
    GLfloat factor=(animationTimeTicks-t1)/deltaTime;
    ASSERT(factor>=0.0f && factor<=1.0f);
    const aiQuaternion& startRotationQ=pNodeAnim->mRotationKeys[rotationIndex].mValue;
    const aiQuaternion& endRotationQ  =pNodeAnim->mRotationKeys[nextRotationIndex].mValue;
    aiQuaternion::Interpolate(out,startRotationQ,endRotationQ,factor);
  }
  out.Normalize();
  return toEigen(out);
}
Eigen::Matrix<GLfloat,3,1> SkinnedMeshShape::calcInterpolatedTranslation(GLfloat animationTimeTicks,const aiNodeAnim* pNodeAnim) const {
  //we need at least two values to interpolate...
  if(pNodeAnim->mNumPositionKeys==1)
    return toEigen(pNodeAnim->mPositionKeys[0].mValue);
  GLuint positionIndex=findPosition(animationTimeTicks,pNodeAnim);
  GLuint nextPositionIndex=positionIndex+1;
  ASSERT(nextPositionIndex<pNodeAnim->mNumPositionKeys)
  GLfloat t1=(GLfloat)pNodeAnim->mPositionKeys[positionIndex].mTime;
  if(t1>animationTimeTicks)
    return toEigen(pNodeAnim->mPositionKeys[positionIndex].mValue);
  else {
    GLfloat t2=(GLfloat)pNodeAnim->mPositionKeys[nextPositionIndex].mTime;
    GLfloat deltaTime=t2-t1;
    GLfloat factor=(animationTimeTicks-t1)/deltaTime;
    ASSERT(factor>=0.0f && factor<=1.0f);
    Eigen::Matrix<GLfloat,3,1> start=toEigen(pNodeAnim->mPositionKeys[positionIndex].mValue);
    Eigen::Matrix<GLfloat,3,1> end=toEigen(pNodeAnim->mPositionKeys[nextPositionIndex].mValue);
    Eigen::Matrix<GLfloat,3,1> delta=end-start;
    return start+factor*delta;
  }
}
void SkinnedMeshShape::readNodeHierarchy(GLfloat animationTimeTicks,const aiNode* pNode,const Eigen::Matrix<GLfloat,4,4>& parentTrans,const aiAnimation& animation) {
  std::string nodeName(pNode->mName.data);
  //std::cout << "Computing bone:" << nodeName << std::endl;
  Eigen::Matrix<GLfloat,4,4> nodeTrans=toEigen(pNode->mTransformation);
  const aiNodeAnim* pNodeAnim=findNodeAnim(animation,nodeName);
  if(pNodeAnim) {
    //scale
    Eigen::Matrix<GLfloat,4,4> scale=Eigen::Matrix<GLfloat,4,4>::Identity();
    scale.diagonal().template segment<3>(0)=calcInterpolatedScaling(animationTimeTicks,pNodeAnim);
    //rotate
    Eigen::Matrix<GLfloat,4,4> rotate=Eigen::Matrix<GLfloat,4,4>::Identity();
    scale.template block<3,3>(0,0)=calcInterpolatedRotation(animationTimeTicks,pNodeAnim);
    //translate
    Eigen::Matrix<GLfloat,4,4> translate=Eigen::Matrix<GLfloat,4,4>::Identity();
    translate.col(3).template segment<3>(0)=calcInterpolatedTranslation(animationTimeTicks,pNodeAnim);
    nodeTrans=translate*rotate*scale;
  }
  //record transform
  Eigen::Matrix<GLfloat,4,4> globalTrans=parentTrans*nodeTrans;
  if(_boneNameToIndexMap.find(nodeName)!=_boneNameToIndexMap.end()) {
    GLuint boneIndex=_boneNameToIndexMap[nodeName];
    _bones[boneIndex]._finalTrans=globalTrans*_bones[boneIndex]._offsetTrans;
  }
  //descend
  for(GLuint i=0; i<pNode->mNumChildren; i++) {
    std::string childName(pNode->mChildren[i]->mName.data);
    readNodeHierarchy(animationTimeTicks,pNode->mChildren[i],globalTrans,animation);
  }
}
void SkinnedMeshShape::updateMeshVerticesCPU(std::shared_ptr<MeshShape> out,std::shared_ptr<MeshShape> in,const BoneData& boneData) const {
  ASSERT(out->nrVertex()==in->nrVertex())
  for(GLuint i=0; i<(GLuint)in->nrVertex(); i++) {
    Eigen::Matrix<GLfloat,3,1> pos=in->getVertex(i);
    Eigen::Matrix<GLfloat,3,1> nor=in->getNormal(i);
    Eigen::Matrix<GLfloat,4,4> trans=Eigen::Matrix<GLfloat,4,4>::Zero();
    for(GLuint j=0; j<boneData._maxNrBone; j++) {
      GLint bid=boneData._boneId[i*boneData._maxNrBone+j];
      GLfloat bw=boneData._boneWeight[i*boneData._maxNrBone+j];
      if(bid>=0)
        trans+=_bones[bid]._finalTrans*bw;
    }
    out->setVertex(i,trans.template block<3,3>(0,0)*pos+trans.template block<3,1>(0,3));
    out->setNormal(i,(trans.template block<3,3>(0,0)*nor).normalized());
  }
}
void SkinnedMeshShape::updateMeshVerticesGPU(std::shared_ptr<MeshShape> out,std::shared_ptr<MeshShape> in,const BoneData& boneData) const {
  ASSERT(out->nrVertex()==in->nrVertex())
  //setup program
  getTransformFeedbackProg()->begin();
  Eigen::Matrix<GLfloat,4,-1> boneTrans=getBoneTransforms(128);
  ASSERT_MSG(boneTrans.cols()==128*4,"We only support at most 128 bones!")
  getTransformFeedbackProg()->setUniformFloat("boneTrans",boneTrans);
  //perform computation
  glEnable(GL_RASTERIZER_DISCARD);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,out->getVBO()->VBOV());
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,1,out->getVBO()->VBON());
  glBeginTransformFeedback(GL_POINTS);
  in->getVBO()->draw(GL_POINTS);
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glFlush();
  //finalize program
  Program::currentProgram()->end();
  //synchronize CPU
  Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(out->_vertices.data(),out->_vertices.size())=out->getVBO()->VBOVData();
}
std::shared_ptr<Program> SkinnedMeshShape::getTransformFeedbackProg() const {
#include "Shader/BoneTransformVert.h"
  std::shared_ptr<Program> transformFeedbackProgram=Program::findProgram("BoneTransform");
  if(!transformFeedbackProgram) {
    Shader::registerShader("BoneTransform",BoneTransformVert);
    Program::registerProgram("BoneTransform",[&](GLuint program) {
      const char* varyings[]= {"VertexOut","NormalOut"};
      glTransformFeedbackVaryings(program,2,varyings,GL_SEPARATE_ATTRIBS);
    },"BoneTransform");
    transformFeedbackProgram=Program::findProgram("BoneTransform");
  }
  return transformFeedbackProgram;
}
}
