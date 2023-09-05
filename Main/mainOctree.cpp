#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/SceneStructure.h>
#include <iostream>

using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::shared_ptr<SceneNode> root;
  std::vector<std::shared_ptr<Bullet3DShape>> shapes;
  std::shared_ptr<MeshShape> sphere=makeSphere(8,true,0.25f);
  for(int i=0; i<100; i++) {
    std::shared_ptr<Bullet3DShape> shapeTA(new Bullet3DShape);
    shapeTA->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>::Random());
    shapeTA->addShape(sphere);
    shapes.push_back(shapeTA);
    //create
    std::cout << "Inserting: " << shapes.size() << std::endl;
    root=SceneNode::update(root,shapeTA);
    root->parityCheck();
  }
  //perturb position
  while(!shapes.empty()) {
    std::cout << "Updating: " << shapes.size() << std::endl;
    for(std::shared_ptr<Bullet3DShape> s:shapes)
      s->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>::Random());
    //update
    root=SceneNode::update(root);
    root->parityCheck();
    //remove
    root=SceneNode::remove(root,shapes.back());
    shapes.pop_back();
    if(root)
      root->parityCheck();
  }
  return 0;
}
