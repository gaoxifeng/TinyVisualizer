#ifdef BOX2D_SUPPORT
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/Box2DShape.h>
#include <TinyVisualizer/SceneStructure.h>
#include <TinyVisualizer/MakeTexture.h>
#include <box2d/box2d.h>
#include <vector>
#include <algorithm>

using namespace DRAWER;

struct AABBContext {
  b2BodyId body;
};
bool aabbCallback(b2ShapeId shapeId, void* context) {
  AABBContext* ctx = (AABBContext*)context;
  ctx->body = b2Shape_GetBody(shapeId);
  return true;
}
#endif
int main(int argc,char** argv) {
#ifdef BOX2D_SUPPORT
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = {0, -9.81f};
  b2WorldId worldId = b2CreateWorld(&worldDef);
  std::vector<b2BodyId> bodyList;

  b2BodyId ground;
  int edgeCount=30;
  {
    b2BodyDef bd = b2DefaultBodyDef();
    ground = b2CreateBody(worldId, &bd);
    bodyList.push_back(ground);

    b2Vec2 vertices[4];
    vertices[0] = {-40.0f, -100.0f};
    vertices[1] = { 40.0f, -100.0f};
    vertices[2] = { 40.0f,  -99.0f};
    vertices[3] = {-40.0f,  -99.0f};
    b2Hull hull = b2ComputeHull(vertices, 4);
    b2Polygon polygon = b2MakePolygon(&hull, 0.0f);
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 0.0f;
    b2CreatePolygonShape(ground, &shapeDef, &polygon);
  }

  {
    b2Polygon box = b2MakeBox(0.5f, 0.125f);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 20.0f;
    shapeDef.material.friction = 0.2f;

    b2BodyId prevBody = ground;
    for(int i=0; i<edgeCount; ++i) {
      b2BodyDef bd = b2DefaultBodyDef();
      bd.type = b2_dynamicBody;
      bd.position = {-14.5f + 1.0f*i, 5.0f};
      b2BodyId body = b2CreateBody(worldId, &bd);
      bodyList.push_back(body);
      b2CreatePolygonShape(body, &shapeDef, &box);

      b2Vec2 anchor = {-15.0f + 1.0f*i, 5.0f};
      b2RevoluteJointDef jd = b2DefaultRevoluteJointDef();
      jd.bodyIdA = prevBody;
      jd.bodyIdB = body;
      jd.localAnchorA = b2Body_GetLocalPoint(prevBody, anchor);
      jd.localAnchorB = b2Body_GetLocalPoint(body, anchor);
      b2CreateRevoluteJoint(worldId, &jd);
      prevBody = body;
    }

    b2Vec2 anchor = {-15.0f + 1.0f*edgeCount, 5.0f};
    b2RevoluteJointDef jd = b2DefaultRevoluteJointDef();
    jd.bodyIdA = prevBody;
    jd.bodyIdB = ground;
    jd.localAnchorA = b2Body_GetLocalPoint(prevBody, anchor);
    jd.localAnchorB = b2Body_GetLocalPoint(ground, anchor);
    b2CreateRevoluteJoint(worldId, &jd);
  }

  for(int i=0; i<10; ++i)
    for(int j=0; j<10; ++j) {
      b2Vec2 vertices[3];
      vertices[0] = {-0.5f, 0.0f};
      vertices[1] = { 0.5f, 0.0f};
      vertices[2] = { 0.0f, 1.5f};
      b2Hull hull = b2ComputeHull(vertices, 3);
      b2Polygon polygon = b2MakePolygon(&hull, 0.0f);

      b2ShapeDef shapeDef = b2DefaultShapeDef();
      shapeDef.density = 1.0f;

      b2BodyDef bd = b2DefaultBodyDef();
      bd.type = b2_dynamicBody;
      bd.position = {-6.0f + i, 25.0f + 2*j};
      b2BodyId body = b2CreateBody(worldId, &bd);
      bodyList.push_back(body);
      b2CreatePolygonShape(body, &shapeDef, &polygon);
    }

  for(int i=0; i<10; ++i)
    for(int j=0; j<10; ++j) {
      b2Circle circle = {{0, 0}, 0.5f};

      b2ShapeDef shapeDef = b2DefaultShapeDef();
      shapeDef.density = 1.0f;

      b2BodyDef bd = b2DefaultBodyDef();
      bd.type = b2_dynamicBody;
      bd.position = {-6.0f + i, 10.0f + j};
      b2BodyId body = b2CreateBody(worldId, &bd);
      bodyList.push_back(body);
      b2CreateCircleShape(body, &shapeDef, &circle);
    }

  bool sim=false;
  AABBContext aabbCtx;
  aabbCtx.body = b2_nullBodyId;
  Drawer drawer(argc,argv);
  drawer.addCamera2D(10);
  drawer.clearLight();
  drawer.setBackground(drawChecker(6,Eigen::Matrix<GLfloat,3,1>(1,1,1),Eigen::Matrix<GLfloat,3,1>(0,.3,.5)),Eigen::Matrix<GLfloat,2,1>(.1,.1));
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>& root) {
    if(sim)
      b2World_Step(worldId, 0.01f, 4);
    //remove bodies that fell below y=-1
    for(auto it=bodyList.begin(); it!=bodyList.end();) {
      if(b2Body_IsValid(*it) && b2Body_GetPosition(*it).y < -1) {
        b2DestroyBody(*it);
        it = bodyList.erase(it);
      } else {
        ++it;
      }
    }
    Box2DShape::syncWorld(root, worldId, bodyList);
    if(B2_IS_NON_NULL(aabbCtx.body) && root) {
      root->visit([&](std::shared_ptr<Shape> s) {
        auto bs = std::custom_pointer_cast<Box2DShape>(s);
        if(bs && B2_ID_EQUALS(bs->getBody(), aabbCtx.body)) {
          drawer.getCamera()->focusOn(s);
          return false;
        } else return true;
      });
    } else drawer.getCamera()->focusOn(NULL);
  });
  drawer.setMouseFunc([&](GLFWwindowPtr,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_1 && action==GLFW_PRESS) {
      aabbCtx.body = b2_nullBodyId;
      Eigen::Matrix<GLfloat,2,1> pos=drawer.getCameraRay();
      b2AABB bb;
      bb.lowerBound = {pos[0], pos[1]};
      bb.upperBound = {pos[0], pos[1]};
      b2World_OverlapAABB(worldId, bb, b2DefaultQueryFilter(), aabbCallback, &aabbCtx);
    }
  });
  drawer.setKeyFunc([&](GLFWwindowPtr wnd,int key,int scan,int action,int mods,bool captured) {
    if(captured)
      return;
    else if(key==GLFW_KEY_R && action==GLFW_PRESS)
      sim=!sim;
  });
  drawer.mainLoop();
  b2DestroyWorld(worldId);
#endif
  return 0;
}
