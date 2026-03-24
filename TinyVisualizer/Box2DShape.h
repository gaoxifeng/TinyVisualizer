#ifndef BOX2D_SHAPE_H
#define BOX2D_SHAPE_H

#include "Drawer.h"
#include "MeshShape.h"
#include "CompositeShape.h"
#ifdef BOX2D_SUPPORT
#include <box2d/box2d.h>
#endif

namespace DRAWER {
class Box2DShape : public CompositeShape {
  RTTI_DECLARE_TYPEINFO(Box2DShape, CompositeShape);
 public:
  Box2DShape();
#ifdef BOX2D_SUPPORT
  Box2DShape(b2BodyId bodyId,int RES=8);
  static void syncWorld(std::shared_ptr<SceneNode>& root,b2WorldId worldId,
                        std::vector<b2BodyId>& bodyList,
                        const Eigen::Matrix<GLfloat,3,1>& c=Eigen::Matrix<GLfloat,3,1>(.8,.8,.8),
                        const Eigen::Matrix<GLfloat,3,1>& cB=Eigen::Matrix<GLfloat,3,1>(.7,.7,.7),
                        GLfloat lineWidth=2);
  b2BodyId getBody() const;
#else
  const void* getBody() const;
#endif
  virtual void draw(PASS_TYPE passType) const override;
  virtual void drawPovray(Povray& pov) const override;
  virtual Eigen::Matrix<GLfloat,6,1> getBB() const override;
  virtual bool rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha) const override;
  void setLocalTransform(GLfloat posx,GLfloat posy,GLfloat scale);
 private:
#ifdef BOX2D_SUPPORT
  void createShape(b2BodyId bodyId,int RES);
  std::shared_ptr<MeshShape> _fill,_border;
  b2BodyId _bodyId;
#endif
  GLfloat _posx,_posy,_scale;
};
}

#endif
