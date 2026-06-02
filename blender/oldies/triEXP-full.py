#!BPY 
""" 
Name: '-> Raydium Full export script' 
Blender: 248 
Group: 'Export' 
Tooltip: 'Export object pose, mesh and texture as tga' 
""" 

try:
	import nt
	os=nt
	os.sep='\\'
except:
	import posix
	os=posix
	os.sep='/'

import Blender,bpy
from Blender import Window,sys
from Blender.sys import *
import math 
from math import *

path=''

allobj = 0
multfile = 0
autotri=1
savetga = 0
saveposrot=0
genfilen=0
pos_file_name=''
exp_file_name=''
selected_file_name=''


def fileExists(f):
	af = join(path,f)
	try:
		file = open(af)
	except IOError:
		exists = 0
	else:
		file.close()
		exists = 1
	return exists

#Using blender feature
#new image are tga
#creating a new image / size is croped to power2
#copy original image to new one
#save image, updating original image with new one
#TODO Unlink temp image file
def conv_tga(image):
	simg=image
	dimg_name = simg.getName().split('.')[0]+'.tga'
	print "Converting %s image to %s." % (simg.getName(),dimg_name)
	
	sz = simg.getSize()
	isx=int(pow(2,int(log(sz[0])/log(2))))
	isy=int(pow(2,int(log(sz[1])/log(2))))
	dp=simg.getDepth()
	dimg = Blender.Image.New(dimg_name.split('.')[0]+'.tmp',isx,isy,dp)
	fxs=float(sz[0])/isx;fys=float(sz[1])/isy
	xo=0;yo=0
	#image rescale to have power of 2 texture
	#x,y in result image,	 xo,yo in original image
	
	for y in range(isy):
		yoi=int(yo);xo=0;dxf=1
		Blender.Window.DrawProgressBar(float(y)/isy,"Converting to tga ...")
		for x in range(isx):
			xoi=int(xo)
			pxy=simg.getPixelI(xoi,yoi)
			dimg.setPixelI(x,y,pxy)
			xo=xo+fxs
		yo=yo+fys
		
	
	dimg.setFilename(join(path,dimg_name))
	dimg.save() 
	
	simg.setFilename(join(path,dimg_name))
	simg.reload()
	simg.setName(dimg_name)
	print "Image converted to %d %d tga and updated(%s)." % (isx,isy,dimg_name)


def valid_texture(image,texture_list):
	#print "Valid texture %s" % image
	file_name=basename(image.filename)
	#already managed
	if file_name in texture_list:
		return  
	texture_list.add(file_name)
	if not(file_name.endswith('.tga')):
		print "Find non .tga texture"
		if savetga:
			conv_tga(image)
			file_name=basename(image.filename)
			texture_list.add(file_name)
	if not (fileExists(file_name)):
		if savetga:
			print "Creating %s " % (join(path,file_name))
			image.setFilename(join(path,file_name))
			image.save()
		else:
			print "Missing %s texture file." % file_name
		
#export object pos and rotation 
def exp_pos_rot(ob,gfich):
	chout = ob.getName()
	pos = ob.getLocation('worldspace')
	chout = chout + " %f,%f,%f" % (pos[0],pos[1],pos[2])
	rot = ob.getEuler('worldspace')
	chout = chout + ",%f,%f,%f" % (-rot[0],-rot[1],-rot[2])
	#print chout
	gfich.write(chout)
	gfich.write("\n")
	
def quadToTri (object,scene):
	object.sel=1
	Blender.Object.Duplicate(mesh=1)
	ob2=Blender.Object.GetSelected()[0]
	m2=ob2.getData(mesh=1)
	for f in m2.faces:
		f.sel=1
	m2.quadToTriangle()
	#scn.unlink(ob2)
	return ob2
	
