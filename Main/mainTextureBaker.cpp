#include <TextureBaker/MeshVisualizer.h>
#include <TextureBaker/VisualTextureBaker.h>
#include <TextureBaker/NormalBasedTextureBaker.h>
#include <TextureBaker/VisualTextureBakerStaggered.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/CompositeShape.h>
#include <TinyVisualizer/SceneStructure.h>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/Drawer.h>
#include <iostream>

using namespace DRAWER;

enum BAKER_MODE {
  NORMAL,
  VISUAL,
  VISUAL_STAGGERED,
  NONE,
};

enum VIS_MODE {
  TEXTURE,
  LOW_MESH,
  HIGH_MESH,
  SAMPLED_RAY,
};

int main(int argc,char** argv) {
  Drawer drawer(argc,argv);

  int res=2048;
  int resSphere=8;
  VIS_MODE mode= TEXTURE;
  BAKER_MODE bakerMode= NORMAL;
  std::shared_ptr<TextureBaker> baker;
  auto g=-Eigen::Matrix<GLdouble,3,1>::UnitY();
  //MeshVisualizer visHigh("high-poly/OBJ/SM_M2_Build_Apartment_01.obj");
  //MeshVisualizer visLow("low-poly/SM_M2_Build_Apartment_01.obj");
  MeshVisualizer visHigh("baker-debug-data/OBJ/SM_M2_Build_Apartment_01.obj");
  MeshVisualizer visLow("baker-debug-data/OBJ/SM_M2_Build_Apartment_01-normal.obj");

  if(bakerMode==NORMAL) {
    baker.reset(new NormalBasedTextureBaker(visHigh,visLow,res));
    baker->setNearestTextureExtender();
    baker->bakeTexture();
    baker->save("bakedNormal.png");
  } else if(bakerMode==VISUAL) {
    baker.reset(new VisualTextureBaker(visHigh,visLow,res,resSphere,g));
    baker->setNearestTextureExtender();
    baker->bakeTexture();
    baker->save("bakedVisual.png");
  } else if(bakerMode==VISUAL_STAGGERED) {
    baker.reset(new VisualTextureBakerStaggered(visHigh,visLow,res,resSphere,g));
    baker->setNearestTextureExtender();
    baker->bakeTexture();
    baker->save("bakedVisualStaggered.png");
  }

  drawer.addCamera2D(1);
  static std::shared_ptr<MeshShape> line,point,texcoord;
  std::shared_ptr<Texture> grid=visLow.getComponents().begin()->second._texture;
  drawer.setKeyFunc([&](GLFWwindow* wnd,int key,int scan,int action,int mods,bool captured) {
    if(captured)
      return;
    else if(key==GLFW_KEY_R && action==GLFW_PRESS) {
      if(mode==TEXTURE)
        mode=LOW_MESH;
      else if(mode==LOW_MESH)
        mode=HIGH_MESH;
      else if(mode==HIGH_MESH)
        mode=TEXTURE;
      drawer.clear();
      drawer.clearLight();
      if(mode==TEXTURE) {
        if(texcoord)
          drawer.addShape(texcoord);
        drawer.setDrawFunc([&]() {
          glActiveTexture(GL_TEXTURE0);
          grid->begin();
          glActiveTexture(GL_TEXTURE1);
          FBO::screenQuad(grid);
          grid->end();
          glActiveTexture(GL_TEXTURE0);
        });
        if(!drawer.getCamera2D()) {
          drawer.addCamera2D(10);
        }
      } else if(mode==LOW_MESH) {
        if(line)
          drawer.addShape(line);
        if(point)
          drawer.addShape(point);
        drawer.setDrawFunc([&]() {});
        drawer.addLightSystem(0,10,false);
        drawer.addShape(visLow.getShape());
        if(!drawer.getCamera3D()) {
          drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
          drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
        }
      } else if(mode==HIGH_MESH) {
        if(line)
          drawer.addShape(line);
        if(point)
          drawer.addShape(point);
        drawer.setDrawFunc([&]() {});
        drawer.addLightSystem(0,10,false);
        drawer.addShape(visHigh.getShape());
        if(!drawer.getCamera3D()) {
          drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
          drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
        }
      } else if(mode==SAMPLED_RAY) {
        drawer.setDrawFunc([&]() {});
        drawer.addLightSystem(0,10,false);
        drawer.addShape(visHigh.getShape());
        RayCaster ray(visHigh);
        drawer.addShape(ray.drawRay(resSphere,g));
        if(!drawer.getCamera3D()) {
          drawer.addCamera3D(90,Eigen::Matrix<GLfloat,3,1>(0,1,0));
          drawer.getCamera3D()->setManipulator(std::shared_ptr<CameraManipulator>(new FirstPersonCameraManipulator(drawer.getCamera3D())));
        }
      }
      if(drawer.getLight()) {
        drawer.getLight()->setDefaultLight(drawer.root()->getBB(),2.,
                                           Eigen::Matrix<GLfloat,3,1>(.1,.1,.1),
                                           Eigen::Matrix<GLfloat,3,1>(.5,.5,.5),
                                           Eigen::Matrix<GLfloat,3,1>(0,0,0));
      }
    }
  });
  drawer.setMouseFunc([&](GLFWwindow* wnd,int button,int action,int,bool captured) {
    if(captured)
      return;
    else if(button==GLFW_MOUSE_BUTTON_2 && action==GLFW_PRESS && baker) {
      double x=0,y=0;
      glfwGetCursorPos(wnd,&x,&y);
      //ray
      Eigen::Matrix<GLfloat,6,1> bb=visLow.getShape()->getBB();
      GLdouble len=(bb.segment<3>(3)-bb.segment<3>(0)).norm();
      Eigen::Matrix<GLdouble,6,1> ray=drawer.getCameraRay(x,y).template cast<GLdouble>();
      ray.segment<3>(3)=ray.segment<3>(3)*len*10+ray.segment<3>(0);
      if(line)
        drawer.removeShape(line);
      line.reset(new MeshShape);
      line->addVertex(ray.segment(0,3).cast<GLfloat>());
      line->addVertex(ray.segment(3,3).cast<GLfloat>());
      line->addIndex(Eigen::Matrix<int,2,1>(0,1));
      line->setLineWidth(2);
      line->setMode(GL_LINES);
      line->setColorAmbient(GL_LINES,0,0,0);
      line->setColorSpecular(GL_LINES,0,0,0);
      line->setColor(GL_LINES,1,0,0);
      line->setUseLight(false);
      drawer.addShape(line);
      //point
      if(point)
        drawer.removeShape(point);
      if(texcoord)
        drawer.removeShape(texcoord);
      RayCaster::RayIntersect I=baker->getLowRayCaster().castRay(ray);
      if(I!=RayCaster::RayIntersectNone) {
        point.reset(new MeshShape);
        std::cout << "Intersection" << std::endl;
        point->addVertex(baker->getLowRayCaster().getIntersectVert(I).template cast<GLfloat>());
        point->addIndexSingle(0);
        point->setPointSize(5);
        point->setMode(GL_POINTS);
        point->setColorAmbient(GL_POINTS,0,0,0);
        point->setColorSpecular(GL_POINTS,0,0,0);
        point->setColor(GL_POINTS,1,0,0);
        point->setUseLight(false);
        drawer.addShape(point);
        //texture coord
        texcoord.reset(new MeshShape);
        Eigen::Matrix<GLdouble,2,1> tc=baker->getLowRayCaster().getIntersectTexcoord(I);
        for(int d=0; d<2; d++) {
          while(tc[d]<0)
            tc[d]+=1;
          while(tc[d]>1)
            tc[d]-=1;
          tc[d]=tc[d]*2-1;
        }
        texcoord->addVertex(Eigen::Matrix<GLdouble,3,1>(tc[0],tc[1],-0.5).template cast<GLfloat>());
        std::cout << "Texcoord=(" << tc[0] << "," << tc[1] << ")" << std::endl;
        texcoord->addIndexSingle(0);
        texcoord->setPointSize(5);
        texcoord->setMode(GL_POINTS);
        texcoord->setColorAmbient(GL_POINTS,0,0,0);
        texcoord->setColorSpecular(GL_POINTS,0,0,0);
        texcoord->setColor(GL_POINTS,1,0,0);
        texcoord->setUseLight(false);
      } else std::cout << "No Intersection" << std::endl;
    }
  });
  drawer.mainLoop();
  return 0;
}
