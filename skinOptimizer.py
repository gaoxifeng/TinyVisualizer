from weightParameterize import *
from loadAnimation import *
import os

def bilinear_downsample(x):
    w = torch.tensor([[1, 3, 3, 1], [3, 9, 9, 3], [3, 9, 9, 3], [1, 3, 3, 1]], dtype=torch.float32, device=x.device) / 64.0
    w = w.expand(x.shape[-1], 1, 4, 4) 
    x = torch.nn.functional.conv2d(x.permute(0, 3, 1, 2), w, padding=1, stride=2, groups=x.shape[-1])
    return x.permute(0, 2, 3, 1)

def optimize(file_low, file_high,
             max_iter=20000,
             log_interval=10,
             save_interval=100,
             enable_mip = False,
             res = 1024,
             ref_res = 2048,
             lr_base=1e-2,
             lr_ramp=0.1,
             use_opengl=False,
             max_mip_level = 0,
             avg_psnr_frames = 10,
             log_path = 'bw_opt'):
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
    psnrs = []
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
        loss = torch.mean((color-color_opt)**2)   #L2 pixel loss
        
        #optimize
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
        scheduler.step()
        
        #log
        psnrs.append(-10.*np.log10(loss.item()))
        if len(psnrs) > avg_psnr_frames:
            psnrs = psnrs[1:]
        if log_interval and (it%log_interval)==0:
            psnr = sum(psnrs)/len(psnrs)
            print("Iter=%4.d, psnr=%4.10f"%(it,psnr))
        #save
        if save_interval and (it%save_interval)==0 and log_path is not None:
            if not os.path.exists(log_path):
                os.makedirs(log_path, 0o777)
            #save frames
            img_path = '%s/color%d.png'%(log_path,it)
            img_opt_path = '%s/color_opt%d.png'%(log_path,it)
            save_image(img_path,color[0,:].detach().cpu().numpy())
            save_image(img_opt_path,color[0,:].detach().cpu().numpy())
            #save animation
            save_path = '%s/iter%d.glb'%(log_path,it)
            low.save(save_path)
            print("Iter=%4.d, saving to %s;%s;%s"%(it,img_path,img_opt_path,save_path))
            
    return low
        
if __name__=='__main__':
    low = optimize('char10.glb','char.glb')