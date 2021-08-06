import pyTinyVisualizer as vis
import numpy as np
import math

if __name__=='__main__':
    drawer=vis.Drawer(0,None)
    drawer.addPlugin(vis.CaptureGIFPlugin(1,'record.gif',drawer.FPS()))
    
    arrow=vis.ArrowShape(60,.1,.2)
    arrow.setArrow(np.array([-.5,-.5,-.5],dtype=np.single),
                   np.array([.5,.5,.5],dtype=np.single))
    arrow.setColorAmbient(vis.GL_TRIANGLES,1,1,1)
    drawer.addShape(arrow)
    
    USE_LIGHT=True
    if USE_LIGHT:
        drawer.addLightSystem(2048,20)
        drawer.getLight().lightSz(10)
        for x in [-1,1]:
            for y in [-1,1]:
                for z in [-1,1]:
                    drawer.getLight().addLight(np.array([x,y,z],dtype=np.single),
                                               np.array([.05,.05,.05],dtype=np.single),
                                               np.array([.2,.2,.2],dtype=np.single),
                                               np.array([0.,0.,0.],dtype=np.single))
        
    
    class CustomPythonCallback(vis.PythonCallback):
        def __init__(self):
            vis.PythonCallback.__init__(self)
            self.fromX=-.5
            self.fromY=-.5
            self.fromZ=-.5
            self.toX=.5
            self.toY=.5
            self.toZ=.5
        
        def frame(self, root):
            arrow.setArrow(np.array([self.fromX,self.fromY,self.fromZ],dtype=np.single),
                           np.array([self.toX,self.toY,self.toZ],dtype=np.single))
        
        def setup(self):
            vis.Begin("Arrow Direction")
            self.fromX=vis.DragFloat("fromX",self.fromX,.01,-1.,1.,"%.3f",0)[1]
            self.fromY=vis.DragFloat("fromY",self.fromY,.01,-1.,1.,"%.3f",0)[1]
            self.fromZ=vis.DragFloat("fromZ",self.fromZ,.01,-1.,1.,"%.3f",0)[1]
            self.toX=vis.DragFloat("toX",self.toX,.01,-1.,1.,"%.3f",0)[1]
            self.toY=vis.DragFloat("toY",self.toY,.01,-1.,1.,"%.3f",0)[1]
            self.toZ=vis.DragFloat("toZ",self.toZ,.01,-1.,1.,"%.3f",0)[1]
            vis.End()

    setup=CustomPythonCallback()
    drawer.setPythonCallback(setup)
    drawer.addPlugin(vis.ImGuiPlugin(setup))
    drawer.getCamera3D().setManipulator(vis.FirstPersonCameraManipulator(drawer.getCamera3D()))
    drawer.mainLoop()