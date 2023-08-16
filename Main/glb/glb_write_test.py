from loadAnimation import *
from meshMerger import *

if __name__=='__main__':
    drawer = vis.Drawer(['--headless','1'])
    anim = Animation('char10.glb')
    
    pos,idx,uv,tex = merge_mesh(anim, True)
    anim.texs = unmerge_mesh(anim, tex)
    for tex in anim.texs:
        tex[:,:,:3]*=.5
    
    anim.save('char10Saved.glb')
    anim = Animation('char10Saved.glb')
    
    render = True
    if render:
        use_opengl = False
        glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    
        anim.calc_animation(0, 0)
        mvp = sample_camera_multi(anim.poss_trans, dir=np.array([0.,0.,1.]), up=np.array([0.,-1.,0.]))
        color, depth = anim.render_by_merge(glctx, mvp, ref=False)
        save_image('color.png', color[0,:].cpu().numpy())