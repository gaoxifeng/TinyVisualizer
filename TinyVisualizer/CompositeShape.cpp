#include "CompositeShape.h"
#include "MeshShape.h"
#ifdef ASSIMP_SUPPORT
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Bullet3DShape.h"
#include <unordered_set>
#include <filesystem>
#include <iostream>
#endif

namespace DRAWER {
#ifdef ASSIMP_SUPPORT
struct AssetReader {
  struct Material {
    Material() {
      _ambient.setZero();
      _diffuse.setZero();
      _specular.setZero();
      _shininess=10;
      _useWireframe=false;
    }
    Eigen::Matrix<GLfloat,4,1> _ambient,_diffuse,_specular;
    GLfloat _shininess;
    bool _useWireframe;
    std::string _texFile;
  };
  struct Hash {
    size_t operator()(const Eigen::Matrix<GLuint,2,1>& key) const {
      std::hash<GLuint> h;
      return h(key[0])+h(key[1]);
    }
  };
public:
  void readFromFile(const std::string& path, CompositeShape& shape) {
    Assimp::Importer importer;
    const aiScene* scene=importer.ReadFile(path.c_str(),aiProcess_JoinIdenticalVertices);
    readFromAsset(scene,NULL,Eigen::Matrix<GLfloat,4,4>::Identity(),path);
    //add to shape
    bool wire=false;
    for(int i=0; i<(int)_geoms.size(); i++) {
      std::shared_ptr<Bullet3DShape> shapeI(new Bullet3DShape);
      //set material
      if((int)_materials.size()>i)
        wire=_materials[i]._useWireframe;
      shapeI->addShape(visualizeShape(_geoms[i],wire));
      //set material
      if((int)_materials.size()>i) {
        //ambient
        shapeI->setColorAmbient(GL_POINTS,_materials[i]._ambient[0],_materials[i]._ambient[1],_materials[i]._ambient[2]);
        shapeI->setColorAmbient(GL_LINES,_materials[i]._ambient[0],_materials[i]._ambient[1],_materials[i]._ambient[2]);
        shapeI->setColorAmbient(GL_TRIANGLES,_materials[i]._ambient[0],_materials[i]._ambient[1],_materials[i]._ambient[2]);
        //diffuse
        shapeI->setColorDiffuse(GL_POINTS,_materials[i]._diffuse[0],_materials[i]._diffuse[1],_materials[i]._diffuse[2]);
        shapeI->setColorDiffuse(GL_LINES,_materials[i]._diffuse[0],_materials[i]._diffuse[1],_materials[i]._diffuse[2]);
        shapeI->setColorDiffuse(GL_TRIANGLES,_materials[i]._diffuse[0],_materials[i]._diffuse[1],_materials[i]._diffuse[2]);
        //specular
        shapeI->setColorSpecular(GL_POINTS,_materials[i]._specular[0],_materials[i]._specular[1],_materials[i]._specular[2]);
        shapeI->setColorSpecular(GL_LINES,_materials[i]._specular[0],_materials[i]._specular[1],_materials[i]._specular[2]);
        shapeI->setColorSpecular(GL_TRIANGLES,_materials[i]._specular[0],_materials[i]._specular[1],_materials[i]._specular[2]);
        //shininess
        shapeI->setShininess(GL_POINTS,_materials[i]._shininess);
        shapeI->setShininess(GL_LINES,_materials[i]._shininess);
        shapeI->setShininess(GL_TRIANGLES,_materials[i]._shininess);
        //texture
        if(!_materials[i]._texFile.empty()) {
          std::shared_ptr<DRAWER::Texture> tex=DRAWER::Texture::load(_materials[i]._texFile);
          std::cout << "Loaded texture from " << _materials[i]._texFile << " w=" <<  tex->width() << " h=" << tex->height() << std::endl;
          shapeI->setTextureDiffuse(tex);
        }
      }
      //assign
      shapeI->setLocalTransform(_trans[i]);
      shape.addShape(shapeI);
    }
  }
private:
  void readFromAsset(const aiScene* scene,const aiNode* node,Eigen::Matrix<GLfloat,4,4> trans,const std::string& path) {
    if(!node)
      readFromAsset(scene,scene->mRootNode,trans,path);
    else {
      Eigen::Matrix<GLfloat,4,4> transNode;
      //row0
      transNode(0,0)=node->mTransformation.a1;
      transNode(0,1)=node->mTransformation.a2;
      transNode(0,2)=node->mTransformation.a3;
      transNode(0,3)=node->mTransformation.a4;
      //row1
      transNode(1,0)=node->mTransformation.b1;
      transNode(1,1)=node->mTransformation.b2;
      transNode(1,2)=node->mTransformation.b3;
      transNode(1,3)=node->mTransformation.b4;
      //row2
      transNode(2,0)=node->mTransformation.c1;
      transNode(2,1)=node->mTransformation.c2;
      transNode(2,2)=node->mTransformation.c3;
      transNode(2,3)=node->mTransformation.c4;
      //row3
      transNode(3,0)=node->mTransformation.d1;
      transNode(3,1)=node->mTransformation.d2;
      transNode(3,2)=node->mTransformation.d3;
      transNode(3,3)=node->mTransformation.d4;
      //add children
      for(int i=0; i<(int)node->mNumChildren; i++)
        readFromAsset(scene,node->mChildren[i],trans*transNode,path);
      //add mesh
      for(int idm=0; idm<(int)node->mNumMeshes; idm++) {
        const aiMesh* m=scene->mMeshes[node->mMeshes[idm]];
        const aiMaterial* mat=scene->mMaterials[m->mMaterialIndex];
        std::vector<GLfloat> vss;
        std::vector<GLfloat> tcss;
        std::vector<GLuint> iss;
        for(int i=0; i<(int)m->mNumVertices; i++) {
          const aiVector3D& v=m->mVertices[i];
          vss.push_back(v.x);
          vss.push_back(v.y);
          vss.push_back(v.z);
          if(m->mNumUVComponents[0]==2) {
            const aiVector3D& t=m->mTextureCoords[0][i];
            tcss.push_back(t[0]);
            tcss.push_back(1-t[1]);
          }
        }
        for(int i=0; i<(int)m->mNumFaces; i++) {
          const aiFace& f=m->mFaces[i];
          for(int j=0; j<(int)f.mNumIndices-2; j++) {
            iss.push_back(f.mIndices[0]);
            iss.push_back(f.mIndices[1]);
            iss.push_back(f.mIndices[2]);
          }
        }
        std::shared_ptr<MeshShape> mesh(new MeshShape);
        Eigen::Matrix<GLfloat,-1,1> tc=Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(tcss.data(),tcss.size());
        mesh->addVertex(Eigen::Map<Eigen::Matrix<GLfloat,-1,1>>(vss.data(),vss.size()), &tc);
        mesh->addIndex(Eigen::Map<Eigen::Matrix<GLuint,-1,1>>(iss.data(),iss.size()));
        _geoms.push_back(mesh);
        //add material
        _materials.push_back(initMaterial(*mat,path));
        //add transform
        _trans.push_back(trans*transNode);
      }
    }
  }
  std::shared_ptr<Shape> visualizeShape(std::shared_ptr<MeshShape> m,bool wire) {
    if(wire) {
      std::unordered_set<Eigen::Matrix<GLuint,2,1>,Hash> edges;
      for(int i=0;i<m->nrIndex();i+=3) {
        for(int d=0;d<3;d++) {
          Eigen::Matrix<GLuint,2,1> e(m->getIndex(i+d),m->getIndex(i+(d+1)%3));
          if(e[0]>e[1]) std::swap(e[0],e[1]);
          edges.insert(e);
        }
      }
      std::vector<GLuint> iss;
      for(auto e:edges) {
        iss.push_back(e[0]);
        iss.push_back(e[1]);
      }
      m->clearIndex();
      m->addIndex(Eigen::Map<Eigen::Matrix<GLuint,-1,1>>(iss.data(),iss.size()));
      m->setMode(GL_LINES);
    } else {
      m->setMode(GL_TRIANGLES);
    }
    return m;
  }
  Material initMaterial(const aiMaterial& mat,const std::string& path) {
    Material ret;
    for(int idmat=0; idmat<(int)mat.mNumProperties; idmat++) {
      const aiMaterialProperty* matP=mat.mProperties[idmat];
      if(std::string(matP->mKey.C_Str()).find("clr.ambient")!=std::string::npos)
        ret._ambient=parseVec4(*matP);
      if(std::string(matP->mKey.C_Str()).find("clr.diffuse")!=std::string::npos)
        ret._diffuse=parseVec4(*matP);
      if(std::string(matP->mKey.C_Str()).find("clr.specular")!=std::string::npos)
        ret._specular=parseVec4(*matP);
      if(std::string(matP->mKey.C_Str()).find("mat.shininess")!=std::string::npos)
        ret._shininess=parseFloat(*matP);
      if(std::string(matP->mKey.C_Str()).find("mat.wireframe")!=std::string::npos)
        ret._useWireframe=parseInt(*matP);
    }
    if(mat.GetTextureCount(aiTextureType_DIFFUSE)>0) {
      aiString texPath;
      mat.GetTexture(aiTextureType_DIFFUSE,0,&texPath);
      ret._texFile=path+"/"+texPath.C_Str();
      ASSERT_MSGV(std::filesystem::exists(ret._texFile),"Cannot file texture image: %s!",ret._texFile.c_str())
    }
    return ret;
  }
  Eigen::Matrix<GLfloat,4,1> parseVec4(const aiMaterialProperty& matP) {
    Eigen::Matrix<GLfloat,4,1> ret;
    if(matP.mType==aiPTI_Float) {
      int N=matP.mDataLength/4;
      for(int i=0; i<N; i++)
        ret[i]=parseFloat(matP,i);
      return ret;
    } else if(matP.mType==aiPTI_Double) {
      int N=matP.mDataLength/8;
      for(int i=0; i<N; i++)
        ret[i]=parseFloat(matP,i);
      return ret;
    } else {
      ASSERT_MSG(false,"Reading floating type aiMaterialProperty, but property type is not float!")
      return ret;
    }
  }
  GLfloat parseFloat(const aiMaterialProperty& matP,int index=0) {
    if(matP.mType==aiPTI_Float)
      return ((float*)matP.mData)[index];
    else if(matP.mType==aiPTI_Double)
      return ((double*)matP.mData)[index];
    else {
      ASSERT_MSG(false,"Reading floating type aiMaterialProperty, but property type is not float!")
      return 0;
    }
  }
  int parseInt(const aiMaterialProperty& matP,int index=0) {
    if(matP.mType==aiPTI_Integer)
      return ((int*)matP.mData)[index];
    else {
      ASSERT_MSG(false,"Reading integer type aiMaterialProperty, but property type is not integer!")
      return 0;
    }
  }
  std::vector<std::shared_ptr<MeshShape>> _geoms;
  std::vector<Material> _materials;
  std::vector<Eigen::Matrix<GLfloat,4,4>> _trans;
};
void CompositeShape::readFromFile(const std::string& path) {
  AssetReader().readFromFile(path, *this);
}
#endif
//CompositeShape
void CompositeShape::addShape(std::shared_ptr<Shape> s) {
  _shapes.push_back(s);
}
void CompositeShape::setAlpha(GLfloat alpha) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setAlpha(alpha);
}
void CompositeShape::setPointSize(GLfloat pointSize) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setPointSize(pointSize);
}
void CompositeShape::setLineWidth(GLfloat lineWidth) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setLineWidth(lineWidth);
}
void CompositeShape::setColorDiffuse(GLenum mode,GLfloat R,GLfloat G,GLfloat B)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColorDiffuse(mode,R,G,B);
}
void CompositeShape::setColorAmbient(GLenum mode,GLfloat RA,GLfloat GA,GLfloat BA)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColorAmbient(mode,RA,GA,BA);
}
void CompositeShape::setColorSpecular(GLenum mode,GLfloat RS,GLfloat GS,GLfloat BS)  {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setColorSpecular(mode,RS,GS,BS);
}
void CompositeShape::setShininess(GLenum mode,GLfloat S) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setShininess(mode,S);
}
void CompositeShape::setTextureDiffuse(std::shared_ptr<Texture> tex) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setTextureDiffuse(tex);
}
void CompositeShape::setTextureSpecular(std::shared_ptr<Texture> tex) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setTextureSpecular(tex);
}
void CompositeShape::setDepth(GLfloat depth) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setDepth(depth);
}
void CompositeShape::setDrawer(Drawer* drawer) {
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->setDrawer(drawer);
}
bool CompositeShape::needRecomputeNormal() const {
  for(int i=0; i<(int)_shapes.size(); i++)
    if(_shapes[i]->needRecomputeNormal())
      return true;
  return false;
}
void CompositeShape::draw(PASS_TYPE passType) const {
  if(!enabled())
    return;
  if(!_castShadow && (passType&SHADOW_PASS)!=0)
    return;
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->draw(passType);
}
void CompositeShape::drawPovray(Povray& pov) const {
  if(!enabled())
    return;
  for(int i=0; i<(int)_shapes.size(); i++)
    _shapes[i]->drawPovray(pov);
}
Eigen::Matrix<GLfloat,6,1> CompositeShape::getBB() const {
  Eigen::Matrix<GLfloat,6,1> bb=resetBB();
  for(std::shared_ptr<Shape> s:_shapes)
    bb=unionBB(bb,s->getBB());
  return bb;
}
bool CompositeShape::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const {
  bool ret=false;
  for(int i=0; i<(int)_shapes.size(); i++)
    if(_shapes[i]->rayIntersect(ray,alpha))
      ret=true;
  return ret;
}
void CompositeShape::updateChild(std::shared_ptr<Shape> s,int id) {
  ASSERT_MSGV(id<numChildren(),
              "User called CompositeShape::updateChild with id=%d, but there are only %d children!",
              id,numChildren())
  _shapes[id]=s;
}
std::shared_ptr<Shape> CompositeShape::getChild(int id) const {
  return _shapes[id];
}
bool CompositeShape::contain(std::shared_ptr<Shape> s) const {
  if(s.get()==this)
    return true;
  for(int i=0; i<numChildren(); i++)
    if(s==getChild(i))
      return true;
    else {
      std::shared_ptr<CompositeShape> c=std::custom_pointer_cast<CompositeShape>(getChild(i));
      if(c && c->contain(s))
        return true;
    }
  return false;
}
void CompositeShape::removeChild(std::shared_ptr<Shape> shape) {
  auto end=std::remove(_shapes.begin(),_shapes.end(),shape);
  _shapes.erase(end,_shapes.end());
}
int CompositeShape::numChildren() const {
  return (int)_shapes.size();
}
}
