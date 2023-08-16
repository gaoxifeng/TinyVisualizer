from loadAnimation import *

def merge_mesh(anim, ref=True):
    #poss
    posM = torch.cat(anim.poss if ref else anim.poss_trans)
    #idxs
    off_idx = [0]
    for pos in anim.poss:
        off_idx.append(off_idx[-1]+pos.shape[0])
    idxM = torch.cat([idx+off_idx[id] for id,idx in enumerate(anim.idxs)])
    #uv/tex
    curr_height = 0
    max_width = max([tex.shape[1] for tex in anim.texs])
    sum_height = sum([tex.shape[0] for tex in anim.texs])
    scaled_texs = []
    scaled_uvs = []
    for uv,tex in zip(anim.uvs,anim.texs):
        scaleW = tex.shape[1] / max_width
        scaleH = tex.shape[0] / sum_height
        offsetH = curr_height / sum_height
        scaled_texs.append(tex.expand((tex.shape[0],max_width,4)))
        scaled_uvs.append(torch.cat([uv[:,0:1]*scaleW,uv[:,1:2]*scaleH+offsetH],dim=1))
        curr_height += tex.shape[0]
    texM = torch.cat(scaled_texs)
    uvM = torch.cat(scaled_uvs)
    return posM,idxM,uvM,texM
    
if __name__=='__main__':
    drawer = vis.Drawer(['--headless','1'])
    anim = Animation('char50.glb')
    
    use_opengl = False
    glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    resolution = 1024
    enable_mip = False
    max_mip_level = 0
    
    anim.calc_animation(0,0)
    pos,idx,uv,tex = merge_mesh(anim, ref=False)
    mvp = sample_camera(pos.cpu().numpy(), dir=np.array([0.,0.,1.]), up=np.array([0.,-1.,0.]))
    
    #color
    color, depth = anim.render(glctx, mvp, resolution, enable_mip, max_mip_level, ref=False)
    save_image('color.png', color[0,:].cpu().numpy())
    
    #merged color
    color, depth = render(glctx, mvp, pos, idx, uv, idx, tex, resolution, enable_mip, max_mip_level)
    save_image('color_merge.png', color[0,:].cpu().numpy())