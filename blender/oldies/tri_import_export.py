import bpy
import os
import time

#bpy.data.meshes.remove(my_mesh)
#bpy.ops.object.convert(target='MESH',keep_original=True)

active_object=None
selected_objects=[]
active_object_mode=''
temp_object=None
all_scene=True

#texture swap count
texture_swap_count=0
current_texture_list=[]
exported_mesh_list=[]

def save_state(context):
    global active_object,selected_objects,active_object_mode
    
    print ("Saving State")
    
    active_object=context.active_object
    if active_object!=None:
        active_object_mode = context.object.mode
        #Switch to object mode as default
        bpy.ops.object.mode_set(mode='OBJECT')
    
    selected_objects = context.selected_objects
    

def restore_state(context):
    global selected_objects,active_object,active_object_mode
    
    print ("Restoring State.")

    #Switch to object mode
    ob=context.object
    if ob!=None:
        if ob.mode=='EDIT':
            bpy.ops.object.mode_set(mode='OBJECT')
            
    if active_object!=None:
        bpy.ops.object.select_pattern(pattern=active_object.name)
        bpy.context.scene.objects.active=active_object

    bpy.ops.object.select_all(action='DESELECT')
    for ob in selected_objects:
        ob.select=True  

    if active_object_mode=='EDIT':
        bpy.ops.object.mode_set(mode='EDIT')        

def convert_to_mesh(obj):
    global temp_object
    
    print ("Creating temporary working Mesh for {}.".format(obj))
    
    if obj.type=='MESH':
        bpy.ops.object.duplicate()
    else:
        bpy.ops.object.convert(target='MESH',keep_original=True)    
        
    if (len(bpy.context.selected_objects)>1):
        raise ("Should never happen ")
    temp_object=bpy.context.selected_objects[0]
    bpy.context.scene.objects.active=temp_object
    
    for modifier in temp_object.modifiers:
        print ("Applying modifier %s"%modifier.name)
        bpy.ops.object.modifier_apply(modifier=modifier.name)
    
    return temp_object

def convert_quad_to_tri(obj):
    print ("Triangulate Faces with pattern")
    bpy.ops.object.select_pattern(pattern=obj.name)
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='SELECT')
    bpy.ops.mesh.quads_convert_to_tris()
    bpy.ops.object.mode_set(mode='OBJECT')

def export_vertice(vert,face,me,obj,f):    
    global all_scene
    
    if all_scene:
        rot = obj.matrix_world
        pos = obj.location
        vloc,qrot,vscale = rot.decompose()    
    
    v=me.vertices[vert].co.copy()
    
    if face.use_smooth:
        n=me.vertices[vert].normal.copy()
    else:
        n=face.normal.copy()
    
    if all_scene:
        v = (rot * v)
        n = qrot * n

    f.write("%+0f %+0f %+0f"%(v[0],v[1],v[2]))
    f.write("  %+0f %+0f %+0f"%(n[0],n[1],n[2]))

def build_vertex_color_list(i_vert,i_face,me):
       
    tex_list=[]
    for vcol in me.vertex_colors:
        if i_vert==0:
            col = vcol.data[i_face].color1
        if i_vert==1:
            col = vcol.data[i_face].color2
        if i_vert==2:
            col = vcol.data[i_face].color3
        r=col[0]
        g=col[1]
        b=col[2]
        texture=("rgb(%3.2f,%3.2f,%3.2f)"%(r,g,b))
        tex_list.append({'u':0.0,'v':0.0,'tex':texture})        

    return tex_list

