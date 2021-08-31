#include "FBO.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

namespace DRAWER {
//FBO
FBO::FBO(int width,int height,GLenum formatColor,GLenum formatDepth):_formatColor(formatColor),_formatDepth(formatDepth) {
  reset(width,height);
}
FBO::FBO(const FBO& other):_formatColor(other._formatColor),_formatDepth(other._formatDepth) {
  reset(other.width(),other.height());
}
FBO& FBO::operator=(const FBO& other) {
  clear();
  _formatColor=other._formatColor;
  _formatDepth=other._formatDepth;
  reset(other.width(),other.height());
  return *this;
}
FBO::~FBO() {
  clear();
}
void FBO::screenQuad(GLfloat minx,GLfloat miny,GLfloat maxx,GLfloat maxy,bool colorTex) {
  glBegin(GL_QUADS);
  if(colorTex)
    glColor3f(0,0,0);
  else glColor3f(1,1,1);
  glTexCoord2f(0,0);
  glVertex2f(minx,miny);

  if(colorTex)
    glColor3f(1,0,0);
  else glColor3f(1,1,1);
  glTexCoord2f(1,0);
  glVertex2f(maxx,miny);

  if(colorTex)
    glColor3f(1,1,0);
  else glColor3f(1,1,1);
  glTexCoord2f(1,1);
  glVertex2f(maxx,maxy);

  if(colorTex)
    glColor3f(0,1,0);
  else glColor3f(1,1,1);
  glTexCoord2f(0,1);
  glVertex2f(minx,maxy);
  glEnd();
}
void FBO::drawScreenQuad(GLfloat minx,GLfloat miny,GLfloat maxx,GLfloat maxy,bool colorTex) const {
  drawScreenQuad([&]() {
    screenQuad(minx,miny,maxx,maxy,colorTex);
  });
}
void FBO::drawScreenQuad(std::function<void()> func) const {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  func();

  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
}
void FBO::begin() const {
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glGetIntegerv(GL_VIEWPORT,const_cast<GLint*>(_vp));
  glViewport(0,0,width(),height());
  glScissor(0,0,width(),height());
}
void FBO::end() const {
  glViewport(_vp[0],_vp[1],_vp[2],_vp[3]);
  glScissor(_vp[0],_vp[1],_vp[2],_vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}
GLfloat FBO::getPixel00() const {
  begin();
  GLfloat ret;
  ASSERT_MSGV(width()==1&&height()==1,"Cannot fetch score if width(%d)!=1 or height(%d)!=1",width(),height())
  glReadPixels(0,0,width(),height(),GL_RED,GL_FLOAT,&ret);
  end();
  return ret;
}
const Texture& FBO::getRBO() const {
  return *_rbo;
}
std::shared_ptr<Texture> FBO::getRBORef() const {
  return _rbo;
}
std::vector<std::uint8_t> FBO::read() const {
  begin();
  std::vector<std::uint8_t> data(width()*height()*4);
  glReadBuffer(GL_COLOR_ATTACHMENT0);
  glReadPixels(0,0,width(),height(),GL_RGBA,GL_UNSIGNED_BYTE,&data[0]);
  end();
  return data;
}
void FBO::saveImage(const std::string& path) const {
  std::vector<std::uint8_t> data=read();
  stbi_write_png(path.c_str(),width(),height(),4,&data[0],width()*4);
}
int FBO::width() const {
  return _rbo->width();
}
int FBO::height() const {
  return _rbo->height();
}
void FBO::reset(int width,int height) {
  _rbo.reset(new Texture(width,height,_formatColor));

  ASSERT_MSG(glad_glGenRenderbuffers,"FBO not supported!")
  if(_formatDepth!=GL_NONE) {
    glGenRenderbuffers(1,&_dbo);
    glBindRenderbuffer(GL_RENDERBUFFER,_dbo);
    glRenderbufferStorage(GL_RENDERBUFFER,_formatDepth,width,height);
    glBindRenderbuffer(GL_RENDERBUFFER,0);
  } else _dbo=-1;

  ASSERT_MSG(glad_glGenFramebuffers,"FBO not supported!")
  glGenFramebuffers(1,&_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,_rbo->id(),0);
  if(_formatDepth!=GL_NONE)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,_dbo);
  ASSERT_MSGV(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE,"Framebuffer incomplete: status=%d!",int(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}
void FBO::clear() {
  glDeleteFramebuffers(1,&_fbo);
  if(_formatDepth!=GL_NONE)
    glDeleteRenderbuffers(1,&_dbo);
}
//FBOShadow
FBOShadow::FBOShadow(int res,GLenum formatDepth):_formatDepth(formatDepth) {
  reset(res);
}
FBOShadow::FBOShadow(const FBOShadow& other):_formatDepth(other._formatDepth) {
  reset(other.width());
}
FBOShadow& FBOShadow::operator=(const FBOShadow& other) {
  clear();
  _formatDepth=other._formatDepth;
  reset(other.width());
  return *this;
}
FBOShadow::~FBOShadow() {
  clear();
}
void FBOShadow::begin(int d) const {
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_POSITIVE_X+d,_dbo,0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glGetIntegerv(GL_VIEWPORT,const_cast<GLint*>(_vp));
  glViewport(0,0,width(),height());
  glScissor(0,0,width(),height());
}
void FBOShadow::end() const {
  glViewport(_vp[0],_vp[1],_vp[2],_vp[3]);
  glScissor(_vp[0],_vp[1],_vp[2],_vp[3]);
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}
void FBOShadow::beginShadow() const {
  glEnable(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP,_dbo);
}
void FBOShadow::endShadow() const {
  glBindTexture(GL_TEXTURE_CUBE_MAP,0);
  glDisable(GL_TEXTURE_CUBE_MAP);
}
std::vector<std::uint8_t> FBOShadow::read(int d) const {
  begin(d);
  std::vector<std::uint8_t> data(width()*height());
  glReadPixels(0,0,width(),height(),GL_DEPTH_COMPONENT,GL_UNSIGNED_BYTE,&data[0]);
  end();
  return data;
}
void FBOShadow::saveImage(int d,const std::string& path) const {
  std::vector<std::uint8_t> data=read(d);
  std::vector<std::uint8_t> dataRGBA(data.size()*4,255);
  for(int i=0; i<(int)data.size(); i++)
    dataRGBA[i*4+0]=dataRGBA[i*4+1]=dataRGBA[i*4+2]=data[i];
  stbi_write_png(path.c_str(),width(),height(),4,&dataRGBA[0],width()*4);
}
int FBOShadow::width() const {
  GLint ret;
  beginShadow();
  glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,GL_TEXTURE_WIDTH,&ret);
  endShadow();
  return ret;
}
int FBOShadow::height() const {
  GLint ret;
  beginShadow();
  glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X,0,GL_TEXTURE_HEIGHT,&ret);
  endShadow();
  return ret;
}
void FBOShadow::reset(int res) {
  glGenTextures(1,&_dbo);
  beginShadow();
  for(int d=0; d<6; d++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+d,0,GL_DEPTH_COMPONENT32,res,res,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);
  }
  endShadow();

