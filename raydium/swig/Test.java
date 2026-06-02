// This a Java version of a Raydium's tutorial:
// http://wiki.raydium.org/wiki/TutorielUtiliserLaPhysique (fr)
// http://wiki.raydium.org/wiki/TutorialUsePhysics (bad en)

// See gen_bindings.php to generate bindings.
// Be sure that raydium.jar is in you CLASSPATH and native JNI Raydium
// in your LD_LIBRARY_PATH (libraydium.so, Unix) or PATH (raydium.dll, win32)

import org.raydium.*;

class Car {

    public Car()
    {
    final float BREAK_FORCE=130.f;
    final float ROTFRICTION=0.0005f;
    final float ERP=0.3f;
    final float CFM=0.8f;
    
    raydium.raydium_ode_object_delete_name("WATURE");
     
    int a=raydium.raydium_ode_object_create("WATURE");
    raydium.raydium_ode_object_box_add("corps",a,1.f,1.2f,0.6f,0.4f,raydium.RAYDIUM_ODE_STANDARD,0,"clio.tri");
    raydium.raydium_ode_element_slip_name("corps",(float)raydium.RAYDIUM_ODE_SLIP_ICE);

    raydium.raydium_ode_object_sphere_add("pneu_ag",a,0.5f,(float)raydium.RAYDIUM_ODE_AUTODETECT,raydium.RAYDIUM_ODE_STANDARD,0,"roue5.tri");
    raydium.raydium_ode_element_rotfriction_name("pneu_ag",ROTFRICTION);
    raydium.raydium_ode_element_move_name_3f("pneu_ag",0.42f,0.253f,-0.180f);
    raydium.raydium_ode_joint_attach_hinge2_name("suspet_ag","corps","pneu_ag",0,0,1,0,1,0);
    raydium.raydium_ode_joint_break_force_name("suspet_ag",BREAK_FORCE);
    raydium.raydium_ode_joint_suspension_name("suspet_ag",ERP,CFM);

    raydium.raydium_ode_object_sphere_add("pneu_ad",a,0.5f,(float)raydium.RAYDIUM_ODE_AUTODETECT,raydium.RAYDIUM_ODE_STANDARD,0,"roue5.tri");
    raydium.raydium_ode_element_rotfriction_name("pneu_ad",ROTFRICTION);
    raydium.raydium_ode_element_move_name_3f("pneu_ad",0.42f,-0.253f,-0.180f);
    raydium.raydium_ode_joint_attach_hinge2_name("suspet_ad","corps","pneu_ad",0,0,1,0,1,0);
    raydium.raydium_ode_joint_break_force_name("suspet_ad",BREAK_FORCE);
    raydium.raydium_ode_joint_suspension_name("suspet_ad",ERP,CFM);

    raydium.raydium_ode_object_sphere_add("pneu_rg",a,0.5f,(float)raydium.RAYDIUM_ODE_AUTODETECT,raydium.RAYDIUM_ODE_STANDARD,0,"roue5.tri");
    raydium.raydium_ode_element_rotfriction_name("pneu_rg",ROTFRICTION);
    raydium.raydium_ode_element_move_name_3f("pneu_rg",-0.444f,0.253f,-0.180f);
    raydium.raydium_ode_joint_attach_hinge2_name("suspet_rg","corps","pneu_rg",0,0,1,0,1,0);
    raydium.raydium_ode_joint_break_force_name("suspet_rg",BREAK_FORCE);
    raydium.raydium_ode_joint_suspension_name("suspet_rg",ERP,CFM);
    raydium.raydium_ode_joint_hinge2_block_name("suspet_rg",1);

    raydium.raydium_ode_object_sphere_add("pneu_rd",a,0.5f,(float)raydium.RAYDIUM_ODE_AUTODETECT,raydium.RAYDIUM_ODE_STANDARD,0,"roue5.tri");
    raydium.raydium_ode_element_rotfriction_name("pneu_rd",ROTFRICTION);
    raydium.raydium_ode_element_move_name_3f("pneu_rd",-0.444f,-0.253f,-0.180f);
    raydium.raydium_ode_joint_attach_hinge2_name("suspet_rd","corps","pneu_rd",0,0,1,0,1,0);
    raydium.raydium_ode_joint_break_force_name("suspet_rd",BREAK_FORCE);
    raydium.raydium_ode_joint_suspension_name("suspet_rd",ERP,CFM);
    raydium.raydium_ode_joint_hinge2_block_name("suspet_rd",1);

    raydium.raydium_ode_motor_create("moteur",a,raydium.RAYDIUM_ODE_MOTOR_ENGINE);
    raydium.raydium_ode_motor_attach_name("moteur","suspet_ag",1);
    raydium.raydium_ode_motor_attach_name("moteur","suspet_ad",1);
    raydium.raydium_ode_motor_power_max_name("moteur",0.2f);
 
    raydium.raydium_ode_motor_create("direction",a,raydium.RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium.raydium_ode_motor_attach_name("direction","suspet_ag",0);
    raydium.raydium_ode_motor_attach_name("direction","suspet_ad",0);
    raydium.raydium_ode_motor_power_max_name("direction",0.2f);
    }
}


class Display {
    public void render()
	{
	raydium.raydium_joy_key_emul();
    
	float direct=raydium.getRaydium_joy_x()*0.3f;
	float speed=raydium.getRaydium_joy_y()*55.f;

	raydium.raydium_ode_motor_speed_name("moteur",-speed);
	raydium.raydium_ode_motor_angle_name("direction",direct);

	if(raydium.getRaydium_key_last()==1027)
	    System.exit(0);

	raydium.raydium_clear_frame();
	raydium.raydium_ode_element_camera_inboard_name("corps",0.2f,0.f,0.1f,2.f,0.f,0.f);
 
	raydium.raydium_ode_draw_all(raydium.RAYDIUM_ODE_DRAW_NORMAL);

	if(raydium.raydium_key_pressed(1)!=0)
	    raydium.raydium_ode_draw_all(raydium.RAYDIUM_ODE_DRAW_DEBUG);
 
	raydium.raydium_rendering_finish();

	raydium.raydium_loop();
	}

    public void renderLoop()
	{
	do {
	    render();
	   }while(true);
	}
}


class Test {

static 
    {
    System.loadLibrary("raydium");
    }

public static void main(String p[])
    {
    raydium.raydium_init_args(p);
    raydium.raydium_php_rayphp_path_change("../../rayphp");
    raydium.raydium_window_create(640,480,raydium.RAYDIUM_RENDERING_WINDOW,"Physics test");
    raydium.raydium_texture_filter_change(raydium.RAYDIUM_TEXTURE_FILTER_TRILINEAR);
    //raydium.setRaydium_projection_near(0.01f);
    //raydium.setRaydium_projection_far(1000.f);
    //raydium.setRaydium_projection_fov(60.f);
    //raydium.raydium_window_view_update();
    raydium.raydium_window_view_perspective(60.f,0.01f,1000.f);
    raydium.raydium_light_on(0);
    raydium.raydium_light_conf_7f(0,50,150,200,1000000,1,0.9f,0.5f);
    raydium.raydium_background_color_change(1,0.9f,0.5f,1);
    raydium.raydium_fog_disable();
    raydium.raydium_shadow_enable();
    raydium.raydium_ode_ground_set_name("cocorobix.tri");
    new Car();
    new Display().renderLoop();
    }
}
