#!BPY

"""
Name: 'Raydium Export (.tri format)'
Blender: 2.36"
Group: 'Export'
Tooltip: 'Export to .tri format'
"""

import Blender
from Blender import NMesh

class source:
	def __init__(self,filename):
		self.file=open(filename,"w")

	def writeFaces(self):
		scene=Blender.Scene.getCurrent()
		allobj=scene.getChildren()
		Blender.Set("curframe", 0)
		mesh=NMesh.GetRawFromObject(allobj[0].name)
		vlist=mesh.verts

		# shame :)
		nv=0
		for nvi in mesh.faces:
		    nv=nv+1
		nv=nv*3

		self.file.write("2\n") #tri version
		self.file.write("20 %i\n" % nv) # number of anims
		# anims
		self.file.write("0 39 stand\n")
		self.file.write("40 45 run\n")
		self.file.write("46 53 attack\n")
		self.file.write("54 57 pain1\n")
		self.file.write("58 61 pain2\n")
		self.file.write("62 65 pain3\n")
		self.file.write("66 71 jump\n")
		self.file.write("72 83 flip\n")
		self.file.write("84 94 salute\n")
		self.file.write("95 111 taunt\n")
		self.file.write("112 122 wave\n")
		self.file.write("123 134 point\n")
		self.file.write("135 153 crstnd\n")
		self.file.write("154 159 crwalk\n")
		self.file.write("160 168 crattack\n")
		self.file.write("169 172 crpain\n")
		self.file.write("173 177 crdeath\n")
		self.file.write("178 183 death1\n")
		self.file.write("184 189 death2\n")
		self.file.write("190 197 death3\n")
		self.file.write("0\n") #sub-tri version
				
		for object in scene.getChildren():
		    objtype=object.getType()
		    if objtype == "Mesh":
			print "ok, it's a mesh"
			for frame in range(198):
			    print "frame ", frame
			    Blender.Set("curframe", frame)
			    mesh=NMesh.GetRawFromObject(object.name)
			    vlist=mesh.verts
			    for face in mesh.faces:
				if len(face)!=3:
				    print "ERROR: NOT A TRIANGLE ONLY MESH ! (select all vertices and use CTRL+T)"
		                for i in range(3): #triangles only ! (CTRL+T)
		                    indx=vlist.index(face[i])
				    if(face.image):
					u=face.uv[i][0]
					v=face.uv[i][1]
					self.file.write("%f %f %f %f %f %s\n" % (vlist[indx][0],vlist[indx][1],vlist[indx][2],u,v,face.image.name))
				    else:   
					self.file.write("%f %f %f 0 0 rgb(0.5,0.5,0.5)\n" % (vlist[indx][0],vlist[indx][1],vlist[indx][2]))
					


# get the face normals
#			self.file.write("GLfloat normals[%d][3] = {\n" % len(mesh.normals) )
#			for normal in mesh.normals:
#				self.file.write("{ %ff, %ff, %ff },\n" % (normal[0],normal[1],normal[2]))
#			self.file.write("};\n\n")

# get the mesh colors
#			if mesh.colors:
#				self.file.write("char OBJ_HAVE_COLORS=1;\n")
#				self.file.write("GLfloat colors[%d][3] = {\n" % len(mesh.colors) )
#				for color in mesh.colors:
#					self.file.write("{ %ff, %ff, %ff },\n" % (color[0],color[1],color[2]))
#				self.file.write("};\n\n")
#			else:
#				self.file.write("char OBJ_HAVE_COLORS=0;\nGLfloat *colors;\n")


	def close(self):
			self.file.flush()
			self.file.close()

def fs_callback(filename):
	if filename.find('.tri', -4) <= 0: filename += '.tri'
	obj=source(filename)
	obj.writeFaces()
	obj.close
	print "Exported to %s. Textures must be .tga, uncompressed, origin NOT at lower-left.\n" % (filename)

Blender.Window.FileSelector(fs_callback, "Export Raydium Tri")
