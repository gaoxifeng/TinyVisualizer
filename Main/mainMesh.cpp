#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <iostream>
#include <fstream>
#include <ctime>
using namespace DRAWER;

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);
  std::ifstream newfile;
  std::string t,a,b,c;
  std::vector<GLfloat> vertices;
  std::vector<GLuint> indices,indicesE;
  newfile.open("bunny.obj",std::ios::in);
  if(newfile.is_open()) {
    std::string tp;
    while(std::getline(newfile,tp)) {
      if(tp.length()>=2 && tp.substr(0,2)=="v ") {
        std::istringstream os(tp);
        os >> t >> a >> b >> c;
        vertices.push_back((GLfloat)atof(a.c_str()));
        vertices.push_back((GLfloat)atof(b.c_str()));
        vertices.push_back((GLfloat)atof(c.c_str()));
      } else if(tp.length()>=2 && tp.substr(0,2)=="f ") {
        std::istringstream os(tp);
        os >> t >> a >> b >> c;
        //face
        indices.push_back(atoi(a.c_str())-1);
        indices.push_back(atoi(b.c_str())-1);
        indices.push_back(atoi(c.c_str())-1);
        //edge
        int off=(int)indices.size()-3;
        indicesE.push_back(indices[off+0]);
        indicesE.push_back(indices[off+1]);
        indicesE.push_back(indices[off+0]);
        indicesE.push_back(indices[off+2]);
        indicesE.push_back(indices[off+1]);
        indicesE.push_back(indices[off+2]);
      }
    }
    newfile.close();
  }
  Eigen::Matrix<GLfloat,-1,3,Eigen::RowMajor> DV=Eigen::Map<Eigen::Matrix<GLfloat,-1,3,Eigen::RowMajor>>(vertices.data(),vertices.size()/3,3);
  Eigen::Matrix<GLuint,-1,3,Eigen::RowMajor> DF=Eigen::Map<Eigen::Matrix<GLuint,-1,3,Eigen::RowMajor>>(indices.data(),indices.size()/3,3);
  Eigen::Matrix<GLuint,-1,2,Eigen::RowMajor> DE=Eigen::Map<Eigen::Matrix<GLuint,-1,2,Eigen::RowMajor>>(indicesE.data(),indicesE.size()/2,2);
  std::cout << "Read " << vertices.size()/3 << " vertices, " << indices.size()/3 << " triangles!" << std::endl;

  //fill
  time_t timeStart=clock();
  auto shapeFill=DRAWER::makeTriMesh(true,DV,DF,false);
  drawer.addShape(shapeFill);
  double timeElapsed=double(clock()-timeStart)/CLOCKS_PER_SEC;
  std::cout << "\nFinished in " << timeElapsed << "s!" << std::endl;

  //wire
  timeStart=clock();
  auto shapeWire=DRAWER::makeWires(DV,DE);
  shapeWire->setColor(GL_LINES,1,0,0);
  shapeWire->setUseLight(false);
  shapeWire->setLineWidth(1);
  shapeWire->setEnabled(true);
  drawer.addShape(shapeWire);
  timeElapsed=double(clock()-timeStart)/CLOCKS_PER_SEC;
  std::cout << "\nFinished in " << timeElapsed << "s!" << std::endl;

  //intersect
  std::shared_ptr<Bullet3DShape> shapeI(new Bullet3DShape);
  shapeI->setLocalTranslate(Eigen::Matrix<GLfloat,3,1>(0,0,0));
  shapeI->addShape(makeSphere(8,true,0.001f));
  shapeI->setColor(GL_TRIANGLES,0,0,1);
  shapeI->setUseLight(false);
  drawer.addShape(shapeI);
  drawer.setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS) {
      double x=0,y=0;
      GLfloat IAlpha=1;
      std::shared_ptr<Shape> IShape;
      glfwGetCursorPos(wnd,&x,&y);
      Eigen::Matrix<GLfloat,6,1> ray=drawer.getCameraRay(x,y);
      if(drawer.rayIntersect(ray,IShape,IAlpha)) {
        std::cout << "Intersection IAlpha=" << IAlpha << std::endl;
        shapeI->setLocalTranslate(ray.segment<3>(0)+ray.segment<3>(3)*IAlpha);
      } else std::cout << "No intersection!" << std::endl;
    }
  });
  drawer.mainLoop();
  return 0;
}
