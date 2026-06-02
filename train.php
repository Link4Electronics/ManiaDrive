<?php
/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

// See train.c


function set_bog_slip($element)
{
raydium_ode_element_slip_name($element,10000);
}

function create_train()
{
$a=raydium_ode_object_create("TRAIN");

// test if creation failed:
if($a<0) return -1;


raydium_ode_object_box_add("train_train",$a,1.5,-1,0,0,RAYDIUM_ODE_STANDARD,0,"train1.tri");

raydium_ode_object_box_add("train_bog1",$a,0.5, 0.8,0.27,0.27,RAYDIUM_ODE_STANDARD,0,"train_bw.tri");
set_bog_slip("train_bog1");
raydium_ode_element_move_name_3f("train_bog1",0.65,0,-0.3);
raydium_ode_joint_attach_hinge_name("train_atta1","train_train","train_bog1",0.65,0,-0.3,0,0,1);

raydium_ode_object_box_add("train_bog2",$a,0.5, 0.8,0.27,0.27,RAYDIUM_ODE_STANDARD,0,"train_bw.tri");
set_bog_slip("train_bog2");
raydium_ode_element_move_name_3f("train_bog2",-0.65,0,-0.3);
raydium_ode_joint_attach_hinge_name("train_atta2","train_train","train_bog2",-0.65,0,-0.3,0,0,1);


raydium_ode_motor_create("train_moteur",$a,RAYDIUM_ODE_MOTOR_ROCKET);
raydium_ode_motor_rocket_set_name("train_moteur","train_bog2",0,0,0);
raydium_ode_motor_rocket_orientation_name("train_moteur",0,-90,0);

return $a;
}

function create_wagon($train,$x,$attach_to)
{
if($train<0) return -1;

$px=$x*2.95;

raydium_ode_object_box_add("train_wagon_$x",$train,1.5,-1,0,0,RAYDIUM_ODE_STANDARD,0,"wagon.tri");
raydium_ode_element_move_name_3f("train_wagon_$x",$px,0,0.1);

raydium_ode_object_box_add("train_wagon_{$x}_bog1",$train,0.5, 0.8,0.27,0.27,RAYDIUM_ODE_STANDARD,0,"train_bw.tri");
set_bog_slip("train_wagon_{$x}_bog1");
raydium_ode_element_move_name_3f("train_wagon_{$x}_bog1",$px-1,0,-0.3);
raydium_ode_joint_attach_hinge_name("train_wagon_{$x}_atta1","train_wagon_$x","train_wagon_{$x}_bog1",$px-1,0,-0.3,0,0,1);

raydium_ode_object_box_add("train_wagon_{$x}_bog2",$train,0.5, 0.8,0.27,0.27,RAYDIUM_ODE_STANDARD,0,"train_bw.tri");
set_bog_slip("train_wagon_{$x}_bog2");
raydium_ode_element_move_name_3f("train_wagon_{$x}_bog2",$px+1,0,-0.3);
raydium_ode_joint_attach_hinge_name("train_wagon_{$x}_atta2","train_wagon_$x","train_wagon_{$x}_bog2",$px+1,0,-0.3,0,0,1);


raydium_ode_object_box_add("train_wagon_{$x}_inter",$train,0.1,-1,0,0,RAYDIUM_ODE_STANDARD,0,"train_a.tri");
raydium_ode_element_move_name_3f("train_wagon_{$x}_inter",$px-1.46,0,-0.2);

raydium_ode_joint_attach_hinge_name("train_wagon_{$x}_inter1","train_wagon_$x","train_wagon_{$x}_inter",$px-1.46+0.15,0,-0.2,0,0,1);
//raydium_ode_joint_hinge_limits_name("train_wagon_{$x}_inter1",-1,1);
raydium_ode_joint_attach_hinge_name("train_wagon_{$x}_inter2",$attach_to,"train_wagon_{$x}_inter",$px-1.46-0.15,0,-0.2,0,0,1);
//raydium_ode_joint_hinge_limits_name("train_wagon_{$x}_inter2",-1,1);
//raydium_ode_joint_suspension_name("train_wagon_{$x}_inter",9,0);

return "train_wagon_$x";
}


echo "deleting old train (if any)...";
raydium_ode_object_delete_name("TRAIN");

echo "train building...";
$train=create_train();
$previous="train_train";
for($i=1;$i<=8;$i++)
    {
    $previous=create_wagon($train,$i,$previous);
    }
$force=1;

echo "train Ok.";
raydium_ode_object_move_name_3f("TRAIN",-7,0,0.45);

//raydium_ode_time_change(0);

?>
