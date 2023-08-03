from weightParameterize import *
from loadAnimation import *

def bilinear_downsample(x):
    w = torch.tensor([[1, 3, 3, 1], [3, 9, 9, 3], [3, 9, 9, 3], [1, 3, 3, 1]], dtype=torch.float32, device=x.device) / 64.0
    w = w.expand(x.shape[-1], 1, 4, 4) 
    x = torch.nn.functional.conv2d(x.permute(0, 3, 1, 2), w, padding=1, stride=2, groups=x.shape[-1])
    return x.permute(0, 2, 3, 1)

def optimize(file_low, file_high,
             max_iter=20000,
             log_interval=10,
             enable_mip = False,
             res = 1024,
             ref_res = 2048,
             lr_base=1e-2,
             lr_ramp=0.1,
             use_opengl=False,
             max_mip_level = 0):
    #load model
    drawer = vis.Drawer(['--headless','1'])
    low = Animation(file_low)
    high = Animation(file_high)
    
    #parameterize weight
    weights = [WeightParameterize(bid,bw) for bid,bw in zip(low.bids,low.bws)]
    vars = [wp.log_bw for wp in weights]
    for v in vars:
        v.requires_grad_()
    
    #initialize nvdiffrast
    glctx = dr.RasterizeGLContext() if use_opengl else dr.RasterizeCudaContext()
    
    #Adam optimizer for texture with a learning rate ramp.
    optimizer = torch.optim.Adam(vars, lr=lr_base)
    scheduler = torch.optim.lr_scheduler.LambdaLR(optimizer, lr_lambda=lambda x: lr_ramp**(float(x)/float(max_iter)))
    
    #main loop
    for it in range(max_iter+1):
        #sample animation/camera
        index,time = high.sample_animation()
        mvp = sample_camera_multi(high.poss_trans)
        #reconstruct weight
        low.bws = [wp.assemble_bw() for wp in weights]
        #set low-res animation to the same
        low.calc_animation(index, time)
        
        #compute loss
        color, _ = high.render(glctx, mvp, ref_res, True, max_mip_level, ref=False)
        color_opt, _ = low.render(glctx, mvp, res, enable_mip, max_mip_level, ref=False)
        while color.shape[1] > res:
            color = bilinear_downsample(color)
        loss = torch.mean((color - color_opt)**2)   #L2 pixel loss
        
        #optimize
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        scheduler.step()
        
        #log
        if log_interval and (it%log_interval)==0:
            psnr=0.
            print("Iter=%4.d, psnr=%4.10f"%(it,psnr))
        
if __name__=='__main__':
    optimize('char10.glb','char.glb')