def export(): 
	global allobj

	Window.WaitCursor(1) 
	tinit = sys.time()
		
	print "Export begin"
	Blender.Window.EditMode(0)
	scene=Blender.Scene.GetCurrent()
	
	print "Path is %s" % path

	if (not(allobj)):
		objects = Blender.Object.GetSelected()
		if len(objects)==0:
			allobj=1
			print "-- No object selected." 
		else:
			print "-- Exporting %d Selected Objects." % (len(objects))
	if allobj:
		objects = scene.objects
		print "-- Exporting all %d objects" % (len(objects))

	texture_list=set()
	mesh_list=set()
	
	if (saveposrot):
		gfich = open(pos_file_name,'w')
		gfich.write('3\n')
		print "-- Export pos/rot file : "+basename(pos_file_name)
	
	#Deselect all objects
	for object in objects:
		object.sel=0
		
	if not(multfile):
		file = open (exp_file_name,'w')
		file.write("1\n") #tri version	
		print "-- Export all meshes file : "+basename(exp_file_name)
	
	for object in objects:
		objtype=object.getType()
		
		if objtype =="Lamp":
			if saveposrot:
				exp_pos_rot(object,gfich)
			
		if objtype != "Mesh":
			continue
		
		if (saveposrot):
			exp_pos_rot(object,gfich)
			
		mesh=object.getData(mesh=1) 		
		
		#Hidden mesh can be used to save hinge pos and rot without needed
		#to generate mesh .tri file
		#need to be documented
		#print mesh.name
		if mesh.name[0]=='.':
			#hidden mesh
			print "%s is an hidden mesh" % (mesh.name)
			continue
			
		if mesh.name in mesh_list:
			#mesh already exported
			continue
		print "%s is a mesh(%d faces)" % (object.getName(),len(mesh.faces))	
		
		if (multfile):
			filen= mesh.name + '.tri'
			dirn=Blender.sys.dirname(exp_file_name)
			filename=join(dirn,filen)
			file = open (filename,"w")
			file.write("1\n") #tri version	
			print "--Export One mesh in file : "+basename(filename)
				
		mesh_list.add(mesh.name)
		
		if autotri:
			#ob2 is a shadow object with all face as triangle
			ob2=quadToTri(object,scene)
			mesh = ob2.getData(mesh=1)
		
		l=len(mesh.faces)/10
		li=0
		lp=0
		org=mesh.activeUVLayer
		layers=mesh.getUVLayerNames()
		if (len(layers)>0):
			mesh.activeUVLayer=layers[0]
		for face in mesh.faces:
			if len(face)!=3:
				print "ERROR: NOT A TRIANGLE ONLY MESH ! (select all vertices and use CTRL+T)"
				if autotri:
					print "This is normaly impossible "
				#continue
			li=li+1
			if li>l:
				lp+=10
				print "%d" % lp
				li=0
				
			for i in range(3): #triangles only ! (CTRL+T) 
				indx=face.v[i].index
				#writing vertex coordinate
				file.write ("%f %f %f " % (mesh.verts[indx].co[0],mesh.verts[indx].co[1],mesh.verts[indx].co[2]))
				if face.smooth:
					nv=mesh.verts[indx].no
				else:
					nv=face.no
				#writing vertex normal
				file.write ("%f %f %f " % (nv[0],nv[1],nv[2]))
				#if face is textured
				if(mesh.faceUV):
					mesh.activeUVLayer=layers[0]					
					u=face.uv[i][0]
					v=face.uv[i][1]
					#get current texture image name
					if (face.image):
						valid_texture(face.image,texture_list)
						texture=Blender.sys.basename(face.image.filename)
					else:
						texture="rgb(0.6,0.0,0.0)"
					but=texture
					cpt=0 # layers counter
					if(len(layers)>1):
						# texture=texture+';'
						# loop on layers and append uv and name to a string
						#print len(layers)
						for layer in layers:
							if (layer==layers[0]):
								continue #already used first texture layer
							mesh.activeUVLayer=layer
							#Not textured layer skipping
							# handle vertex with only one texture defined on a 
							# mesh multitextured							
							if not(face.image):
								continue
							#tex flag inactive, face is not textured, skipping
							if not(face.mode and Blender.Mesh.FaceModes['TEX']):
								continue
							#Face image flagged as reflective
							if (face.image.reflect):
								#Found a face mapped on environnement using .tri file # syntax
								valid_texture(face.image,texture_list)
								t=Blender.sys.basename(face.image.filename)
								texture=texture+"#"+t
							else: #face image is not flagged as reflection so normal script behavior
								uu=face.uv[i][0]
								vv=face.uv[i][1]
								valid_texture(face.image,texture_list)
								t=Blender.sys.basename(face.image.filename)
								texture=texture+';'+str(uu)+'|'+str(vv)+'|'+t
						#end of layer multitexturing loop
					file.write("%f %f %s\n" % (u,v,texture))
				#face has no face UV informations
				else:
					#Face has vertex color assigned, using them.
					if(mesh.vertexColors and len(face.col)>0):
						file.write("0 0 rgb(%3.3f,%3.3f,%3.3f)\n" % (face.col[i].r/255.0,face.col[i].g/255.0,face.col[i].b/255.0))
					else:
					#No information, defaulting to gray face
						file.write("0 0 rgb(0.6,0.6,0.6)\n")
		if (org):
			mesh.activeUVLayer=org
		if multfile:
			file.flush()
			file.close()
		if autotri:
			scene.objects.unlink(ob2)
		print "Mesh Exported."
	if not(multfile):
		file.flush()
		file.close()
	if(saveposrot):	
		gfich.flush()
		gfich.close()
	
	print 'Export Script finished in %.2f seconds\n' % (sys.time()-tinit)
	print ' '
	Window.WaitCursor(0) 
	