  glGenFramebuffers(1,&_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER,_fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_CUBE_MAP_POSITIVE_X,_dbo,0);
  ASSERT_MSGV(glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE,"Framebuffer incomplete: status=%d!",int(glCheckFramebufferStatus(GL_FRAMEBUFFER)));
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}
void FBOShadow::clear() {
  glDeleteFramebuffers(1,&_fbo);
  glDeleteTextures(1,&_dbo);
}
//FBOPingPong
FBOPingPong::FBOPingPong(int levelMin,int levelMax,GLenum formatColor,GLenum formatDepth) {
  for(int i=levelMin; i<=levelMax; i++)
    _fbos.push_back(FBO(1<<i,1<<i,formatColor,i==levelMax?formatDepth:GL_NONE));
}
void FBOPingPong::saveImage(int i,const std::string& path) const {
  ASSERT_MSGV(i>=0 && i<(int)_fbos.size(),"Invalid FBO PingPong index (0<=%d<%d)!",i,(int)_fbos.size())
  _fbos[i].saveImage(path);
}
const FBO& FBOPingPong::getFBO(int i) const {
  ASSERT_MSGV(i>=0 && i<(int)_fbos.size(),"Invalid FBO PingPong index (0<=%d<%d)!",i,(int)_fbos.size())
  return _fbos[i];
}
int FBOPingPong::nrFBO() const {
  return (int)_fbos.size();
}
void FBOPingPong::begin() {
  _fbos.back().begin();
}
void FBOPingPong::end() {
  _fbos.back().end();
  for(int i=_fbos.size()-2; i>=0; i--) {
    glActiveTexture(GL_TEXTURE0);
    _fbos[i+1].getRBO().begin();
    glActiveTexture(GL_TEXTURE1);
    _fbos[i].begin();
    _fbos[i].drawScreenQuad();
    _fbos[i].end();
    _fbos[i+1].getRBO().end();
    glActiveTexture(GL_TEXTURE0);
  }
}
}
