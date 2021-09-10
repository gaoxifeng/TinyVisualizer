%module(directors="1") pyTinyVisualizer
%include "std_shared_ptr.i"
%include "std_string.i"
%include "typemaps.i"
%include "eigen.i"
%apply int *INOUT { int* v };
%apply bool *INOUT { bool* v };
%apply float *INOUT { float* v };
%eigen_typemaps(Eigen::Matrix<GLfloat,-1,-1>)
%eigen_typemaps(Eigen::Matrix<GLfloat,-1,1>)
%eigen_typemaps(Eigen::Matrix<GLfloat,2,1>)
%eigen_typemaps(Eigen::Matrix<GLfloat,3,1>)
%{
#include <glad/gl.h>
#include <imgui.h>
#include <TinyVisualizer/DrawerUtility.h>
#include <TinyVisualizer/Shader.h>
#include <TinyVisualizer/Texture.h>
#include <TinyVisualizer/MakeTexture.h>
#include <TinyVisualizer/FBO.h>
#include <TinyVisualizer/Drawer.h>
#include <TinyVisualizer/ShadowAndLight.h>
#include <TinyVisualizer/CameraManipulator.h>
#include <TinyVisualizer/TrackballCameraManipulator.h>
#include <TinyVisualizer/FirstPersonCameraManipulator.h>
#include <TinyVisualizer/CameraExportPlugin.h>
#include <TinyVisualizer/CaptureMPEG2Plugin.h>
#include <TinyVisualizer/CaptureGIFPlugin.h>
#include <TinyVisualizer/ImGuiPlugin.h>
#include <TinyVisualizer/Camera2D.h>
#include <TinyVisualizer/Camera3D.h>
#include <TinyVisualizer/CompositeShape.h>
#include <TinyVisualizer/MeshShape.h>
#include <TinyVisualizer/LowDimensionalMeshShape.h>
#include <TinyVisualizer/MakeMesh.h>
#include <TinyVisualizer/CellShape.h>
#include <TinyVisualizer/ArrowShape.h>
#include <TinyVisualizer/TerrainShape.h>
#include <TinyVisualizer/Box2DShape.h>
#include <TinyVisualizer/Bullet3DShape.h>
#include <TinyVisualizer/VisibilityScore.h>
%}
%typedef float GLfloat;
%ignore ImGui::TextV;
%ignore ImGui::TextColoredV;
%ignore ImGui::TextDisabledV;
%ignore ImGui::TextWrappedV;
%ignore ImGui::LabelTextV;
%ignore ImGui::BulletTextV;
%ignore ImGui::TreeNodeV;
%ignore ImGui::TreeNodeExV;
%ignore ImGui::SetTooltipV;
%ignore ImGui::LogTextV;
%ignore ImGuiTextBuffer;
%include <glad/gl.h>
%include <GLFW/glfw3.h>
%include <imgui.h>
%include <TinyVisualizer/DrawerUtility.h>
%shared_ptr(DRAWER::Shader);
%include <TinyVisualizer/Shader.h>
%shared_ptr(DRAWER::Texture);
%include <TinyVisualizer/Texture.h>
%include <TinyVisualizer/MakeTexture.h>
%shared_ptr(DRAWER::FBO);
%include <TinyVisualizer/FBO.h>
%shared_ptr(DRAWER::Shape);
%shared_ptr(DRAWER::Camera);
%shared_ptr(DRAWER::Plugin);
%shared_ptr(DRAWER::Drawer);
%feature("director") PythonCallback;
%include <TinyVisualizer/Drawer.h>
%shared_ptr(DRAWER::ShadowLight);
%include <TinyVisualizer/ShadowAndLight.h>
%shared_ptr(DRAWER::CameraManipulator);
%include <TinyVisualizer/CameraManipulator.h>
%shared_ptr(DRAWER::TrackballCameraManipulator);
%include <TinyVisualizer/TrackballCameraManipulator.h>
%shared_ptr(DRAWER::FirstPersonCameraManipulator);
%include <TinyVisualizer/FirstPersonCameraManipulator.h>
%shared_ptr(DRAWER::CameraExportPlugin);
%include <TinyVisualizer/CameraExportPlugin.h>
%shared_ptr(DRAWER::CaptureMPEG2Plugin);
%include <TinyVisualizer/CaptureMPEG2Plugin.h>
%shared_ptr(DRAWER::CaptureGIFPlugin);
%include <TinyVisualizer/CaptureGIFPlugin.h>
%shared_ptr(DRAWER::ImGuiPlugin);
%include <TinyVisualizer/ImGuiPlugin.h>
%shared_ptr(DRAWER::Camera2D);
%include <TinyVisualizer/Camera2D.h>
%shared_ptr(DRAWER::Camera3D);
%include <TinyVisualizer/Camera3D.h>
%shared_ptr(DRAWER::CompositeShape);
%include <TinyVisualizer/CompositeShape.h>
%shared_ptr(DRAWER::MeshShape);
%include <TinyVisualizer/MeshShape.h>
%shared_ptr(DRAWER::LowDimensionalMeshShape);
%include <TinyVisualizer/LowDimensionalMeshShape.h>
%include <TinyVisualizer/MakeMesh.h>
%shared_ptr(DRAWER::CellShape);
%include <TinyVisualizer/CellShape.h>
%shared_ptr(DRAWER::ArrowShape);
%include <TinyVisualizer/ArrowShape.h>
%shared_ptr(DRAWER::TerrainShape);
%include <TinyVisualizer/TerrainShape.h>
%shared_ptr(DRAWER::Box2DShape);
%include <TinyVisualizer/Box2DShape.h>
%shared_ptr(DRAWER::Bullet3DShape);
%include <TinyVisualizer/Bullet3DShape.h>
%include <TinyVisualizer/VisibilityScore.h>

