// Kartagony - ODE kart creation code

void create_kart(void)
{
int a;

#define BREAK_FORCE	150
#define ROTFRICTION	0.0005
#define SUSP		0.9,0.1

#define WHEEL_H		-0.05
#define WHEEL_FX	0.20
#define WHEEL_FY	0.12

a=raydium_ode_object_delete_name("KART");
    
  a=raydium_ode_object_create("KART");
    raydium_ode_object_box_add("kart_corps",a,1,RAYDIUM_ODE_AUTODETECT,0,0,RAYDIUM_ODE_STANDARD,TAG_KART+menu_team,"kart_batcox.tri");
    raydium_ode_element_slip_name("kart_corps",RAYDIUM_ODE_SLIP_ICE);

    raydium_ode_element_OnBlow_name("kart_corps",blow_touched);

    raydium_ode_object_sphere_add("kart_pneu_ag",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,TAG_KART+menu_team,"kart_batcox_wheel.tri");
    raydium_ode_element_rotfriction_name("kart_pneu_ag",ROTFRICTION);
    raydium_ode_element_move_name_3f("kart_pneu_ag",WHEEL_FX,WHEEL_FY,WHEEL_H);
    raydium_ode_joint_attach_hinge2_name("kart_suspet_ag","kart_corps","kart_pneu_ag",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    if(WHEELS_BREAK)
	raydium_ode_joint_break_force_name("kart_suspet_ag",BREAK_FORCE);
    raydium_ode_joint_suspension_name("kart_suspet_ag",SUSP);

    raydium_ode_object_sphere_add("kart_pneu_ad",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,TAG_KART+menu_team,"kart_batcox_wheel.tri");
    raydium_ode_element_rotfriction_name("kart_pneu_ad",ROTFRICTION);
    raydium_ode_element_move_name_3f("kart_pneu_ad",WHEEL_FX,-WHEEL_FY,WHEEL_H);
    raydium_ode_joint_attach_hinge2_name("kart_suspet_ad","kart_corps","kart_pneu_ad",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    if(WHEELS_BREAK)
	raydium_ode_joint_break_force_name("kart_suspet_ad",BREAK_FORCE);
    raydium_ode_joint_suspension_name("kart_suspet_ad",SUSP);

    raydium_ode_object_sphere_add("kart_pneu_rg",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,TAG_KART+menu_team,"kart_batcox_wheel.tri");
    raydium_ode_element_rotfriction_name("kart_pneu_rg",ROTFRICTION);
    raydium_ode_element_move_name_3f("kart_pneu_rg",-WHEEL_FX,WHEEL_FY,WHEEL_H);
    raydium_ode_joint_attach_hinge2_name("kart_suspet_rg","kart_corps","kart_pneu_rg",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    raydium_ode_joint_hinge2_block_name("kart_suspet_rg",1);
    if(WHEELS_BREAK)
	raydium_ode_joint_break_force_name("kart_suspet_rg",BREAK_FORCE);
    raydium_ode_joint_suspension_name("kart_suspet_rg",SUSP);

    raydium_ode_object_sphere_add("kart_pneu_rd",a,0.5,RAYDIUM_ODE_AUTODETECT,RAYDIUM_ODE_STANDARD,TAG_KART+menu_team,"kart_batcox_wheel.tri");
    raydium_ode_element_rotfriction_name("kart_pneu_rd",ROTFRICTION);
    raydium_ode_element_move_name_3f("kart_pneu_rd",-WHEEL_FX,-WHEEL_FY,WHEEL_H);
    raydium_ode_joint_attach_hinge2_name("kart_suspet_rd","kart_corps","kart_pneu_rd",RAYDIUM_ODE_JOINT_SUSP_DEFAULT_AXES);
    raydium_ode_joint_hinge2_block_name("kart_suspet_rd",1);
    if(WHEELS_BREAK)
	raydium_ode_joint_break_force_name("kart_suspet_rd",BREAK_FORCE);
    raydium_ode_joint_suspension_name("kart_suspet_rd",SUSP);
    
    raydium_ode_motor_create("kart_moteur",a,RAYDIUM_ODE_MOTOR_ENGINE);
    raydium_ode_motor_attach_name("kart_moteur","kart_suspet_rg",1);
    raydium_ode_motor_attach_name("kart_moteur","kart_suspet_rd",1);
//    raydium_ode_motor_power_max_name("kart_moteur",0.03);
//    raydium_ode_motor_gears_set_name("kart_moteur",gears,6);
//    raydium_ode_motor_gear_change_name("kart_moteur",gear);

    raydium_ode_motor_create("kart_frein",a,RAYDIUM_ODE_MOTOR_ENGINE);
    raydium_ode_motor_attach_name("kart_frein","kart_suspet_ag",1);
    raydium_ode_motor_attach_name("kart_frein","kart_suspet_ad",1);
    
    raydium_ode_motor_create("kart_direction",a,RAYDIUM_ODE_MOTOR_ANGULAR);
    raydium_ode_motor_attach_name("kart_direction","kart_suspet_ag",0);
    raydium_ode_motor_attach_name("kart_direction","kart_suspet_ad",0);
    raydium_ode_motor_power_max_name("kart_direction",0.2);
    
    dReal pos[3]={0,0,1};
    raydium_ode_object_move(a,pos);
    
    raydium_sound_SourcePlay(sounds[SOUND_ENGINE]);
}

