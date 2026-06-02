#!BPY

"""
Name: 'FSrad lights export (.lamp format)'
Blender: 2.28
Group: 'Export'
Tooltip: 'Export to .lamp format'
"""

import Blender
from math import *
from Blender.Mathutils import *
from Blender import NMesh

OUTPUTNAME="exported.tri.lamp"

class source:
	def __init__(self,filename):
		self.file=open(filename,"w")

	def writeLamps(self):
		scene=Blender.Scene.getCurrent()
		self.file.write("0\n")
		
		for object in scene.getChildren():
		    objtype=object.getType()
		    if objtype == "Lamp":
			lamp=object.getData()
			if lamp.type == Blender.Lamp.Types['Area']:
			    print "lamp found"
			    pos=object.loc
			    col=lamp.col
			    direc=euler2Vector(object.rot)
			    ener=lamp.energy
			    self.file.write("%f %f %f %f %f %f %f %f %f %f\n" % (pos[0],pos[1],pos[2],direc[0],direc[1],direc[2],col[0],col[1],col[2],ener))

	def close(self):
			self.file.flush()
			self.file.close()


def euler2Vector(rot):
    vec = Vector([0,0,-1])
    x,y,z = rot
    mat = Euler([degs(x),degs(y),degs(z)]).toMatrix()
    res = VecMultMat(vec,mat.rotationPart())
    return res[0],res[1],res[2]
	                                                                                  
def radians(deg):
  return deg * pi/180.0
                                                                                  
def degs(rad):
    return rad * 180.0/pi
        
    

obj=source(OUTPUTNAME)
obj.writeLamps()
obj.close
print "Lamps exported to %s\n" % (OUTPUTNAME)