def build_uv_texture_list(i_vert,i_face,me,obj):
    global texture_swap_count,current_texture_list
    
    tex_list=[]
    tex_name_list=[]
    for uvt in me.uv_textures:            
        if uvt.data[i_face].image!=None:
            if i_vert==0:
                uv=uvt.data[i_face].uv1
            if i_vert==1:
                uv=uvt.data[i_face].uv2
            if i_vert==2:
                uv=uvt.data[i_face].uv3
            texture=os.path.basename(uvt.data[i_face].image.filepath)
            
            name=uvt.data[i_face].image.name
            if (tex_name_list.count(name)==0):
                tex_name_list.append(name)
                
            if not(os.path.isfile(texture)) or uvt.data[i_face].image.file_format!='TARGA':
                image=uvt.data[i_face].image
                image.file_format='TARGA'
                image.filepath_raw=os.path.basename(image.filepath).rsplit('.',1)[0]+'.tga'
                try:
                    image.save()
                except:
                    pass
                texture=os.path.basename(uvt.data[i_face].image.filepath)
            map=uvt.data[i_face].image.mapping;
                
            tex_list.append({'u':uv[0],'v':uv[1],'tex':texture,'map':map});

    for i in range(len(tex_name_list)):
        try:
            if (tex_name_list[i]!=current_texture_list[i]):
                current_texture_list[i]=tex_name_list[i]
                texture_swap_count+=1
        except:
            current_texture_list.append(tex_name_list[i])
            texture_swap_count+=1
    
    return tex_list

def selected(faces):

    bpy.ops.object.mode_set(mode='OBJECT')
    n=0
    for face in faces:
        if face.select:
            n=n+1 
    bpy.ops.object.mode_set(mode='EDIT')    
    return n

def unselect_unhide():
    bpy.ops.object.mode_set(mode='EDIT')
    bpy.ops.mesh.select_all(action='DESELECT')
    bpy.ops.mesh.reveal()
    bpy.ops.object.mode_set(mode='OBJECT')        
    return

def select_same_image():
    bpy.ops.object.mode_set(mode='EDIT')            
    bpy.ops.mesh.select_similar(type='IMAGE')   
    bpy.ops.object.mode_set(mode='OBJECT')        
    return   

def hide_unselected():
    bpy.ops.object.mode_set(mode='EDIT')                
    bpy.ops.mesh.hide(unselected=True)
    bpy.ops.object.mode_set(mode='OBJECT')         

def export_mesh(obj,f):
    
    me=obj.data

    #me=obj.to_mesh(scene=bpy.context.scene,apply_modifiers=True,settings='RENDER')
    
    nf=len(me.faces)
    lper=-11
    print ("Exporting mesh %s, %d faces, %d vertices"%(obj.data.name,len(me.faces),len(me.vertices)))

    cpt=0 
#    for i_tface,tface in enumerate(me.faces):
    #depth, texture name, texture mode, indice
    faces_array=[[0,'',i] for i in range(nf)]
        

    print ("Working on %d faces" % len(me.faces))
    
    print ("Building faces texture list")
    for i,face in enumerate(me.faces):
        for uvt in me.uv_textures:
            if uvt.data[i].image!=None:
                faces_array[face.index][1]+='|'+uvt.data[i].image.name
                faces_array[face.index][0]=uvt.data[i].image.depth;
    
    print ("Sorting texture list")
    faces_array.sort()
        
        #bpy.ops.object.mode_set(mode='OBJECT')        

    print ("Exporting sorted list")
    #for i_face,face in enumerate(me.faces):
    for fa in faces_array:
        
        face = me.faces[fa[2]]
        i_face = face.index

        per=((cpt+1)*100)/nf
        if per>lper+10:
            print("%d%% (%d)"%(per,cpt))
            lper=per
        #print (i_face)
        verts = face.vertices
        
        for i_vert,vert in enumerate(verts):
            export_vertice(vert,face,me,obj,f)

            tex_list=build_uv_texture_list(i_vert,i_face,me,obj)
            if (len(tex_list)==0):
                tex_list+=build_vertex_color_list(i_vert,i_face,me)

            #print ("**"+tex_list)
            
            if len(tex_list)==0:
                tex_list.append({'u':0.0,'v':0.0,'tex':'rgb(0.6,0.6,0.6)'})
                
            tex=tex_list[0]
            f.write("  %f %f %s"%(tex['u'],tex['v'],tex['tex']))
            ref_tex=tex_list[0] # used to find special multi texture without uv coordinates
            tex_list.remove(tex)
            for tex in tex_list:
                #ugly way to specify a texture without inherited uv mapping
                if (tex['map']=='UV'):
                    if (abs(tex['u']-50.0*ref_tex['u'])<1e-5) and (abs(tex['v']-50.0*ref_tex['v'])<1e-5):
                        f.write(";%s"%(tex['tex']))
                    else:
                        f.write(";%f|%f|%s"%(tex['u'],tex['v'],tex['tex']))
                else:
                    f.write("#%s"%(tex['tex']))

            f.write("\n")
        cpt+=1
        
                        
    print ("%d%% (%d)"%(per,nf))
    #bpy.data.meshes.remove(me)    

