from pyTinyVisualizer import pyTinyVisualizer as vis
import numpy as np
import math

if __name__=='__main__':
    drawer=vis.Drawer(0,None)
    drawer.addPlugin(vis.CaptureGIFPlugin(vis.GLFW_KEY_1,'record.gif',drawer.FPS()))
    
    USE_LIGHT=True
    if USE_LIGHT:
        drawer.addLightSystem(2048,20)
        drawer.getLight().lightSz(10)
        drawer.getLight().addLight(np.array([2.,2.,2.],dtype=np.single),
                                   np.array([1.,1.,1.],dtype=np.single),
                                   np.array([1.,1.,1.],dtype=np.single),
                                   np.array([0.,0.,0.],dtype=np.single))

    height=np.array([[math.sin(x*.25)*math.sin(y*.25) for y in range(128)] for x in range(64)],dtype=np.single)
    terrain=vis.TerrainShape(height,1,np.array([1/16.,1/8.,1/16.],dtype=np.single),np.array([1/16.,1/16.],dtype=np.single))
    terrain.setTexture(vis.drawGrid())
    drawer.addShape(terrain)

    #this is currently not supported by swig
    #terrain=vis.TerrainShape(lambda x,y: math.sin(x*4.)*math.sin(y*4.)/8.,1,np.array([4.,4.,4.,12.,8.,8.],dtype=np.single),.1,np.array([1/8.,1/8.],dtype=np.single))
    #terrain.setTexture(vis.drawGrid())
    #drawer.addShape(terrain)

    shapeTB=vis.Bullet3DShape()
    box=vis.makeBox(1,True,np.array([0.25,0.25,0.25],dtype=np.single))
    box.setTexture(vis.drawChecker())
    shapeTB.setLocalTranslate(np.array([2,0.5,2],dtype=np.single))
    shapeTB.addShape(box)
    drawer.addShape(shapeTB)

    drawer.addCamera3D(90,np.array([0,1,0],dtype=np.single))
    drawer.getCamera3D().setManipulator(vis.FirstPersonCameraManipulator(drawer.getCamera3D()))
    drawer.mainLoop()