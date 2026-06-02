#!BPY

"""
Name: 'Raydium Export (.tri format)'
Blender: 2.43"
Group: 'Export'
Tooltip: 'Export to .tri format'
"""

#import rpdb2;
#rpdb2.start_embedded_debugger("test",True)

import Blender

from Blender import NMesh
from Blender.sys import *

class source:
	def __init__(self,filename):
		self.file=open(filename,"w")

	def writeFaces(self):
		Blender.Window.EditMode(0)
		scene=Blender.Scene.GetCurrent()
		self.file.write("1\n") #tri version
		
		objects = Blender.Object.GetSelected()

		if len(objects) == 0:
		    print "-- Exporting all objects"
		    objects = scene.getChildren()
		else:
		    print "-- Exporting only selected objects"

		for object in objects:
		    objtype=object.getType()
		    if objtype == "Mesh":
			print "ok, it's a mesh"
			mesh=object.getData(mesh=1) # let's get a Mesh, not a NMesh (faster, thin)
			for face in mesh.faces:
				if len(face)!=3:
				    print "ERROR: NOT A TRIANGLE ONLY MESH ! (select all vertices and use CTRL+T)"
		                for i in range(3): #triangles only ! (CTRL+T)
		                    #indx=face.vlist.index(face[i])
				    indx=face.v[i].index
				    if(mesh.faceUV and face.image):
					u=face.uv[i][0]
					v=face.uv[i][1]
					layers=mesh.getUVLayerNames()
					texture=Blender.sys.basename(face.image.filename)
					but=texture
					cpt=0 # layers counter
					if(len(layers)>1):
					    org=mesh.activeUVLayer
					    texture=texture+';'
					    # loop on layers and append uv and name to a string
					    for layer in layers:
						mesh.activeUVLayer=layer
						uu=face.uv[i][0]
						vv=face.uv[i][1]
						t=Blender.sys.basename(face.image.filename)
						if(t!=but):
						    if(cpt>0):
							texture=texture+'|'
						    cpt=cpt+1
						    texture=texture+str(uu)+'|'+str(vv)+'|'+t
					    mesh.activeUVLayer=org
					self.file.write("%f %f %f %f %f %f %f %f %s\n" % (mesh.verts[indx].co[0],mesh.verts[indx].co[1],mesh.verts[indx].co[2],mesh.verts[indx].no[0],mesh.verts[indx].no[1],mesh.verts[indx].no[2],u,v,texture))
				    else:
						if(mesh.vertexColors and len(face.col)>0):
							self.file.write("%f %f %f %f %f %f 0 0 rgb(%3.3f,%3.3f,%3.3f)\n" % (mesh.verts[indx].co[0],mesh.verts[indx].co[1],mesh.verts[indx].co[2],mesh.verts[indx].no[0],mesh.verts[indx].no[1],mesh.verts[indx].no[2],face.col[i].r/255.0,face.col[i].g/255.0,face.col[i].b/255.0))
						else:
							self.file.write("%f %f %f %f %f %f 0 0 rgb(0.6,0.6,0.6)\n" % (mesh.verts[indx].co[0],mesh.verts[indx].co[1],mesh.verts[indx].co[2],mesh.verts[indx].no[0],mesh.verts[indx].no[1],mesh.verts[indx].no[2]))
					

	def close(self):
			self.file.flush()
			self.file.close()

def fs_callback(filename):
	if filename.find('.tri', -4) <= 0: filename += '.tri'
	obj=source(filename)
	obj.writeFaces()
	obj.close
	print "Exported to %s. Textures must be .tga, uncompressed, origin NOT at lower-left.\n" % (filename)

#fs_callback("tt.tri")
nfile = makename (ext = '.tri')
Blender.Window.FileSelector(fs_callback, "Export Raydium Tri", nfile)
