from utils import *
import math

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
    shape = vis.SkinnedMeshShape(path)
    for id in range(shape.numChildren()):
        mesh = shape.getMeshRef(id)
        #read vertex
        pos = []
        uv = []
        for vid in range(mesh.nrVertex()):
            pos.append(mesh.getVertex(vid).T)
            uv.append(mesh.getTexcoord(vid).T)
        poss.append(torch.tensor(np.vstack(pos),dtype=torch.float32).cuda())
        uv.append(torch.tensor(np.vstack(uv),dtype=torch.float32).cuda())
        #read index
        idx = []
        for fid in range(mesh.nrIndex()//3):
            idx.append([mesh.getIndex(fid*3+d) for d in range(3)])
        idxs.append(torch.tensor(np.vstack(idx),dtype=torch.int32).cuda())
        #read texture
        texs.append(load_texture(mesh.getTexture()))
        #read bone
    return poss, uvs, idxs, texs

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

if __name__=='__main__':
    drawer = vis.Drawer(0,None)
    poss,uvs,idxs,texs = load_animation('gargoyle_model/Internal.OBJ')
    #mvp = sample_camera(pos.cpu().numpy())
    
    use_opengl = False
    glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    color = render_multi(glctx, mvp, pos, idx, uv, idx, tex[0], 1024, False, 0)
    save_image('color.png', color[0,:].cpu().numpy())