def write_some_data(context, filepath, save_elements,save_position):
    global all_scene,texture_swap_count
    global exported_mesh_list

    exported_mesh_list=[]
    
    pos_file_name = os.path.basename(filepath) + ".pos"

    print ("Using file name: %s Working directory: %s"%(os.path.basename(filepath),os.path.dirname(filepath)))
    
    print ("Changing working directory to %s"%os.path.dirname(filepath))
    os.chdir(os.path.dirname(filepath))
   
    if save_elements:
        print ("Saving Each element in its own file according to mesh name.")
        all_scene=False
    else:
        print ("Saving all scene in one file.")
        print ("Opening %s for writing."%os.path.basename(filepath))
        f =open(os.path.basename(filepath),'w')
        print ("Version 1 tri file")
        f.write("1\n")
        all_scene=True
    
    if save_position:
        print ("Saving elements positions in %s file."%pos_file_name)
        f_pos=open(pos_file_name,'w')
    
    save_state(context)
    
    if len(context.selected_objects)==0:
        bpy.ops.object.select_all(action='SELECT')
        print ("No object selected, selecting all objects.")    

    selected_objects = context.selected_objects
    
    texture_swap_count=0
    print ("%d objects to export."%(len(selected_objects)))
    
    big_wobj=None    
                           
    for i_obj,obj in enumerate(selected_objects):
        
        try:
            mesh_name=obj.data.name
        except:
            mesh_name=""

        if save_position:
            n1=obj.name.replace(" ","_")
            n2=mesh_name.replace(" ","_")
            f_pos.write("%32.32s %32.32s %f %f %f %f %f %f\n"%(n1,n2,obj.location[0],obj.location[1],obj.location[2],obj.rotation_euler[0],obj.rotation_euler[1],obj.rotation_euler[2]))
 
        if obj.type=='LAMP' or obj.type=='CAMERA' or obj.type=='CURVE':
            print ("Skipping object {}.".format(obj))
            continue

        mesh_name=obj.data.name
        if mesh_name[0]=='.':
            print ("Not exporting %s hidden mesh (begin with a . "%mesh_name)
            continue
        
        if not (mesh_name in exported_mesh_list):
            exported_mesh_list.append(mesh_name)
        else:
            print ("Skipping %s already exported"%mesh_name)
            continue

        bpy.ops.object.select_all(action='DESELECT')
        #bpy.ops.object.select_name(name=obj.name)
        bpy.ops.object.select_pattern(pattern=obj.name)
        
        print ("Exporting {} / {} Type:{}".format(obj,obj.name,obj.type))
           
        wobj = convert_to_mesh(obj)
        convert_quad_to_tri(wobj)
        
        if all_scene:
            if big_wobj==None:
                print("Creating One big object/mesh for all scene.\n")
                big_wobj=wobj
            else:
                print ("Joining %s and %s\n"%(big_wobj.name,wobj.name))
                bpy.ops.object.select_all(action='DESELECT')
                bpy.context.scene.objects.active=big_wobj
                bpy.ops.object.select_pattern(pattern=big_wobj.name)
                bpy.ops.object.select_pattern(pattern=wobj.name,extend=True)
                bpy.ops.object.join()
                
            if i_obj < len(selected_objects)-1:
                print ("Not last object, continue join.\n")
                continue
            else:
                wobj=big_wobj
                print ("Ok for exporting %s...\n"%wobj.name)
                
                
                
                
        if not all_scene:
            file_name=mesh_name
            file_name+='.tri'
            print ("Opening %s for writing."%file_name)
            f =open(file_name,'w')
            print ("Version 1 tri file")
            f.write("1\n")               
            

        export_mesh(wobj,f)
            
        print ("Removing temporary object.")
        bpy.ops.object.delete()

        if not all_scene:
            print("Closing %s file."%file_name)
            f.close()

    print ("Closing tri file.")
    if all_scene:
        f.close()
        
    if save_position:
        print ("Closing position file.")
        f_pos.close()
        
    restore_state(context)
    print ("Texture swap count:%d."%texture_swap_count)
    return {'FINISHED'}


