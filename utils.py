import pyTinyVisualizer as vis
import nvdiffrast.torch as dr
import numpy as np
import torch

def transform_pos(mtx, pos):
    t_mtx = torch.from_numpy(mtx).cuda() if isinstance(mtx, np.ndarray) else mtx
    posw = torch.cat([pos, torch.ones([pos.shape[0], 1]).cuda()], axis=1)
    return torch.matmul(posw, t_mtx.t())[None, ...]

def render(glctx, mtx, pos, pos_idx, uv, uv_idx, tex, resolution, enable_mip, max_mip_level):
    pos_clip = transform_pos(mtx, pos)
    rast_out, rast_out_db = dr.rasterize(glctx, pos_clip, pos_idx, resolution=[resolution, resolution])

    if enable_mip:
        texc, texd = dr.interpolate(uv[None, ...], rast_out, uv_idx, rast_db=rast_out_db, diff_attrs='all')
        color = dr.texture(tex[None, ...], texc, texd, filter_mode='linear-mipmap-linear', max_mip_level=max_mip_level)
    else:
        texc, _ = dr.interpolate(uv[None, ...], rast_out, uv_idx)
        color = dr.texture(tex[None, ...], texc, filter_mode='linear')

    color = color * torch.clamp(rast_out[..., -1:], 0, 1) # Mask out background.
    return color, rast_out

def combine(c0, d0, c1, d1):
    cond_tri0 = d0[:,:,:,3]>0.5
    cond_tri1 = d1[:,:,:,3]>0.5
    cond_tri0_only = torch.logical_and(cond_tri0,torch.logical_not(cond_tri1))
    
    cond_depth = d0[:,:,:,2]<d1[:,:,:,2]
    cond_depth = torch.logical_and(torch.logical_and(cond_tri0,cond_tri1),cond_depth)
    
    cond = torch.logical_or(cond_tri0_only,cond_depth)
    shape = (cond.shape[0], cond.shape[1], cond.shape[2], 4)
    c = torch.where(cond.unsqueeze(3).expand(shape), c0, c1)
    d = torch.where(cond.unsqueeze(3).expand(shape), d0, d1)
    return c,d

def render_multi(glctx, mtx, poss, idxs, uvs, texs, resolution, enable_mip, max_mip_level):
    id = 0
    color_multi, rast_multi = None, None
    for pos, idx, uv, tex in zip(poss, idxs, uvs, texs):
        color, rast_out = render(glctx, mtx, pos, idx, uv, idx, tex, resolution, enable_mip, max_mip_level)
        #debug output code, ignored
        #save_image('color%d.png'%id, color[0,:].cpu().numpy())
        #save_image('depth%d.png'%id, rast_out[0,:,:,2].cpu().numpy())
        if id == 0:
            color_multi = color
            rast_multi = rast_out
        else:
            color_multi, rast_multi = combine(color, rast_out, color_multi, rast_multi)
        id += 1
    return color_multi, rast_multi
    
def save_image(fn, x):
    import imageio
    x = np.rint(x * 255.0)
    x = np.clip(x, 0, 255).astype(np.uint8)
    imageio.imsave(fn, x)

def modelview(eye, ctr, up):
    vis.matrixMode(vis.GL_MODELVIEW_MATRIX)
    vis.pushMatrix()
    vis.loadIdentity()
    vis.lookAtf(eye[0], eye[1], eye[2],
                ctr[0], ctr[1], ctr[2],
                up [0], up [1], up [2])
    ret = vis.getFloatv4(vis.GL_MODELVIEW_MATRIX)
    vis.popMatrix()
    return ret

def projection(fovy=90., aspect=1., zNear=1., zFar=1000.):
    vis.matrixMode(vis.GL_PROJECTION_MATRIX)
    vis.pushMatrix()
    vis.loadIdentity()
    vis.perspectivef(fovy, aspect, zNear, zFar)
    ret = vis.getFloatv4(vis.GL_PROJECTION_MATRIX)
    vis.popMatrix()
    return ret