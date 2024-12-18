import sys
import pyTinyVisualizer as vis
import numpy as np
import math

if __name__=='__main__':
    drawer=vis.Drawer([])
    drawer.addPlugin(vis.CaptureGIFPlugin(vis.GLFW_KEY_1,'record.gif',drawer.FPS(),False))
    
    afrom=[-.5,-.5,-.5]
    ato=[.5,.5,.5]
    arrow=vis.ArrowShape(60,.1,.2,32)
    arrow.setArrow(afrom,ato)
    arrow.setColorAmbient(vis.GL_TRIANGLES,1,1,1)
    drawer.addShape(arrow)
    
    USE_LIGHT=True
    if USE_LIGHT:
        drawer.addLightSystem(2048,20,False)
        drawer.getLight().lightSz(10)
        for x in [-1,1]:
            for y in [-1,1]:
                for z in [-1,1]:
                    drawer.getLight().addLight(np.array([x,y,z],dtype=np.single),
                                               np.array([.05,.05,.05],dtype=np.single),
                                               np.array([.2,.2,.2],dtype=np.single),
                                               np.array([0.,0.,0.],dtype=np.single))
        
    def setup():
        global afrom,ato
        vis.imgui.Begin("Arrow Direction",0)
        _,afrom=vis.imgui.DragFloat3("from",afrom,.01,-1.,1.,"%.3f",0)
        _,ato=vis.imgui.DragFloat3("from",ato,.01,-1.,1.,"%.3f",0)
        vis.imgui.End()
    
    def frame(node):
        global afrom,ato
        arrow.setArrow(afrom,ato)
    
    drawer.addPlugin(vis.ImGuiPlugin(setup))
    drawer.setFrameFunc(frame)
    drawer.getCamera3D().setManipulator(vis.FirstPersonCameraManipulator(drawer.getCamera3D()))
    drawer.mainLoop()