def read_some_data(context,filepath,clean_mesh):
    
    print ("Using file name: %s Working directory: %s"%(os.path.basename(filepath),os.path.dirname(filepath)))
    
    print ("Changing working directory to %s"%os.path.dirname(filepath))
    os.chdir(os.path.dirname(filepath))
    print ("importing %s."%filepath)
    
    finf=os.stat(filepath)
    fsiz=finf.st_size
    file=open(filepath,'r')
    name=os.path.basename(filepath).split('.')[0]
    ver = file.readline()
    print (ver)
    ver=int(ver)
    if ver != 1 and ver!= 0 :
        print ("Not a version 1 tri file. Not supported.")
        return {'CANCELLED'}
 
    print ('Creating Object')
    bpy.ops.object.add(type='MESH')
    #print ('Creating Mesh %s'%name)
    #bpy.data.meshes.new(name)
    #me=bpy.data.meshes[name]
    #bpy.context.object.data=me
    me=bpy.context.object.data
    print ("Reading datas.")

    nv=0
    ne=0
    nf=0

    v=me.vertices
    e=me.edges
    f=me.faces
    
    n=0
    while 1:
        vert=file.readline() 
        n=n+1
        if not vert:
            break
        
    file.seek(0)    #rewind file
    file.readline() #skip first line (version)
    
    #allocate vertices,edges face, very very faster this way 
    v.add(n)
    f.add(n/3)
    e.add(n)
    
    ltt=[] #last texture table, used to count texture changes
    texture_change_count=0 
    
    image_not_found=[]

    lper=-11    
    while 1:
        vert=file.readline() 
        
        if not vert:
            break


        per=(nv+2)*100/n;
        if per>lper+10:
            print ("%d%% (%d)"%(per,nf))
            lper=per
        
        vs=vert.split()
        ivs=0

        v[nv].co=[float(vs[0]),float(vs[1]),float(vs[2])]
        ivs=3

        if ver==1:
            v[nv].normal=[float(vs[3]),float(vs[4]),float(vs[5])]
            ivs=6

        #now compute texture information
        tu=[]
        tv=[]
        tt=[]
        tu.append(float(vs[ivs])) #remember it will be tu[0] 
        tv.append(float(vs[ivs+1]))
        
        t=vs[ivs+2] #full texture name
        
        t=t.split(';') #split textures name, first name only, others u|v|name
        temp=[];
        for it in t:
            ts=it.split('#');
            if len(ts)==1:
                temp.append([ts[0],0]);
            else:
                temp.append([ts[0],0]);
                temp.append([ts[1],1]); # if we can split, the second is environnement texture.
                
        t=temp;
        
        tt.append(t[0]) #add first texture name to texture list
        t.remove(t[0])
        
        for itt  in t:
            it=itt[0]; # get texture string
            map=itt[1]; 
            #print (it)
            iit=it.split('|')
            #u and v coordinate and texture name
            if len(iit)==3:
                tu.append(float(iit[0]))
                tv.append(float(iit[1]))
                tt.append([iit[2],map])
                
            #only texture name using raydium hard coded scale factor of 50 from first texture coordinate
            if len(iit)==1:
                tu.append(tu[0]*50) #first texture it's ALWAYS in 0
                tv.append(tv[0]*50)
                tt.append([iit[0],map])
        
        ntt=0 # number of texture layers
        ntc=0 # number of vertex color layers
        
        for i,ti in enumerate(tt): #for each texture
            t=ti[0];
            map=ti[1];
            if t.find('rgb')==0:
                ntc=ntc+1
                if ntc > len(me.vertex_colors):
                    print ("Adding RGB texture layer")
                    me.vertex_colors.new()
                l=t.split('(')
                l=l[1]
                l=l.split(',')
                l[2]=l[2].split(')')[0]
                if (nv%3)==0:
                    me.vertex_colors[ntc-1].data[nf].color1=[float(l[0]),float(l[1]),float(l[2])]
                if (nv%3)==1:
                    me.vertex_colors[ntc-1].data[nf].color2=[float(l[0]),float(l[1]),float(l[2])]
                if (nv%3)==2:
                    me.vertex_colors[ntc-1].data[nf].color3=[float(l[0]),float(l[1]),float(l[2])]                                        
            else:
                ntt=ntt+1
                #print (t)
                if ntt > len (me.uv_textures):
                    print ("Adding uv texture layer")
                    me.uv_textures.new()
                im=bpy.data.images.get(t)

                if (image_not_found.count(t)==0):
                    if not(isinstance(im,bpy.types.Image)):
                        print ("Loading image %s."%t)
                        try:
                            print ("trying")
                            bpy.data.images.load(t)
                            print ("end trying")
                        except:
                            print ("except")
                            image_not_found.append(t)
                            print ("Not found image:%s."%t)
                            pass
                    
                im=bpy.data.images.get(t)                        
                me.uv_textures[ntt-1].data[nf].image=im
                if map==0:
                    me.uv_textures[ntt-1].data[nf].image.mapping='UV';
                else:
                    me.uv_textures[ntt-1].data[nf].image.mapping='REFLECTION';
                #me.uv_textures[ntt-1].data[nf].use_image=True
                if(nv%3)==0:
                    me.uv_textures[ntt-1].data[nf].uv1=[tu[i],tv[i]]
                if(nv%3)==1:
                    me.uv_textures[ntt-1].data[nf].uv2=[tu[i],tv[i]]
                if(nv%3)==2:
                    me.uv_textures[ntt-1].data[nf].uv3=[tu[i],tv[i]]
        
        for i,t in enumerate(tt): #Count texture changes
            try:
                lt=ltt[i]
            except:
                ltt.append(t)
                texture_change_count+=1
                
            if (t!=ltt[i]):
                texture_change_count+=1
                ltt[i]=t
                           

        nv=nv+1

        if (nv%3)==0:
            e[ne].vertices=[(nv-3),(nv-2)]
            e[ne+1].vertices=[(nv-2),(nv-1)]
            e[ne+2].vertices=[(nv-1),(nv-3)]
            ne=ne+3
            
            f[nf].vertices=[(nv-3),(nv-2),(nv-1)]
            
            if v[nv-3].normal==v[nv-2].normal and v[nv-2].normal==v[nv-1].normal:
                f[nf].use_smooth=False
            else:
                f[nf].use_smooth=True
                
            nf=nf+1                
    
    print ("%d%% (%d)"%(per,nf))
    print (".tri file need %d textures swap"% texture_change_count)
    
    for t in image_not_found:
        print ("Error image %s not found."%t)
        
    print ("Mesh Imported Vertices:%d Edges:%d Faces:%d."%(len(v),len(e),len(f)))
    #me.from_pydata(v,e,f)
    bpy.ops.object.mode_set(mode='EDIT')
    print("close file.")
    file.close()  
    if clean_mesh:  
        print("Clean Up Mesh")
        bpy.ops.mesh.remove_doubles(limit=0.0001)
        bpy.ops.mesh.tris_convert_to_quads()
        
    bpy.ops.object.mode_set(mode='OBJECT')
    return {'FINISHED'} 

# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper,ImportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty


class ExportSomeData(bpy.types.Operator, ExportHelper):
    '''Export Raydium .tri file.'''
    bl_idname = "export.raydium_tri"  # this is important since its how bpy.ops.export.some_data is constructed
    bl_label = "Export Raydium Tri File"

    # ExportHelper mixin class uses this
    filename_ext = ".tri"

    filter_glob = StringProperty(default="*.tri", options={'HIDDEN'})

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    save_elements = BoolProperty(name="Save Each Element", description="Save each object as one file, using mesh name", default=False)
    save_position = BoolProperty(name="Save Each Element Pos/Rot",description="Save Position and rotation of each element",default=False)

#    type = EnumProperty(items=(('OPT_A', "First Option", "Description one"),
#                               ('OPT_B', "Second Option", "Description two."),
#                               ),
#                        name="Example Enum",
#                        description="Choose between two items",
#                        default='OPT_A')
#
    @classmethod
    def poll(cls, context):
        #return context.active_object != None
        return len(context.selectable_objects)>0

    def execute(self, context):
        now=time.time()
        print ("----------------New Export------------------------")        
        ret=write_some_data(context, self.filepath, self.save_elements,self.save_position)
        print ("Elapsed: %f"%(time.time()-now))        
        return ret
    
