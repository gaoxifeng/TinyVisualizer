#ifdef BOX2D_SUPPORT
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/Box2DShape.h>
#include <TinyVisualizer/SceneStructure.h>
#include <TinyVisualizer/MakeTexture.h>
#include <Box2D/Box2D.h>

using namespace DRAWER;

class AABBCallback : public b2QueryCallback {
 public:
  AABBCallback():_body(NULL) {}
  bool ReportFixture(b2Fixture* fixture) override {
    _body=fixture->GetBody();
    return true;
  }
  const b2Body* _body;
};
#endif
int main(int argc,char** argv) {
#ifdef BOX2D_SUPPORT
  b2World* world=new b2World(b2Vec2(0,-9.81f));
  b2Body* ground=NULL;
  int edgeCount=30;
  {
    b2BodyDef bd;
    ground=world->CreateBody(&bd);
    b2EdgeShape shape;
    shape.Set(b2Vec2(-40.0f,-100.0f),b2Vec2(40.0f,-100.0f));
    ground->CreateFixture(&shape,0.0f);
  }

  {
    b2PolygonShape shape;
    shape.SetAsBox(0.5f,0.125f);

    b2FixtureDef fd;
    fd.shape=&shape;
    fd.density=20.0f;
    fd.friction=0.2f;

    b2RevoluteJointDef jd;
    b2Body* prevBody=ground;
    for(int32 i=0; i<edgeCount; ++i) {
      b2BodyDef bd;
      bd.type=b2_dynamicBody;
      bd.position.Set(-14.5f+1.0f*i,5.0f);
      b2Body* body=world->CreateBody(&bd);
      body->CreateFixture(&fd);

      b2Vec2 anchor(-15.0f+1.0f*i,5.0f);
      jd.Initialize(prevBody,body,anchor);
      world->CreateJoint(&jd);
      prevBody=body;
    }

    b2Vec2 anchor(-15.0f+1.0f*edgeCount,5.0f);
    jd.Initialize(prevBody,ground,anchor);
    world->CreateJoint(&jd);
  }

  for(int32 i=0; i<10; ++i)
    for(int32 j=0; j<10; ++j) {
      b2Vec2 vertices[3];
      vertices[0].Set(-0.5f,0.0f);
      vertices[1].Set(0.5f,0.0f);
      vertices[2].Set(0.0f,1.5f);
      b2PolygonShape shape;
      shape.Set(vertices,3);

      b2FixtureDef fd;
      fd.shape=&shape;
      fd.density=1.0f;

      b2BodyDef bd;
      bd.type=b2_dynamicBody;
      bd.position.Set(-6.0f+i,25.0f+2*j);
      b2Body* body=world->CreateBody(&bd);
      body->CreateFixture(&fd);
    }

  for(int32 i=0; i<10; ++i)
    for(int32 j=0; j<10; ++j) {
      b2CircleShape shape;
      shape.m_radius=0.5f;

      b2FixtureDef fd;
      fd.shape=&shape;
      fd.density=1.0f;

      b2BodyDef bd;
      bd.type=b2_dynamicBody;
      bd.position.Set(-6.0f+i,10.0f+j);
      b2Body* body=world->CreateBody(&bd);
      body->CreateFixture(&fd);
    }

  bool sim=false;
  AABBCallback cb;
  Drawer drawer(argc,argv);
  drawer.addCamera2D(10);
  drawer.clearLight();
  dynamic_cast<Camera2D&>(drawer.getCamera()).setTexture(drawChecker(6,Eigen::Matrix<GLfloat,3,1>(1,1,1),Eigen::Matrix<GLfloat,3,1>(0,.3,.5)),Eigen::Matrix<GLfloat,2,1>(.1,.1));
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>& root) {
    if(sim)
      world->Step(0.01f,100,100);
    std::vector<b2Body*> dss;
    for(b2Body* body=world->GetBodyList(); body; body=body->GetNext())
      if(body->GetTransform().p.y<-1)
        dss.push_back(body);
    for(b2Body* b:dss)
      world->DestroyBody(b);
    Box2DShape::syncWorld(root,world);
    if(cb._body && root) {
      root->visit([&](std::shared_ptr<Shape> s) {
        if(std::dynamic_pointer_cast<Box2DShape>(s) && std::dynamic_pointer_cast<Box2DShape>(s)->getBody()==cb._body) {
          drawer.getCamera().focusOn(s);
          return false;
        } else return true;
      });
    } else drawer.getCamera().focusOn(NULL);
  });
  drawer.setMouseFunc([&](GLFWwindow*,int button,int action,int) {
    if(button==GLFW_MOUSE_BUTTON_1 && action==GLFW_PRESS) {
      b2AABB bb;
      cb._body=NULL;
      Eigen::Matrix<GLfloat,2,1> pos=drawer.getWorldPos();
      bb.lowerBound.x=bb.upperBound.x=pos[0];
      bb.lowerBound.y=bb.upperBound.y=pos[1];
      world->QueryAABB(&cb,bb);
    }
  });
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods) {
    if(key==GLFW_KEY_R && action==GLFW_PRESS)
      sim=!sim;
  });
  drawer.mainLoop();
#endif
  return 0;
}
