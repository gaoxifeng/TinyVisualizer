#include "SceneStructure.h"
#include "Drawer.h"

namespace DRAWER {
SceneNode::SceneNode():_nrShape(0) {}
SceneNode::SceneNode(const Eigen::Matrix<int,3,1>& loc,int sz):_sz(sz),_nrShape(0),_loc(loc) {}
std::shared_ptr<SceneNode> SceneNode::update(std::shared_ptr<SceneNode> root,std::shared_ptr<Shape> s) {
  std::shared_ptr<Shape> toBeAdjusted;
  if(root && !s) {
    root->update(toBeAdjusted);
    if(root->empty())
      root=NULL;
  }
  if(s) {
    if(!root || !root->tryAssign(s,s->getBB()))
      toBeAdjusted=insertList(toBeAdjusted,s);
  }
  //no need to continue, everything in root
  if(!toBeAdjusted)
    return root;

  int sz;
  Eigen::Matrix<int,3,1> loc;
  if(root) {
    sz=root->_sz;
    loc=root->_loc;
  } else {
    sz=1;
    ASSERT(s!=NULL || toBeAdjusted!=NULL)
    if(s)
      loc=s->getBB().segment<3>(0).cast<int>();
    else loc=toBeAdjusted->getBB().segment<3>(0).cast<int>();
  }
  for(std::shared_ptr<Shape> sAdj=toBeAdjusted; sAdj; sAdj=sAdj->_next) {
    Eigen::Matrix<GLfloat,6,1> bb=sAdj->getBB();
    while(!contain(loc,sz,bb)) {
      loc.array()-=sz;
      sz<<=2;
    }
  }
  ASSERT(!root || contain(loc,sz,*root))
  std::shared_ptr<SceneNode> newRoot(new SceneNode(loc,sz));
  if(root)
    newRoot->tryAssign(root);
  std::shared_ptr<Shape> tmp=toBeAdjusted;
  while(tmp) {
    std::shared_ptr<Shape> next=tmp->_next;
    tmp->_next=NULL;
    ASSERT(newRoot->tryAssign(tmp,tmp->getBB()))
    tmp=next;
  }
  return newRoot;
}
std::shared_ptr<SceneNode> SceneNode::remove(std::shared_ptr<SceneNode> root,std::shared_ptr<Shape> s) {
  ASSERT(root->remove(s))
  if(root->empty())
    return NULL;
  return root;
}
void SceneNode::update(std::shared_ptr<Shape>& toBeAdjusted) {
  std::shared_ptr<Shape> toBeAdjustedInner,tmp;
  //recursively check for incorrectly assigned stuff
  for(std::shared_ptr<SceneNode> c:_children)
    if(c)
      c->update(toBeAdjustedInner);
  _nrShape-=sizeList(toBeAdjustedInner);
  //check whether incorrect stuff is still contained in this node
  tmp=toBeAdjustedInner;
  toBeAdjustedInner=NULL;
  while(tmp) {
    std::shared_ptr<Shape> next=tmp->_next;
    tmp->_next=NULL;
    if(!tryAssign(tmp,tmp->getBB()))
      toBeAdjusted=insertList(toBeAdjusted,tmp);
    tmp=next;
  }
  //check the shape that is associated with this node
  tmp=_shapes;
  _nrShape-=sizeList(tmp);
  _shapes=NULL;
  while(tmp) {
    std::shared_ptr<Shape> next=tmp->_next;
    tmp->_next=NULL;
    if(!tryAssign(tmp,tmp->getBB()))
      toBeAdjusted=insertList(toBeAdjusted,tmp);
    tmp=next;
  }
  for(std::shared_ptr<SceneNode>& c:_children)
    if(c && c->empty())
      c=NULL;
}
bool SceneNode::remove(std::shared_ptr<Shape> s) {
  if(_shapes==s) {
    _shapes=_shapes->_next;
    _nrShape--;
    return true;
  } else if(_shapes) {
    for(std::shared_ptr<Shape> last=_shapes,ss=_shapes->_next; ss; last=ss,ss=ss->_next)
      if(ss==s) {
        last->_next=ss->_next;
        _nrShape--;
        return true;
      }
  }
  for(int i=0; i<8; i++)
    if(_children[i] && _children[i]->remove(s)) {
      if(_children[i]->empty())
        _children[i]=NULL;
      _nrShape--;
      return true;
    }
  return false;
}
void SceneNode::tryAssign(std::shared_ptr<SceneNode> s) {
  ASSERT_MSG(_sz>1,"A SceneNode cannot have _sz<1!")
  for(int i=0; i<8; i++) {
    Eigen::Matrix<int,3,1> loc=_loc;
    if(i&1)
      loc[0]+=_sz>>1;
    if(i&2)
      loc[1]+=_sz>>1;
    if(i&4)
      loc[2]+=_sz>>1;
    if(loc==s->_loc && (_sz>>1)==s->_sz) {
      if(!_children[i])
        _children[i]=s;
      else _children[i]->merge(s);
      _nrShape+=s->_nrShape;
      return;
    } else if(contain(loc,_sz>>1,*s)) {
      if(!_children[i])
        _children[i].reset(new SceneNode(loc,_sz>>1));
      _children[i]->tryAssign(s);
      _nrShape+=s->_nrShape;
      return;
    }
  }
  ASSERT_MSG(false,"Failed assigning nodes!")
}
bool SceneNode::tryAssign(std::shared_ptr<Shape> s,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(contain(*this,bb)) {
    for(int i=0; i<8; i++) {
      Eigen::Matrix<int,3,1> loc=_loc;
      if(i&1)
        loc[0]+=_sz>>1;
      if(i&2)
        loc[1]+=_sz>>1;
      if(i&4)
        loc[2]+=_sz>>1;
      if(contain(loc,_sz>>1,bb)) {
        if(!_children[i])
          _children[i].reset(new SceneNode(loc,_sz>>1));
        ASSERT(_children[i]->tryAssign(s,bb))
        _nrShape++;
        return true;
      }
    }
    _shapes=insertList(_shapes,s);
    _nrShape++;
    return true;
  } else return false;
}
void SceneNode::merge(std::shared_ptr<SceneNode> s) {
  _shapes=insertList(_shapes,s->_shapes);
  for(int i=0; i<8; i++)
    if(_children[i] && s->_children[i])
      _children[i]->merge(s->_children[i]);
    else if(s->_children[i])
      _children[i]=s->_children[i];
  _nrShape+=s->_nrShape;
}
int SceneNode::nrShape() const {
  int ret=sizeList(_shapes);
  for(int i=0; i<8; i++)
    if(_children[i])
      ret+=_children[i]->nrShape();
  return ret;
}
int SceneNode::nrChildren() const {
  int ret=0;
  for(int i=0; i<8; i++)
    if(_children[i])
      ret++;
  return ret;
}
bool SceneNode::isLeaf() const {
  for(int i=0; i<8; i++)
    if(_children[i])
      return false;
  return true;
}
bool SceneNode::empty() const {
  //return isLeaf() && _shapes.empty();
  return _nrShape==0;
}
void SceneNode::check() const {
  visit([&](const SceneNode& n) {
    ASSERT_MSGV(n.nrShape()==n._nrShape,"n.nrShape()(%d)!=n._nrShape(%d)",n.nrShape(),n._nrShape)
    for(std::shared_ptr<Shape> s=_shapes; s; s=s->_next) {
      ASSERT_MSG(contain(*this,s->getBB()),"Shape not contained in node!")
    }
    for(int i=0; i<8; i++) {
      Eigen::Matrix<int,3,1> loc=_loc;
      if(i&1)
        loc[0]+=_sz>>1;
      if(i&2)
        loc[1]+=_sz>>1;
      if(i&4)
        loc[2]+=_sz>>1;
      for(std::shared_ptr<Shape> s=_shapes; s; s=s->_next) {
        ASSERT_MSG(!contain(loc,_sz>>1,s->getBB()),"Shape should descend to child!")
      }
    }
    for(std::shared_ptr<SceneNode> c:_children)
      if(c) {
        ASSERT_MSG(!c->empty(),"Found empty child!")
      }
    return true;
  });
}
Eigen::Matrix<GLfloat,6,1> SceneNode::getBB() const {
  Eigen::Matrix<GLfloat,6,1> ret=resetBB();
  for(int i=0; i<8; i++)
    if(_children[i])
      ret=unionBB(ret,_children[i]->getBB());
  for(std::shared_ptr<Shape> s=_shapes; s; s=s->_next)
    ret=unionBB(ret,s->getBB());
  return ret;
}
void SceneNode::visit(std::function<bool(const SceneNode&)> f) const {
  if(!f(*this))
    return;
  else {
    for(std::shared_ptr<SceneNode> s:_children)
      if(s)
        s->visit(f);
  }
}
bool SceneNode::visit(std::function<bool(std::shared_ptr<Shape>)> f) const {
  for(std::shared_ptr<Shape> s=_shapes; s; s=s->_next)
    if(!f(s))
      return false;
  for(std::shared_ptr<SceneNode> s:_children)
    if(s)
      if(!s->visit(f))
        return false;
  return true;
}
bool SceneNode::rayIntersect(const Eigen::Matrix<GLfloat,6,1>& ray,std::shared_ptr<Shape>& IShape,GLfloat& IAlpha) const {
  //bounding box
  if(!rayIntersectBB(ray,IAlpha,getBB()))
    return false;
  //nodes
  bool ret=false;
  for(std::shared_ptr<Shape> s=_shapes; s; s=s->_next)
    if(s->rayIntersect(ray,IAlpha)) {
      IShape=s;
      ret=true;
    }
  //children
  for(int i=0; i<8; i++)
    if(_children[i] && _children[i]->rayIntersect(ray,IShape,IAlpha))
      ret=true;
  return ret;
}
void SceneNode::draw(std::function<void(std::shared_ptr<Shape>)> f,const Eigen::Matrix<GLfloat,-1,1>* viewFrustum) const {
  visit([&](const SceneNode& node) {
    //view frustum culling
    if(viewFrustum) {
      Eigen::Matrix<GLfloat,3,1> lb=node._loc.cast<GLfloat>()*_RES;
      Eigen::Matrix<GLfloat,3,1> ub=(lb.cast<GLfloat>().array()+node._sz*_RES).matrix();
      Eigen::Matrix<GLfloat,3,1> ctr=(lb+ub)/2,ext=(ub-lb)/2;
      for(int i=0; i<viewFrustum->size(); i+=4) {
        GLfloat ctrVal=viewFrustum->segment<3>(i).dot(ctr)+viewFrustum->coeff(i+3);
        GLfloat deltaVal=(viewFrustum->segment<3>(i).array()*ext.array()).abs().sum();
        if(ctrVal-deltaVal>=0)
          return false;
      }
    }
    //loop over shapes
    for(std::shared_ptr<Shape> s=node._shapes; s; s=s->_next)
      f(s);
    return true;
  });
}
bool SceneNode::contain(const Eigen::Matrix<int,3,1>& loc,int sz,const Eigen::Matrix<GLfloat,6,1>& bb) {
  if(!(bb.segment<3>(0).array()>=(loc.cast<GLfloat>()*_RES).array()).all())
    return false;
  if(!(bb.segment<3>(3).array()<=((loc.cast<GLfloat>().array()+sz)*_RES)).all())
    return false;
  return true;
}
bool SceneNode::contain(const Eigen::Matrix<int,3,1>& loc,int sz,const SceneNode& sc) {
  if(!(sc._loc.array()>=loc.array()).all())
    return false;
  if(!((sc._loc.array()+sc._sz)<=(loc.array()+sz)).all())
    return false;
  return true;
}
bool SceneNode::contain(const SceneNode& sc0,const Eigen::Matrix<GLfloat,6,1>& bb) {
  return contain(sc0._loc,sc0._sz,bb);
}
bool SceneNode::contain(const SceneNode& sc0,const SceneNode& sc) {
  return contain(sc0._loc,sc0._sz,sc);
}
std::shared_ptr<Shape> SceneNode::insertList(std::shared_ptr<Shape> s0,std::shared_ptr<Shape> sNew) {
  std::shared_ptr<Shape> sLast=sNew;
  while(sLast->_next)
    sLast=sLast->_next;
  sLast->_next=s0;
  return sNew;
}
int SceneNode::sizeList(std::shared_ptr<Shape> s) {
  int ret=0;
  for(; s; s=s->_next)
    ret++;
  return ret;
}
GLfloat SceneNode::_RES=0.1f;
}
