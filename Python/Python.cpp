#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
//essential components
#include <TinyVisualizer/SceneStructure.h>
#include <TinyVisualizer/Texture.h>
#include <TinyVisualizer/Povray.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/MultiDrawer.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/MakeTexture.h>
//plugins
#include <TinyVisualizer/CameraExportPlugin.h>
#include <TinyVisualizer/CaptureGIFPlugin.h>
#include <TinyVisualizer/CaptureMPEG2Plugin.h>
#include <TinyVisualizer/ImGuiPlugin.h>
//extra components
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/ArrowShape.h>
#include <TinyVisualizer/BezierCurveShape.h>
#include <TinyVisualizer/CellShape.h>
#include <TinyVisualizer/CompositeShape.h>
#include <TinyVisualizer/Box2DShape.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/LowDimensionalMeshShape.h>
#include <TinyVisualizer/SkinnedMeshShape.h>
#include <TinyVisualizer/TerrainShape.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/TrackballCameraManipulator.h>

namespace py=pybind11;
using namespace DRAWER;
//essential components
#include "Constants.h"
#include "PythonImgui.h"
void initGLFWwindowPtr(py::module& m) {
  py::class_<GLFWwindowPtr>(m,"GLFWwindowPtr")
  .def(py::init());
}
void initSceneNode(py::module& m) {
  py::class_<SceneNode,
  std::shared_ptr<SceneNode>>(m,"SceneNode")
  .def(py::init())
  .def(py::init<const Eigen::Matrix<int,3,1>&,int>())
  .def("update",static_cast<void(SceneNode::*)(std::shared_ptr<Shape>&)>(&SceneNode::update))
  .def("remove",static_cast<bool(SceneNode::*)(std::shared_ptr<Shape>)>(&SceneNode::remove))
  .def("tryAssign",static_cast<void(SceneNode::*)(std::shared_ptr<SceneNode>)>(&SceneNode::tryAssign))
  .def("tryAssign",static_cast<bool(SceneNode::*)(std::shared_ptr<Shape>,const Eigen::Matrix<GLfloat,6,1>&)>(&SceneNode::tryAssign))
  .def("merge",&SceneNode::merge)
  .def("nrShape",&SceneNode::nrShape)
  .def("nrChildren",&SceneNode::nrChildren)
  .def("isLeaf",&SceneNode::isLeaf)
  .def("empty",&SceneNode::empty)
  .def("parityCheck",&SceneNode::parityCheck)
  .def("getBB",&SceneNode::getBB)
  .def("visit",static_cast<void(SceneNode::*)(std::function<bool(const SceneNode&)>)const>(&SceneNode::visit))
  .def("visit",static_cast<bool(SceneNode::*)(std::function<bool(std::shared_ptr<Shape>)>)const>(&SceneNode::visit))
  .def("rayIntersect",&SceneNode::rayIntersect)
  .def("draw",&SceneNode::draw);
}
void initTexture(py::module& m) {
  py::class_<Texture,
  std::shared_ptr<Texture>>(m,"Texture")
  .def(py::init<int,int,GLenum,bool>())
  .def(py::init<const Texture&>())
  .def("begin",&Texture::begin)
  .def("end",&Texture::end)
  .def("width",&Texture::width)
  .def("height",&Texture::height)
  .def("id",&Texture::id)
  .def("loadCPUData",&Texture::loadCPUData)
  .def("syncGPUData",&Texture::syncGPUData)
  .def("save",static_cast<void(Texture::*)(const std::string&,int quality,const Eigen::Matrix<GLfloat,4,1>*)const>(&Texture::save))
  .def_static("load",static_cast<std::shared_ptr<Texture>(*)(const std::string&)>(&Texture::load))
  .def("setDataChannel",static_cast<void(Texture::*)(int,const Eigen::Matrix<GLfloat,-1,-1>&)>(&Texture::setDataChannel))
  .def("getDataChannel",static_cast<Eigen::Matrix<GLfloat,-1,-1>(Texture::*)(int)const>(&Texture::getDataChannel))
  .def("setData",static_cast<void(Texture::*)(int,int,const Eigen::Matrix<GLfloat,4,1>&)>(&Texture::setData))
  .def("getData",static_cast<Eigen::Matrix<GLfloat,4,1>(Texture::*)(int,int)const>(&Texture::getData))
  .def("getData",static_cast<Eigen::Matrix<GLfloat,4,1>(Texture::*)(const Eigen::Matrix<GLfloat,2,1>&)const>(&Texture::getData))
  .def("getData",static_cast<Eigen::Matrix<GLdouble,4,1>(Texture::*)(const Eigen::Matrix<GLdouble,2,1>&)const>(&Texture::getData));
}
void initPovray(py::module& m) {
  py::class_<Povray,
  std::shared_ptr<Povray>>(m,"Povray")
  .def(py::init<const std::string&>());
}
void initShape(py::module& m) {
  py::class_<Shape,
  std::shared_ptr<Shape>>(m,"Shape")
  //.def(py::init())
  .def("setAlpha",&Shape::setAlpha)
  .def("setPointSize",&Shape::setPointSize)
  .def("setLineWidth",&Shape::setLineWidth)
  .def("setColorDiffuse",&Shape::setColorDiffuse)
  .def("setColorAmbient",&Shape::setColorAmbient)
  .def("setColorSpecular",&Shape::setColorSpecular)
  .def("setShininess",&Shape::setShininess)
  .def("setTextureDiffuse",&Shape::setTextureDiffuse)
  .def("setTextureSpecular",&Shape::setTextureSpecular)
  .def("setDepth",&Shape::setDepth)
  .def("setDrawer",&Shape::setDrawer)
  .def("setEnabled",&Shape::setEnabled)
  .def("setCastShadow",&Shape::setCastShadow)
  .def("setUseLight",&Shape::setUseLight)
  .def("needRecomputeNormal",&Shape::needRecomputeNormal)
  .def("enabled",&Shape::enabled)
  .def("castShadow",&Shape::castShadow)
  .def("useLight",&Shape::useLight)
  .def("draw",&Shape::draw)
  .def("drawPovray",&Shape::drawPovray)
  .def("getBB",&Shape::getBB)
  .def("rayIntersect",[](std::shared_ptr<Shape> shape,const Eigen::Matrix<GLfloat,6,1>& ray,GLfloat& alpha)->std::tuple<bool,GLfloat> {
    bool ret=shape->rayIntersect(ray,alpha);
    return std::make_tuple(ret,alpha);
  });
}
void initCamera(py::module& m) {
  py::class_<Camera,
  std::shared_ptr<Camera>>(m,"Camera")
  .def("focusOn",&Camera::focusOn)
  .def("frame",&Camera::frame)
  .def("mouse",&Camera::mouse)
  .def("wheel",&Camera::wheel)
  .def("motion",&Camera::motion)
  .def("key",&Camera::key)
  .def("draw",&Camera::draw)
  .def("drawPovray",&Camera::drawPovray)
  .def("getCameraRay",&Camera::getCameraRay)
  .def("getViewFrustum",&Camera::getViewFrustum)
  .def("saveCamera",&Camera::saveCamera)
  .def("loadCamera",&Camera::loadCamera);
}
void initCameraManipulator(py::module& m) {
  py::class_<CameraManipulator,
  std::shared_ptr<CameraManipulator>>(m,"CameraManipulator")
  .def(py::init<std::shared_ptr<Camera3D>>())
  .def("frame",&CameraManipulator::frame)
  .def("mouse",&CameraManipulator::mouse)
  .def("wheel",&CameraManipulator::wheel)
  .def("motion",&CameraManipulator::motion)
  .def("key",&CameraManipulator::key)
  .def("preDraw",&CameraManipulator::preDraw)
  .def("postDraw",&CameraManipulator::postDraw)
  .def("imGuiCallback",&CameraManipulator::imGuiCallback)
  .def("init",&CameraManipulator::init);
}
void initCamera2D(py::module& m) {
  py::class_<Camera2D,Camera,
  std::shared_ptr<Camera2D>>(m,"Camera2D")
  .def(py::init<GLfloat>());
}
void initCamera3D(py::module& m) {
  py::class_<Camera3D,Camera,
  std::shared_ptr<Camera3D>>(m,"Camera3D")
  .def(py::init<GLfloat,const Eigen::Matrix<GLfloat,3,1>&>())
  .def("setManipulator",&Camera3D::setManipulator)
  .def("getManipulator",&Camera3D::getManipulator)
  .def("position",&Camera3D::position)
  .def("direction",&Camera3D::direction)
  .def("up",&Camera3D::up)
  .def("setPosition",&Camera3D::setPosition)
  .def("setDirection",&Camera3D::setDirection)
  .def("setUp",&Camera3D::setUp);
}
void initPlugin(py::module& m) {
  py::class_<Plugin,
  std::shared_ptr<Plugin>>(m,"Plugin")
  .def(py::init())
  .def("setDrawer",&Plugin::setDrawer)
  .def("init",&Plugin::init)
  .def("finalize",&Plugin::finalize)
  .def("preDraw",&Plugin::preDraw)
  .def("postDraw",&Plugin::postDraw)
  .def("frame",&Plugin::frame)
  .def("mouse",&Plugin::mouse)
  .def("wheel",&Plugin::wheel)
  .def("motion",&Plugin::motion)
  .def("key",&Plugin::key);
}
void initDrawer(py::module& m) {
  py::class_<Drawer,
  std::shared_ptr<Drawer>>(m,"Drawer")
  .def(py::init<const std::vector<std::string>&>())
  .def("setRes",&Drawer::setRes)
  .def("setBackground",static_cast<void(Drawer::*)(GLfloat,GLfloat,GLfloat)>(&Drawer::setBackground))
  .def("setBackground",static_cast<void(Drawer::*)(std::shared_ptr<Texture>,const Eigen::Matrix<GLfloat,2,1>&)>(&Drawer::setBackground))
  .def("addLightSystem",&Drawer::addLightSystem)
  .def("timer",&Drawer::timer)
  .def("frame",&Drawer::frame)
  .def("draw",&Drawer::draw)
  .def("drawPovray",[](std::shared_ptr<Drawer> drawer,std::shared_ptr<Povray> pov) {
    drawer->drawPovray(*pov);
  })
  .def("setMouseFunc",&Drawer::setMouseFunc)
  .def("setWheelFunc",&Drawer::setWheelFunc)
  .def("setMotionFunc",&Drawer::setMotionFunc)
  .def("setKeyFunc",&Drawer::setKeyFunc)
  .def("setFrameFunc",&Drawer::setFrameFunc)
  .def("setDrawFunc",&Drawer::setDrawFunc)
  .def("addCamera2D",&Drawer::addCamera2D)
  .def("addCamera3D",static_cast<void(Drawer::*)(GLfloat,const Eigen::Matrix<GLfloat,3,1>&)>(&Drawer::addCamera3D))
  .def("addCamera3D",static_cast<void(Drawer::*)(GLfloat,const Eigen::Matrix<GLfloat,3,1>&,const Eigen::Matrix<GLfloat,3,1>&,const Eigen::Matrix<GLfloat,3,1>&)>(&Drawer::addCamera3D))
  .def("rayIntersect",&Drawer::rayIntersect)
  .def("getCameraRay",static_cast<Eigen::Matrix<GLfloat,-1,1>(Drawer::*)(double,double)>(&Drawer::getCameraRay))
  .def("getCameraRay",static_cast<Eigen::Matrix<GLfloat,-1,1>(Drawer::*)()>(&Drawer::getCameraRay))
  .def("root",&Drawer::root)
  .def("getLight",&Drawer::getLight)
  .def("clearLight",&Drawer::clearLight)
  .def("getCamera",&Drawer::getCamera)
  .def("getCamera2D",&Drawer::getCamera2D)
  .def("getCamera3D",&Drawer::getCamera3D)
  .def("nextFrame",&Drawer::nextFrame)
  .def("mainLoop",&Drawer::mainLoop)
  .def("FPS",&Drawer::FPS)
  .def("addPlugin",&Drawer::addPlugin)
  .def("contain",&Drawer::contain)
  .def("removeShape",&Drawer::removeShape)
  .def("addShape",&Drawer::addShape)
  .def("focusOn",&Drawer::focusOn)
  .def("clearScene",&Drawer::clearScene)
  .def("clear",&Drawer::clear);
}
void initMultiDrawer(py::module& m) {
  py::class_<MultiDrawer,
  std::shared_ptr<MultiDrawer>>(m,"MultiDrawer")
  .def(py::init<const std::vector<std::string>&>())
  .def("setViewportLayout",&MultiDrawer::setViewportLayout)
  .def("getDrawer",&MultiDrawer::getDrawer)
  .def("getViewport",&MultiDrawer::getViewport)
  .def("setRes",&MultiDrawer::setRes)
  .def("timer",&MultiDrawer::timer)
  .def("frame",&MultiDrawer::frame)
  .def("draw",&MultiDrawer::draw)
  .def("nextFrame",&MultiDrawer::nextFrame)
  .def("mainLoop",&MultiDrawer::mainLoop)
  .def("addPlugin",&MultiDrawer::addPlugin)
  .def("clear",&MultiDrawer::clear);
}
void initShadowAndLight(py::module& m) {
  py::class_<Material>(m,"Material")
  .def(py::init<>())
  .def_readwrite("_ambient",&Material::_ambient)
  .def_readwrite("_ambient",&Material::_diffuse)
  .def_readwrite("_ambient",&Material::_specular)
  .def_readwrite("_ambient",&Material::_texDiffuse)
  .def_readwrite("_ambient",&Material::_texSpecular)
  .def_readwrite("_ambient",&Material::_pointSize)
  .def_readwrite("_ambient",&Material::_lineWidth)
  .def_readwrite("_ambient",&Material::_shininess);

  py::class_<ShadowLight,
  std::shared_ptr<ShadowLight>>(m,"ShadowLight")
  .def(py::init<int,int,bool>())
  .def("addLight",&ShadowLight::addLight)
  .def("setDefaultLight",&ShadowLight::setDefaultLight)
  .def("setLightPos",&ShadowLight::setLightPos)
  .def("getLightPos",&ShadowLight::getLightPos)
  .def("setLightAmbient",&ShadowLight::setLightAmbient)
  .def("getLightAmbient",&ShadowLight::getLightAmbient)
  .def("setLightDiffuse",&ShadowLight::setLightDiffuse)
  .def("getLightDiffuse",&ShadowLight::getLightDiffuse)
  .def("setLightSpecular",&ShadowLight::setLightSpecular)
  .def("getLightSpecular",&ShadowLight::getLightSpecular)
  .def("clear",&ShadowLight::clear)
  .def("softShadow",static_cast<int(ShadowLight::*)()const>(&ShadowLight::softShadow))
  .def("softShadow",static_cast<void(ShadowLight::*)(int)>(&ShadowLight::softShadow))
  .def("autoAdjust",static_cast<bool(ShadowLight::*)()const>(&ShadowLight::autoAdjust))
  .def("autoAdjust",static_cast<void(ShadowLight::*)(bool)>(&ShadowLight::autoAdjust))
  .def("lightSz",static_cast<int(ShadowLight::*)()const>(&ShadowLight::lightSz))
  .def("lightSz",static_cast<void(ShadowLight::*)(int)>(&ShadowLight::lightSz))
  .def("bias",static_cast<GLfloat(ShadowLight::*)()const>(&ShadowLight::bias))
  .def("bias",static_cast<void(ShadowLight::*)(GLfloat)>(&ShadowLight::bias))
  .def("nrLight",&ShadowLight::nrLight)
  .def("hasShadow",&ShadowLight::hasShadow)
  .def("renderShadow",&ShadowLight::renderShadow)
  .def("drawPovray",&ShadowLight::drawPovray)
  .def("begin",&ShadowLight::begin)
  .def("setupLightMaterial",&ShadowLight::setupLightMaterial)
  .def("end",&ShadowLight::end);
}
void initMakeFunctions(py::module& m) {
  m.def("makeSquare",&makeSquare);
  m.def("makeCircle",&makeCircle);
  m.def("makeGridLine",&makeGridLine);
  m.def("makeGrid",&makeGrid);
  m.def("makeBox",&makeBox);
  m.def("makeSphericalBox",&makeSphericalBox);
  m.def("makeCapsule",&makeCapsule);
  m.def("makeSphere",&makeSphere);
  m.def("makeTriMesh",&makeTriMesh);
  m.def("makeWires",&makeWires);

  m.def("drawTexture",&drawTexture);
  m.def("drawChecker",&drawChecker);
  m.def("drawGrid",&drawGrid);
  m.def("getWhiteTexture",&getWhiteTexture);
}
//plugins
void initCameraExportPlugin(py::module& m) {
  py::class_<CameraExportPlugin,Plugin,
  std::shared_ptr<CameraExportPlugin>>(m,"CameraExportPlugin")
  .def(py::init<int,int,const std::string&>())
  .def("saveCamera",&CameraExportPlugin::saveCamera)
  .def("loadCamera",&CameraExportPlugin::loadCamera);
}
void initCaptureGIFPlugin(py::module& m) {
  py::class_<CaptureGIFPlugin,Plugin,
  std::shared_ptr<CaptureGIFPlugin>>(m,"CaptureGIFPlugin")
  .def(py::init<int,const std::string&,int,bool>())
  .def("getScreenshot",[&](std::shared_ptr<CaptureGIFPlugin> plugin)->std::tuple<int,int,std::vector<unsigned char>> {
    int w,h;
    std::vector<unsigned char> data;
    plugin->getScreenshot(w,h,data);
    return std::make_tuple(w,h,data);
  })
  .def("takeScreenshot",&CaptureGIFPlugin::takeScreenshot)
  .def("startRecording",&CaptureGIFPlugin::startRecording)
  .def("addFrame",&CaptureGIFPlugin::addFrame)
  .def("stopRecording",&CaptureGIFPlugin::stopRecording)
  .def("recording",&CaptureGIFPlugin::recording);
}
void initCaptureMPEG2Plugin(py::module& m) {
  py::class_<CaptureMPEG2Plugin,Plugin,
  std::shared_ptr<CaptureMPEG2Plugin>>(m,"CaptureMPEG2Plugin")
  .def(py::init<int,const std::string&,int>())
  .def("startRecording",&CaptureMPEG2Plugin::startRecording)
  .def("addFrame",&CaptureMPEG2Plugin::addFrame)
  .def("stopRecording",&CaptureMPEG2Plugin::stopRecording)
  .def("recording",&CaptureMPEG2Plugin::recording);
}
void initImGuiPlugin(py::module& m) {
  py::class_<ImGuiPlugin,Plugin,
  std::shared_ptr<ImGuiPlugin>>(m,"ImGuiPlugin")
  .def(py::init<std::function<void()>>());
  initImgui(m);
}
//extra components
void initMeshShape(py::module& m) {
  py::class_<MeshShape,Shape,
  std::shared_ptr<MeshShape>>(m,"MeshShape")
  .def(py::init())
  .def(py::init<const std::vector<GLfloat>&,const std::vector<GLuint>&,GLenum>())
  .def("addVertex",[](std::shared_ptr<MeshShape> m,const Eigen::Matrix<GLfloat,-1,1>& v) {
    m->addVertex(v);
  })
  .def("addVertex",[](std::shared_ptr<MeshShape> m,const Eigen::Matrix<GLfloat,-1,1>& v,const Eigen::Matrix<GLfloat,-1,1>& tc) {
    m->addVertex(v,&tc);
  })
  .def("addIndex",&MeshShape::addIndex)
  .def("addIndexSingle",&MeshShape::addIndexSingle)
  .def("setMode",&MeshShape::setMode)
  .def("nrVertex",&MeshShape::nrVertex)
  .def("nrIndex",&MeshShape::nrIndex)
  .def("clear",&MeshShape::clear)
  .def("clearIndex",&MeshShape::clearIndex)
  .def("computeNormals",&MeshShape::computeNormals)
  .def("setNormal",&MeshShape::setNormal)
  .def("getNormal",&MeshShape::getNormal)
  .def("setVertex",&MeshShape::setVertex)
  .def("setVertices",&MeshShape::setVertices)
  .def("getVertex",&MeshShape::getVertex)
  .def("setTexcoord",&MeshShape::setTexcoord)
  .def("getTexcoord",&MeshShape::getTexcoord)
  .def("getIndex",&MeshShape::getIndex)
  .def("debugWriteObj",&MeshShape::debugWriteObj)
  .def("getTextureDiffuse",&MeshShape::getTextureDiffuse)
  .def("getTextureSpecular",&MeshShape::getTextureSpecular)
  .def("getMaterial",&MeshShape::getMaterial)
  .def("setMaterial",&MeshShape::setMaterial);
}
void initArrowShape(py::module& m) {
  py::class_<ArrowShape,MeshShape,
  std::shared_ptr<ArrowShape>>(m,"ArrowShape")
  .def(py::init<GLfloat,GLfloat,GLfloat,int>())
  .def("setArrow",&ArrowShape::setArrow);
}
void initBezierCurveShape(py::module& m) {
  py::class_<BezierCurveShape,MeshShape,
  std::shared_ptr<BezierCurveShape>>(m,"BezierCurveShape")
  .def(py::init<GLfloat,bool,int>())
  .def("getControlPoint",[](std::shared_ptr<BezierCurveShape> shape,int i)->Eigen::Matrix<GLfloat,3,1> {
    return shape->getControlPoint(i);
  })
  .def("setControlPoint",[](std::shared_ptr<BezierCurveShape> shape,int i,const Eigen::Matrix<GLfloat,3,1>& p) {
    shape->getControlPoint(i)=p;
  })
  .def("addControlPoint",[](std::shared_ptr<BezierCurveShape> shape,const Eigen::Matrix<GLfloat,-1,1>& points) {
    shape->addControlPoint(points);
  })
  .def("clearControlPoint",&BezierCurveShape::clearControlPoint);
}
void initCellShape(py::module& m) {
  py::class_<CellShape,MeshShape,
  std::shared_ptr<CellShape>>(m,"CellShape")
  .def(py::init())
  .def(py::init<const std::vector<Eigen::Matrix<int,3,1>>&,GLfloat,bool>())
  .def("reset",&CellShape::reset);
}
void initCompositeShape(py::module& m) {
  py::class_<CompositeShape,Shape,
  std::shared_ptr<CompositeShape>>(m,"CompositeShape")
  .def(py::init())
  .def("addShape",&CompositeShape::addShape)
  .def("updateChild",&CompositeShape::updateChild)
  .def("getChild",&CompositeShape::getChild)
  .def("contain",&CompositeShape::contain)
  .def("removeChild",&CompositeShape::removeChild)
  .def("numChildren",&CompositeShape::numChildren);
}
void initBox2DShape(py::module& m) {
  py::class_<Box2DShape,CompositeShape,
  std::shared_ptr<Box2DShape>>(m,"Box2DShape")
  .def(py::init())
  .def("setLocalTransform",&Box2DShape::setLocalTransform);
}
void initBullet3DShape(py::module& m) {
  py::class_<Bullet3DShape,CompositeShape,
  std::shared_ptr<Bullet3DShape>>(m,"Bullet3DShape")
  .def(py::init())
  .def("setLocalTransform",&Bullet3DShape::setLocalTransform)
  .def("setLocalTranslate",&Bullet3DShape::setLocalTranslate)
  .def("setLocalRotate",&Bullet3DShape::setLocalRotate)
  .def("setLocalTransform",&Bullet3DShape::setLocalTransform);
}
void initLowDimensionalMeshShape(py::module& m) {
  py::class_<LowDimensionalMeshShape,Shape,
  std::shared_ptr<LowDimensionalMeshShape>>(m,"LowDimensionalMeshShape")
  .def(py::init<std::shared_ptr<MeshShape>>())
  .def("setLowToHighDimensionalMapping",&LowDimensionalMeshShape::setLowToHighDimensionalMapping)
  .def("updateHighDimensionalMapping",&LowDimensionalMeshShape::updateHighDimensionalMapping);
}
void initSkinnedMeshShape(py::module& m) {
  py::class_<SkinnedMeshShape,Bullet3DShape,
  std::shared_ptr<SkinnedMeshShape>>(m,"SkinnedMeshShape")
  .def(py::init<const std::string&>())
  .def("write",&SkinnedMeshShape::write)
  .def("setAnimatedFrame",&SkinnedMeshShape::setAnimatedFrame)
  .def("getBoneTransforms",&SkinnedMeshShape::getBoneTransforms)
  .def("getBoneId",static_cast<Eigen::Matrix<GLint,4,-1>(SkinnedMeshShape::*)(int)const>(&SkinnedMeshShape::getBoneId))
  .def("getBoneWeight",&SkinnedMeshShape::getBoneWeight)
  .def("setBoneWeight",&SkinnedMeshShape::setBoneWeight)
  .def("getMeshRef",&SkinnedMeshShape::getMeshRef)
  .def("getMesh",&SkinnedMeshShape::getMesh)
  .def("duration",&SkinnedMeshShape::duration)
  .def("nrAnimation",&SkinnedMeshShape::nrAnimation);
}
void initTerrainShape(py::module& m) {
  py::class_<TerrainShape,MeshShape,
  std::shared_ptr<TerrainShape>>(m,"TerrainShape")
  .def(py::init<const Eigen::Matrix<GLfloat,-1,-1>&,int,
       const Eigen::Matrix<GLfloat,3,1>&,
       const Eigen::Matrix<GLfloat,2,1>&>())
  .def(py::init<std::function<GLfloat(GLfloat,GLfloat)>,int,
       const Eigen::Matrix<GLfloat,6,1>&,GLfloat,
       const Eigen::Matrix<GLfloat,2,1>&>());
}
void initFirstPersonCameraManipulator(py::module& m) {
  py::class_<FirstPersonCameraManipulator,CameraManipulator,
  std::shared_ptr<FirstPersonCameraManipulator>>(m,"FirstPersonCameraManipulator")
  .def(py::init<std::shared_ptr<Camera3D>>())
  .def("isSpeedMode",&FirstPersonCameraManipulator::isSpeedMode);
}
void initTrackballCameraManipulator(py::module& m) {
  py::class_<TrackballCameraManipulator,CameraManipulator,
  std::shared_ptr<TrackballCameraManipulator>>(m,"TrackballCameraManipulator")
  .def(py::init<std::shared_ptr<Camera3D>>());
}
PYBIND11_MODULE(pyTinyVisualizer, m) {
  //constants
  initDefineGLConstants(m);
  initDefineGLFWConstants(m);
  //essential components
  initGLFWwindowPtr(m);
  initSceneNode(m);
  initTexture(m);
  initPovray(m);
  initShape(m);
  initCamera(m);
  initCameraManipulator(m);
  initCamera2D(m);
  initCamera3D(m);
  initPlugin(m);
  initDrawer(m);
  initMultiDrawer(m);
  initShadowAndLight(m);
  initMakeFunctions(m);
  //plugins
  initCameraExportPlugin(m);
  initCaptureGIFPlugin(m);
  initCaptureMPEG2Plugin(m);
  initImGuiPlugin(m);
  //extra components
  initMeshShape(m);
  initArrowShape(m);
  initBezierCurveShape(m);
  initCellShape(m);
  initCompositeShape(m);
  initBox2DShape(m);
  initBullet3DShape(m);
  initLowDimensionalMeshShape(m);
  initSkinnedMeshShape(m);
  initTerrainShape(m);
  initFirstPersonCameraManipulator(m);
  initTrackballCameraManipulator(m);
}
