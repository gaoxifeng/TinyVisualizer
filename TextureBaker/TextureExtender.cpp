#include "TextureExtender.h"
#include <unordered_set>
#include <iostream>

namespace DRAWER {
TextureExtender::TextureExtender(std::shared_ptr<Texture> texture):_texture(texture) {}
TextureExtender::~TextureExtender() {}
NearestTextureExtender::NearestTextureExtender(std::shared_ptr<Texture> texture):TextureExtender(texture) {}
void NearestTextureExtender::extend(const std::vector<TexelIntersect>& intersect) {
  Texture::TextureCPUData& data=_texture->loadCPUData();
  std::vector<unsigned char> inside(data._width*data._height,false);
  //label inside
  #pragma omp parallel for
  for(int i=0; i<(int)intersect.size(); i++)
    if(std::get<0>(intersect[i])>=0)
      inside[std::get<0>(intersect[i])]=true;
  //label boundary
  std::unordered_set<int> bd,bdLast;
  for(unsigned int hh=0; hh<data._height; hh++)
    for(unsigned int ww=0; ww<data._width; ww++) {
      int off=ww+data._width*hh;
      if(!inside[off]) {
        bool boundary=false;
        if(ww>0 && inside[off-1])
          boundary=true;
        if(ww<data._width-1 && inside[off+1])
          boundary=true;
        if(hh>0 && inside[off-data._width])
          boundary=true;
        if(hh<data._height-1 && inside[off+data._width])
          boundary=true;
        if(boundary)
          bdLast.insert(off);
      }
    }
  //propagate
  std::vector<unsigned char> insideLast;
  std::function<Eigen::Matrix<GLdouble,4,1>(int)> getColor=[&](int off)->Eigen::Matrix<GLdouble,4,1> {
    return Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+off*4).cast<GLdouble>()/255;
  };
  while(!bdLast.empty()) {
    bd.clear();
    insideLast=inside;
    for(int off:bdLast) {
      int nrColor=0;
      unsigned int ww=off%data._width;
      unsigned int hh=off/data._width;
      Eigen::Matrix<GLdouble,4,1> color(0,0,0,0);
      if(ww>0) {
        if(insideLast[off-1]) {
          color+=getColor(off-1);
          nrColor++;
        } else bd.insert(off-1);
      }
      if(ww<data._width-1) {
        if(insideLast[off+1]) {
          color+=getColor(off+1);
          nrColor++;
        } else bd.insert(off+1);
      }
      if(hh>0) {
        if(insideLast[off-data._width]) {
          color+=getColor(off-data._width);
          nrColor++;
        } else bd.insert(off-data._width);
      }
      if(hh<data._height-1) {
        if(insideLast[off+data._width]) {
          color+=getColor(off+data._width);
          nrColor++;
        } else bd.insert(off+data._width);
      }
      Eigen::Map<Eigen::Matrix<unsigned char,4,1>> c(data._data+off*4);
      c=((color/nrColor)*255).cast<unsigned char>();
      inside[off]=true;
    }
    bd.swap(bdLast);
  }
  _texture->syncGPUData();
}
LaplaceTextureExtender::LaplaceTextureExtender(std::shared_ptr<Texture> texture):NearestTextureExtender(texture) {}
void LaplaceTextureExtender::extend(const std::vector<TexelIntersect>& intersect) {
  NearestTextureExtender::extend(intersect);

  Texture::TextureCPUData& data=_texture->loadCPUData();
  std::vector<unsigned char> inside(data._width*data._height,false);
  //label inside
  #pragma omp parallel for
  for(int i=0; i<(int)intersect.size(); i++)
    if(std::get<0>(intersect[i])>=0)
      inside[std::get<0>(intersect[i])]=true;
  //red-black laplace smoothing
  std::function<Eigen::Matrix<GLdouble,4,1>(int)> getColor=[&](int off)->Eigen::Matrix<GLdouble,4,1> {
    return Eigen::Map<Eigen::Matrix<unsigned char,4,1>>(data._data+off*4).cast<GLdouble>()/255;
  };
  bool more=true;
  for(int iter=0; more; iter++) {
    more=false;
    std::cout << "Laplace iter=" << iter << std::endl;
    for(unsigned int pass=0; pass<2; pass++) {
      #pragma omp parallel for
      for(unsigned int hh=0; hh<data._height; hh++)
        for(unsigned int ww=0; ww<data._width; ww++) {
          int off=ww+data._width*hh;
          if(!inside[off] && ((ww+hh)%2)==pass) {
            Eigen::Matrix<GLdouble,4,1> color(0,0,0,0);
            int nrColor=0;
            if(ww>0) {
              color+=getColor(off-1);
              nrColor++;
            }
            if(ww<data._width-1) {
              color+=getColor(off+1);
              nrColor++;
            }
            if(hh>0) {
              color+=getColor(off-data._width);
              nrColor++;
            }
            if(hh<data._height-1) {
              color+=getColor(off+data._width);
              nrColor++;
            }
            Eigen::Map<Eigen::Matrix<unsigned char,4,1>> c(data._data+off*4);
            Eigen::Matrix<unsigned char,4,1> cNew=((color/nrColor)*255).cast<unsigned char>();
            bool diff=c!=cNew;
            //#pragma omp atomic
            more |= diff;
            c=cNew;
          }
        }
    }
  }
  _texture->syncGPUData();
}
}