def gui():
	global saveposrot,multfile,autotri,savetga,genfilen
	
	oallobj = Blender.Draw.Create(0)
	omultfile = Blender.Draw.Create(0)
	oautotri = Blender.Draw.Create(1)
	osavetga = Blender.Draw.Create(0)
	osaveposrot=Blender.Draw.Create(0)
	ogenfilen=Blender.Draw.Create(0)
	
	conf = Blender.Registry.GetKey("raydium_full_export", True)
	
#	if not conf:
#		return
	
	try:
		oallobj.val = conf["allobj"]
		omultfile.val = conf["multfile"]
		oautotri.val = conf["autotri"]
		osavetga.val = conf["savetga"]
		osaveposrot.val = conf["saveposrot"]
		ogenfilen.val = conf["genfile"]
	except:
		# If one of the key is not in the dict, don't worry, it'll use the defaults
		pass

	block = []
	#block.append("Raydium tri export")#("Name: ", text, 0, 30, "this is some tool tip"))
	dirs="Dir."+Blender.sys.makename()
	#block.append("Export Mesh")
	block.append(("Save All Objects",oallobj,"If Not Set Save only selected objects"))
	block.append(("Separate Files",omultfile,"Each Mesh is saved in it's own file."))
	block.append(("Auto Quad->Tri",oautotri,"Automaticaly convert quad to tri."))
	block.append(("Create/Save tga",osavetga,"If needed create and save texture tga files"))
	block.append(("Save Object Pose",osaveposrot,"Save each object position and rotation"))
	block.append(("Auto. FileName",ogenfilen,"Automaticaly generate file name based on blender filename"))
	block.append("")
	block.append("")
	block.append("")
	
	retval = Blender.Draw.PupBlock("Raydium_Export "+dirs, block)
	if retval:
		conf = {}	
		conf["allobj"] = oallobj.val
		conf["multfile"] = omultfile.val
		conf["autotri"] = oautotri.val
		conf["savetga"] = osavetga.val
		conf["saveposrot"] = osaveposrot.val
		conf["genfile"] = ogenfilen.val
		Blender.Registry.SetKey("raydium_full_export", conf, True)
	
	
	saveposrot = osaveposrot.val
	multfile = omultfile.val
	autotri = oautotri.val
	savetga = osavetga.val
	saveposrot = osaveposrot.val
	genfilen = ogenfilen.val
	
def fs2(a):	
	global exp_file_name

	a=makename(a,'.tri')
	exp_file_name=a
	
	export()

def fs1(a):
	a=makename(a,'.tri')
	pos_file_name=a;

	exp_file_name = makename (ext = '.tri')
	if (not(multfile)):
		if (not(genfilen)):
			Blender.Window.FileSelector(fs2, ".tri Mesh Export", exp_file_name)
	else:
		fs2(exp_file_name)

		
def main(): 
	global pos_file_name,exp_file_name,selected_file_name,path,genfilen
	for i in range(20):
		print 
		
	gui()
	path = dirname(makename())
	pos_file_name = makename (ext = '-pos.tri')
	if (saveposrot):
		if (not(genfilen)):
			Blender.Window.FileSelector(fs1, ".tri Object Pos/Rot File", pos_file_name)
	fs1(pos_file_name)	
		
	 
# This lets you can import the script without running it 
if __name__ == '__main__': 
	main() 