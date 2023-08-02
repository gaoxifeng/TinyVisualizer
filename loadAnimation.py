from utils import *
import math,random

def load_texture(tex):
    tex.loadCPUData()
    ret = np.zeros((tex.height(),tex.width(),4)).astype(np.float32)
    for c in range(4):
        ret[:,:,c] = tex.getDataChannel(c).T
    return torch.tensor(ret).cuda()

def get_bounding_box(pos):
    minC = np.min(pos,0)
    maxC = np.max(pos,0)
    return (minC,maxC)

def sample_camera(pos):
    minC,maxC = get_bounding_box(pos)
    #ctr
    ctr = (minC+maxC)/2
    #dir
    dist = np.linalg.norm(maxC-minC)/2
    dir = np.random.rand(3) * 2 - 1
    dir /= np.linalg.norm(dir)
    #up
    up = np.random.rand(3) * 2 - 1
    up = np.cross(up, dir)
    up /= np.linalg.norm(up)
    
    #create matrix
    eye = ctr + dir * dist
    mv = modelview(eye.tolist(), ctr.tolist(), up.tolist())
    
    #automatic fovy computation
    x = np.matmul(pos - eye, -dir)
    y = np.matmul(pos - eye, np.cross(up, dir))
    z = np.matmul(pos - eye, up)
    fovy = np.max(np.arctan2(np.abs(y), np.abs(x)))
    fovz = np.max(np.arctan2(np.abs(z), np.abs(x)))
    fov = max(fovy, fovz) * 360. / math.pi
    
    #automatic near-far computation
    p = projection(fov, 1., max(.001, np.min(x)), np.max(x))
    return np.matmul(p, mv)

def sample_camera_multi(poss):
    poss = [pos.cpu().numpy() for pos in poss]
    return sample_camera(np.vstack(poss))

class Animation:
    def __init__(self,path):
        self.poss = []
        self.uvs = []
        self.idxs = []
        self.texs = []
        self.bids = []
        self.bws = []
        self.shape = vis.SkinnedMeshShape(path)
        for id in range(self.shape.numChildren()):
            mesh = self.shape.getMesh(id)
            #read vertex
            pos = []
            uv = []
            for vid in range(mesh.nrVertex()):
                pos.append(mesh.getVertex(vid).T)
                uv.append(mesh.getTexcoord(vid).T)
            self.poss.append(torch.tensor(np.vstack(pos),dtype=torch.float32).cuda())
            self.uvs.append(torch.tensor(np.vstack(uv),dtype=torch.float32).cuda())
            #read index
            idx = []
            for fid in range(mesh.nrIndex()//3):
                idx.append([mesh.getIndex(fid*3+d) for d in range(3)])
            self.idxs.append(torch.tensor(np.vstack(idx),dtype=torch.int32).cuda())
            #read texture
            self.texs.append(load_texture(mesh.getTexture()))
            #read bone
            self.bids.append(torch.tensor(self.shape.getBoneId(id).T,dtype=torch.int32).cuda())
            self.bws.append(torch.tensor(self.shape.getBoneWeight(id).T,dtype=torch.float32).cuda())
        self.poss_trans = None

    def render(self,glctx, mvp, resolution=1024, enable_mip=False, max_mip_level=0, ref=True):
        return render_multi(glctx, mvp, 
                            self.poss if ref else self.poss_trans, 
                            self.idxs, self.uvs, self.texs, resolution, enable_mip, max_mip_level)

    def sample_animation(self):
        index = random.randint(0,self.shape.nrAnimation()-1)
        time = random.uniform(0,self.shape.duration(index))
        self.shape.setAnimatedFrame(index, time, False)
        self.bone_trans = torch.tensor(self.shape.getBoneTransforms(),dtype=torch.float32).cuda()
        self.bone_trans = torch.swapaxes(self.bone_trans.reshape((4,self.bone_trans.shape[1]//4,4)),1,2)
        self.calc_animated_poss()

    def calc_animated_poss(self):
        #bid: [#maxBone,#vertex]
        #bw:  [#maxBone,#vertex]
        #boneTrans: [#bone,4,4]
        self.poss_trans = []
        for pos, bid, bw in zip(self.poss, self.bids, self.bws):
            pos_trans = []
            for pid in range(pos.shape[0]):
                trans = torch.zeros((4,4)).cuda()
                for d in range(4):
                    if bid[pid,d]>=0:
                        trans += self.bone_trans[:,:,bid[pid,d]] * bw[pid,d]
                p_trans = torch.mm(trans[:3,:3],pos[pid:pid+1,:].T) + trans[:3,3:4]
                pos_trans.append(p_trans)
            pos_trans = torch.cat(pos_trans,dim=1).T
            self.poss_trans.append(pos_trans)

if __name__=='__main__':
    drawer = vis.Drawer(0,None)
    anim = Animation('char10.glb')
    anim.sample_animation()
    mvp = sample_camera_multi(anim.poss_trans)
    
    use_opengl = False
    glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    color, depth = anim.render(glctx, mvp, ref=False)
    save_image('color.png', color[0,:].cpu().numpy())
    save_image('depth.png', depth[0,:,:,2].cpu().numpy())