class ImportSomeData(bpy.types.Operator,ImportHelper):
    '''Import Raydium .tri file.'''
    bl_idname = "import_mesh.raydium_tri"  # this is important since its how bpy.ops.export.some_data is constructed
    bl_label = "Import Raydium Tri File"

    # ExportHelper mixin class uses this
    filename_ext = ".tri"

    filter_glob = StringProperty(default="*.tri", options={'HIDDEN'})

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    clean_mesh = BoolProperty(name="Clean Up Mesh", description="Remove doubles vertices and try to make quads faces", default=False)

#    type = EnumProperty(items=(('OPT_A', "First Option", "Description one"),
#                               ('OPT_B', "Second Option", "Description two."),
#                               ),
#                        name="Example Enum",
#                        description="Choose between two items",
#                        default='OPT_A')
#
    @classmethod
    def poll(cls, context):
        return True
        #return context.active_object != None
        #return len(context.selectable_objects)>0

    def execute(self, context):
        now=time.time()
        print ("----------------New Import------------------------")
        ret=read_some_data(context,self.filepath,self.clean_mesh) 
        print ("Elapsed: %f"%(time.time()-now))
        return ret


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Raydium (.tri)")

def menu_func_import(self, context):
    self.layout.operator(ImportSomeData.bl_idname, text="Raydium (.tri)")

def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.INFO_MT_file_export.append(menu_func_export)
    bpy.utils.register_class(ImportSomeData)
    bpy.types.INFO_MT_file_import.append(menu_func_import)    


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)
    bpy.utils.unregister_class(ImportSomeData)
    bpy.types.INFO_MT_file_export.remove(menu_func_Import)    


if __name__ == "__main__":
    #unregister()
    try:
        unregister()
    except:
        pass
    register()

    # test call
    for i in range(20):
        print ("\n")
    
    #bpy.ops.export.raydium_tri('INVOKE_DEFAULT')
    #bpy.ops.import_mesh.raydium_tri('INVOKE_DEFAULT')
    #write_some_data(bpy.context,"c:\\test.tri",True)
    
    if (0) :
        try:
            bpy.ops.object.mode_set(mode='OBJECT')
        except:
            pass
        print ('remove object')
        bpy.ops.object.select_all(action='SELECT')
        bpy.ops.object.delete()
        print ('removed')
        
        try:
            while 1:
                print ('remove mesh')
                bpy.data.meshes.remove(bpy.data.meshes[0])
                print ('removed')
        except:
            pass    
        
        #read_some_data(bpy.context,'C:\\raydium_win32_sdk_1041\\bin\\Cube.smooth.053.tri')
        now=time.time()   
        read_some_data(bpy.context,'C:\\raydium_win32_sdk_1041\\bin\\a.tri',True)
        print ("Elapsed: %f."%(time.time()-now))