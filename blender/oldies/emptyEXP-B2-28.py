#!BPY

"""
Name: 'Empty entities export for Raydium'
Blender: 2.28
Group: 'Export'
Tooltip: 'Raydium entities'
"""

import Blender
from math import *
from Blender.Mathutils import *
from Blender import NMesh

OUTPUTNAME="exported.ent"

class source:
	def __init__(self,filename):
		self.file=open(filename,"w")

	def writeLamps(self):
		scene=Blender.Scene.getCurrent()
		self.file.write("0\n")
		
		for object in scene.getChildren():
		    objtype=object.getType()
		    if objtype == "Empty":
			    emp=object.getData()
			    print "empty found"
			    pos=object.loc
			    name=object.name
			    #direc=euler2Vector(object.rot)
			    direc=object.rot
			    self.file.write("%f %f %f %f %f %f %s\n" % (pos[0],pos[1],pos[2],-direc[0],-direc[1],-direc[2],name))

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
print "empty entities exported to %s\n" % (OUTPUTNAME)
