#ifdef BULLET_SUPPORT
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/TrackballCameraManipulator.h>
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>

using namespace DRAWER;

btRigidBody* createRigidBody(btDiscreteDynamicsWorld* dynamicsWorld,float mass,const btTransform& startTransform,btCollisionShape* shape) {
  btAssert((!shape || shape->getShapeType()!=INVALID_SHAPE_PROXYTYPE));
  //rigidbody is dynamic if and only if mass is non zero, otherwise static
  bool isDynamic=(mass!=0.f);
  btVector3 localInertia(0,0,0);
  if (isDynamic)
    shape->calculateLocalInertia(mass,localInertia);
  //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
  btDefaultMotionState* motionState=new btDefaultMotionState(startTransform);
  btRigidBody::btRigidBodyConstructionInfo cInfo(mass,motionState,shape,localInertia);
  btRigidBody* body=new btRigidBody(cInfo);
  body->setUserIndex(-1);
  dynamicsWorld->addRigidBody(body);
  return body;
}
#endif
int main(int argc,char** argv) {
#ifdef BULLET_SUPPORT
#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5
  btDefaultCollisionConfiguration* collisionConfiguration=new btDefaultCollisionConfiguration();
  //m_collisionConfiguration->setConvexConvexMultipointIterations();
  //use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
  btCollisionDispatcher* dispatcher=new btCollisionDispatcher(collisionConfiguration);
  btDbvtBroadphase* broadphase=new btDbvtBroadphase();
  //the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
  btSequentialImpulseConstraintSolver* sol=new btSequentialImpulseConstraintSolver;
  btDiscreteDynamicsWorld* dynamicsWorld=new btDiscreteDynamicsWorld(dispatcher,broadphase,sol,collisionConfiguration);
  dynamicsWorld->setGravity(btVector3(0,-10,0));

  //create a ground
  btBoxShape* groundShape=new btBoxShape(btVector3(5,1,5));
  btTransform groundTransform;
  groundTransform.setIdentity();
  groundTransform.setOrigin(btVector3(0,0,0));
  createRigidBody(dynamicsWorld,0.,groundTransform,groundShape);

  //create a few dynamic rigidbodies
  //Re-using the same collision is better for memory usage and performance
  btBoxShape* boxShape=new btBoxShape(btVector3(.1,.1,.1));
  btSphereShape* sphereShape=new btSphereShape(.1);
  btCapsuleShapeX* capsuleXShape=new btCapsuleShapeX(.1,.2);
  btCapsuleShapeZ* capsuleZShape=new btCapsuleShapeZ(.1,.2);
  //Create Dynamic Objects
  btTransform startTransform;
  startTransform.setIdentity();
  //rigidbody is dynamic if and only if mass is non zero, otherwise static
  btVector3 localInertia(0,0,0);
  boxShape->calculateLocalInertia(1.,localInertia);
  sphereShape->calculateLocalInertia(1.,localInertia);
  capsuleXShape->calculateLocalInertia(1.,localInertia);
  capsuleZShape->calculateLocalInertia(1.,localInertia);
  for(int k=0; k<ARRAY_SIZE_Y; k++)
    for(int i=0; i<ARRAY_SIZE_X; i++)
      for(int j=0; j<ARRAY_SIZE_Z; j++) {
        startTransform.setOrigin(btVector3(   .2*i,1.5+.2*k,.2*j));
        if((k+i+j)%2==0)
          createRigidBody(dynamicsWorld,1.,startTransform,boxShape);
        else createRigidBody(dynamicsWorld,1.,startTransform,sphereShape);

        startTransform.setOrigin(btVector3( 2+.2*i,1.5+.2*k,.4*j));
        createRigidBody(dynamicsWorld,1.,startTransform,capsuleZShape);

        startTransform.setOrigin(btVector3(-2+.4*i,1.5+.2*k,.2*j));
        createRigidBody(dynamicsWorld,1.,startTransform,capsuleXShape);
      }

  bool sim=false;
  Drawer drawer(argc,argv);
#define USE_LIGHT
#ifdef USE_LIGHT
  drawer.addLightSystem(2048);
  drawer.getLight().lightSz()=10;
  drawer.getLight().addLight(Eigen::Matrix<GLfloat,3,1>(0,5, 2),
                             Eigen::Matrix<GLfloat,3,1>(1,1,1),
                             Eigen::Matrix<GLfloat,3,1>(1,1,1),
                             Eigen::Matrix<GLfloat,3,1>(0,0,-2));
  drawer.getLight().addLight(Eigen::Matrix<GLfloat,3,1>(0,5,0),
                             Eigen::Matrix<GLfloat,3,1>(1,1,1),
                             Eigen::Matrix<GLfloat,3,1>(1,0,0),
                             Eigen::Matrix<GLfloat,3,1>(0,0,0));
#endif
  std::shared_ptr<Texture> checker=drawGrid();
  drawer.setFrameFunc([&](std::shared_ptr<SceneNode>& root) {
    if(sim)
      dynamicsWorld->stepSimulation(0.01f);
    btCollisionObjectArray& bss=dynamicsWorld->getCollisionObjectArray();
    btCollisionObjectArray dss;
    for(int i=0; i<bss.size(); i++) {
      btCollisionObject* b=bss.at(i);
      if(b->getWorldTransform().getOrigin().y()<-1)
        dss.push_back(b);
    }
    for(int i=0; i<dss.size(); i++)
      dynamicsWorld->removeCollisionObject(dss.at(i));
    Bullet3DShape::syncWorld(root,dynamicsWorld,checker);
  });
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods) {
    if(key==GLFW_KEY_R && action==GLFW_PRESS)
      sim=!sim;
  });
  drawer.mainLoop();
#endif
  return 0;
}
