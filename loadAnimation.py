from utils import *
import math,random

def load_texture(tex):
    tex.loadCPUData()
    ret = np.zeros((tex.height(),tex.width(),4)).astype(np.float32)
    for c in range(4):
        ret[:,:,c] = tex.getDataChannel(c).T
    return torch.tensor(ret).cuda()

def load_animation(path):
    poss = []
    uvs = []
    idxs = []
    texs = []
    bids = []
    bws = []
    shape = vis.SkinnedMeshShape(path)
    for id in range(shape.numChildren()):
        mesh = shape.getMesh(id)
        #read vertex
        pos = []
        uv = []
        for vid in range(mesh.nrVertex()):
            pos.append(mesh.getVertex(vid).T)
            uv.append(mesh.getTexcoord(vid).T)
        poss.append(torch.tensor(np.vstack(pos),dtype=torch.float32).cuda())
        uvs.append(torch.tensor(np.vstack(uv),dtype=torch.float32).cuda())
        #read index
        idx = []
        for fid in range(mesh.nrIndex()//3):
            idx.append([mesh.getIndex(fid*3+d) for d in range(3)])
        idxs.append(torch.tensor(np.vstack(idx),dtype=torch.int32).cuda())
        #read texture
        texs.append(load_texture(mesh.getTexture()))
        #read bone
        bids.append(shape.getBoneId(id))
        bws.append(shape.getBoneWeight(id))
    return shape, poss, uvs, idxs, texs, bids, bws

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

def sample_animation(shape):
    index = random.randint(0,shape.nrAnimation())
    time = random.uniform(0,shape.duration(index))
    shape.setAnimatedFrame(index, time, False)
    return shape.getBoneTransforms()

def calc_animated_pos(posRef, bid, bw, boneTrans):
    #bid: [#maxBone,#vertex]
    #bw:  [#maxBone,#vertex]
    #boneTrans: [#bone,4,4]
    pos = torch.zero(posRef.shape)
    for id in range(bid.shape[0]):
        t = torch.index_select(boneTrans,0,bid[id,:])
        pos += torch.bmm(t, posRef) * bw[id,:]
    return pos

if __name__=='__main__':
    drawer = vis.Drawer(0,None)
    shape,poss,uvs,idxs,texs,bids,bws = load_animation('char10.glb')
    #poss = [poss[0], poss[0].clone() + torch.tensor([100.,100.,100.]).cuda()]
    #uvs = uvs + uvs
    #idxs = idxs + idxs
    #texs = texs + texs
    #bids = bids + bids
    #bws = bws + bws
    mvp = sample_camera_multi(poss)
    
    use_opengl = False
    glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    color, depth = render_multi(glctx, mvp, poss, idxs, uvs, texs, 1024, False, 0)
    save_image('color.png', color[0,:].cpu().numpy())
    save_image('depth.png', depth[0,:,:,2].cpu().numpy())