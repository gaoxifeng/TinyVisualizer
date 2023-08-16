from utils import *
import math,random,imageio

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

def sample_camera(pos, dir=None, up=None, nearCoef=0.9, farCoef=1.1):
    minC,maxC = get_bounding_box(pos)
    dist = np.linalg.norm(maxC-minC)/2
    #ctr
    ctr = (minC+maxC)/2
    #dir
    if dir is None:
        dir = np.random.rand(3) * 2 - 1
        dir /= np.linalg.norm(dir)
    #up
    if up is None:
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
    p = projection(fov, 1., max(.001, np.min(x)*nearCoef), np.max(x)*farCoef)
    return np.matmul(p, mv)

def sample_camera_multi(poss, dir=None, up=None, nearCoef=0.9, farCoef=1.1):
    poss = [pos.cpu().numpy() for pos in poss]
    return sample_camera(np.vstack(poss), dir=dir, up=up, nearCoef=nearCoef, farCoef=farCoef)

class Animation:
    def __init__(self, path):
        self.poss = []
        self.uvs = []
        self.idxs = []
        self.texs = []
        self.bids = []
        self.bws = []
        self.zero_trans = torch.zeros((4,4)).cuda()
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
            self.texs.append(load_texture(mesh.getTextureDiffuse()))
            #read bone
            self.bids.append(torch.tensor(self.shape.getBoneId(id).T,dtype=torch.int32).cuda())
            self.bws.append(torch.tensor(self.shape.getBoneWeight(id).T,dtype=torch.float32).cuda())
        self.poss_trans = None

    def save(self, path):
        for id in range(self.shape.numChildren()):
            mesh = self.shape.getMesh(id)
            tex = mesh.getTextureDiffuse()
            texRef = self.texs[id].detach().cpu().numpy()
            for d in range(4):
                tex.setDataChannel(d, texRef[:,:,d].T)
            
        for id,bw in enumerate(self.bws):
            self.shape.setBoneWeight(id,bw.detach().cpu().numpy().T)
        self.shape.write(path)

    def render(self, glctx, mvp, resolution=1024, enable_mip=False, max_mip_level=0, ref=True):
        return render_multi(glctx, mvp, 
                            self.poss if ref else self.poss_trans, 
                            self.idxs, self.uvs, self.texs, resolution, enable_mip, max_mip_level)

    def render_by_merge(self, glctx, mvp, resolution=1024, enable_mip=False, max_mip_level=0, ref=True):
        from meshMerger import merge_mesh
        pos,idx,uv,tex = merge_mesh(self, ref)
        return render(glctx, mvp, pos, idx, uv, idx, tex, resolution, enable_mip, max_mip_level)

    def nr_animation(self):
        return self.shape.nrAnimation()
    
    def duration(self, index):
        return self.shape.duration(index)

    def sample_animation(self):
        index = random.randint(0,self.nr_animation()-1)
        time = random.uniform(0,self.duration(index))
        self.calc_animation(index, time)
        return index, time

    def calc_animation(self, index, time):
        self.shape.setAnimatedFrame(index, time, False)
        self.bone_trans = torch.tensor(self.shape.getBoneTransforms(),dtype=torch.float32).cuda()
        self.bone_trans = torch.permute(self.bone_trans.reshape((4,self.bone_trans.shape[1]//4,4)),(1,0,2))
        self.bone_trans = torch.cat((self.zero_trans.unsqueeze(0), self.bone_trans), dim=0)  #add a dummy bone
        self.calc_animated_poss()

    def calc_animated_poss(self, fast=True):
        #bid: [#maxBone,#vertex]
        #bw:  [#maxBone,#vertex]
        #boneTrans: [#bone,4,4]
        self.poss_trans = []
        for pos, bid, bw in zip(self.poss, self.bids, self.bws):
            if fast:
                trans  = torch.index_select(self.bone_trans,0,bid[:,0]+1) * bw[:,0].unsqueeze(1).unsqueeze(2)
                trans += torch.index_select(self.bone_trans,0,bid[:,1]+1) * bw[:,1].unsqueeze(1).unsqueeze(2)
                trans += torch.index_select(self.bone_trans,0,bid[:,2]+1) * bw[:,2].unsqueeze(1).unsqueeze(2)
                trans += torch.index_select(self.bone_trans,0,bid[:,3]+1) * bw[:,3].unsqueeze(1).unsqueeze(2)
                pos_trans = torch.bmm(trans[:,:3,:3], pos.unsqueeze(2)).squeeze(2) + trans[:,:3,3]
            else:
                pos_trans = []
                for pid in range(pos.shape[0]):
                    trans = torch.zeros((4,4)).cuda()
                    for d in range(4):
                        trans += self.bone_trans[bid[pid,d]+1,...] * bw[pid,d]
                    p_trans = torch.mm(trans[:3,:3],pos[pid:pid+1,:].T) + trans[:3,3:4]
                    pos_trans.append(p_trans)
                pos_trans = torch.cat(pos_trans,dim=1).T
            self.poss_trans.append(pos_trans)

if __name__=='__main__':
    drawer = vis.Drawer(['--headless','1'])
    anim = Animation('char50.glb')
    anim.save('char50-modified.glb')
    
    use_opengl = False
    glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    
    mvp = None
    nFrame = 1000
    writer = imageio.get_writer('animation.mp4', fps=drawer.FPS())
    for i in range(nFrame):
        print('Writing frame %d/%d'%(i,nFrame))
        anim.calc_animation(0, i*1./drawer.FPS())
        if mvp is None:
            mvp = sample_camera_multi(anim.poss_trans, dir=np.array([0.,0.,1.]), up=np.array([0.,-1.,0.]))
        color, depth = anim.render_by_merge(glctx, mvp, ref=False)
        save_image('color.png', color[0,:].cpu().numpy())
        im = imageio.imread('color.png')
        writer.append_data(im)
    writer.close()