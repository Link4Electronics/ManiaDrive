#ifndef REG_API_C
#define REG_API_C

/*
    Raydium - CQFD Corp.
    http://raydium.org/
    Released under both BSD license and Lesser GPL library license.
    See "license.txt" file.
*/

#ifndef DONT_INCLUDE_HEADERS
#include "index.h"
#else
#include "headers/reg_api.h"
#endif

#ifdef REG_API
#ifdef PHP_SUPPORT

// Part 1: create PHP wrappers

// file.c
PHP_v_sS(raydium_file_home_path_cpy);
PHP_v_v(raydium_file_cache_flush);

// path.c
PHP_i_v(raydium_path_package_cache_clear);
//PHP_i_s(raydium_path_package_register); // Crash non reentrent php calls not supported
PHP_v_si(raydium_path_package_mode);

// light.c
PHP_v_v(raydium_light_enable);
PHP_v_v(raydium_light_disable);
PHP_v_i(raydium_light_on);
PHP_v_i(raydium_light_off);
PHP_v_i(raydium_light_switch);
PHP_v_ifff(raydium_light_move_3f);

// fog.c
PHP_v_v(raydium_fog_enable);
PHP_v_v(raydium_fog_disable);

// background.c
PHP_v_ffff(raydium_background_color_change);

// sound.c
PHP_i_s(raydium_sound_load_music);
PHP_i_if(raydium_sound_SetSourceGain);

// render.c
PHP_v_v(raydium_rendering_wireframe);
PHP_v_v(raydium_rendering_normal);
PHP_v_v(raydium_rendering_displaylists_enable);
PHP_v_v(raydium_rendering_displaylists_disable);
PHP_v_ffff(raydium_render_lightmap_color_4f);
PHP_v_f(raydium_render_fps_limit);
PHP_v_v(raydium_render_loading);
PHP_v_i(raydium_render_vblank);

// window.c
PHP_v_v(raydium_window_view_update);
PHP_v_fff(raydium_window_view_perspective);

// sky.c
PHP_v_s(raydium_sky_box_name);

// console.c
PHP_v_s(raydium_console_exec_script);
PHP_v_v(raydium_console_event);

// log.c
PHP_v_svaria(raydium_log);

// capture.c
PHP_v_s(raydium_capture_frame);

// key.c
PHP_i_i(raydium_key_pressed);

// mouse.c
PHP_i_i(raydium_mouse_button_pressed);
PHP_v_v(raydium_mouse_hide);
PHP_v_v(raydium_mouse_show);

// clear.c
PHP_v_v(raydium_clear_frame);

// camera.c
PHP_v_v(raydium_camera_replace);

// object.c
PHP_v_s(raydium_object_draw_name);

// network.c
//PHP_i_v(raydium_network_internet_test);
PHP_i_s(raydium_network_client_connect_to);

// particle2.c
PHP_i_ss(raydium_particle_generator_load);
PHP_v_s(raydium_particle_generator_delete_name);
PHP_v_sfff(raydium_particle_generator_move_name_3f);
PHP_i_s(raydium_particle_state_dump);
PHP_i_s(raydium_particle_state_restore);

// shadow.c
PHP_v_v(raydium_shadow_enable);
PHP_v_v(raydium_shadow_disable);
PHP_v_i(raydium_shadow_mode);
PHP_v_f(raydium_shadow_camerabox_size);

// hdr.c
PHP_v_v(raydium_hdr_enable);
PHP_v_v(raydium_hdr_disable);

// lensflare.c
PHP_v_v(raydium_lensflare_enable);
PHP_v_v(raydium_lensflare_disable);

// parser.c
PHP_i_ss(raydium_parser_db_set);
PHP_i_sSs(raydium_parser_db_get);

#ifdef ODE_SUPPORT
//ode.c
PHP_v_v(raydium_ode_callback);
PHP_v_i(raydium_ode_draw_all);
PHP_v_sffffff(raydium_ode_element_camera_inboard_name);
PHP_i_s(raydium_ode_element_find);
PHP_i_ii(raydium_ode_element_delete);
PHP_i_si(raydium_ode_element_delete_name);
PHP_v_ii(raydium_ode_element_gravity);
PHP_v_si(raydium_ode_element_gravity_name);
//PHP_i_s(raydium_ode_element_ground_texture_get_name);
PHP_i_sff(raydium_ode_element_material_name);
PHP_v_sfff(raydium_ode_element_move_name_3f);
PHP_v_sfff(raydium_ode_element_rotate_name_3f);
PHP_v_ssi(raydium_ode_element_moveto_name);
PHP_v_ss(raydium_ode_element_particle_name);
PHP_i_sf(raydium_ode_element_player_angle_name);
PHP_i_i(raydium_ode_element_player_get);
PHP_v_si(raydium_ode_element_rotate_direction_name);
//PHP_v_sf(raydium_ode_element_rotateq_name);
PHP_i_sf(raydium_ode_element_rotfriction_name);
//PHP_i_sf(raydium_ode_element_rotq_get_name);
PHP_i_sf(raydium_ode_element_slip_name);
PHP_v_ii(raydium_ode_element_sound_update);
PHP_v_si(raydium_ode_element_sound_update_name);
PHP_i_i(raydium_ode_element_tag_get);
PHP_i_s(raydium_ode_element_touched_get_name);
PHP_v_si(raydium_ode_element_ttl_set_name);
PHP_v_s(raydium_ode_ground_set_name);
PHP_i_sssffffff(raydium_ode_joint_attach_hinge_name);
PHP_i_sssffffff(raydium_ode_joint_attach_hinge2_name);
PHP_i_sii(raydium_ode_joint_attach_fixed);
PHP_v_sf(raydium_ode_joint_break_force_name);
PHP_v_sff(raydium_ode_joint_hinge_limits_name);
PHP_v_si(raydium_ode_joint_hinge2_block_name);
PHP_v_sff(raydium_ode_joint_suspension_name);
PHP_i_ssffff(raydium_ode_launcher_name_3f);
PHP_i_ssffff(raydium_ode_launcher_simple_name_3f);
PHP_v_sf(raydium_ode_motor_angle_name);
PHP_v_ssi(raydium_ode_motor_attach_name);
PHP_i_sii(raydium_ode_motor_create);
PHP_v_si(raydium_ode_motor_gear_change_name);
PHP_v_sf(raydium_ode_motor_power_max_name);
PHP_v_sfff(raydium_ode_motor_rocket_orientation_name);
PHP_v_si(raydium_ode_motor_rocket_playermovement_name);
PHP_v_ssfff(raydium_ode_motor_rocket_set_name);
PHP_v_sf(raydium_ode_motor_speed_name);
PHP_f_si(raydium_ode_motor_speed_get_name);
PHP_v_ss(raydium_ode_name_auto);
PHP_i_siffffiis(raydium_ode_object_box_add);
PHP_i_s(raydium_ode_object_create);
PHP_i_i(raydium_ode_object_delete);
PHP_i_s(raydium_ode_object_delete_name);
PHP_i_s(raydium_ode_object_find);
PHP_v_sfff(raydium_ode_object_move_name_3f);
PHP_v_sfff(raydium_ode_object_rotate_name_3f);
PHP_i_siffiis(raydium_ode_object_sphere_add);
PHP_i_sifffiis(raydium_ode_object_capsule_add);
PHP_v_f(raydium_ode_time_change);
PHP_i_s(raydium_ode_capture_3d);
PHP_v_s(raydium_ode_capture_record);
PHP_v_v(raydium_ode_capture_record_stop);
PHP_v_si(raydium_ode_capture_play);
PHP_v_v(raydium_ode_capture_stop);
PHP_v_i(raydium_ode_autodisable_set);

// ode_net.c
PHP_i_i(raydium_ode_network_element_isdistant);
PHP_v_v(raydium_ode_network_element_send_all);
PHP_v_i(raydium_ode_network_element_send_iterative);
PHP_v_i(raydium_ode_network_element_send_random);
#endif

// gui.c
PHP_i_s(raydium_gui_theme_load);
PHP_v_v(raydium_gui_show);
PHP_v_v(raydium_gui_hide);
PHP_i_v(raydium_gui_isvisible);
PHP_v_s(raydium_gui_window_delete_name);
PHP_v_fff(raydium_gui_widget_sizes);
PHP_i_sffff(raydium_gui_window_create);
PHP_i_siffs(raydium_gui_button_create_simple);
PHP_i_siffsfff(raydium_gui_label_create);
PHP_i_siffiii(raydium_gui_track_create);
PHP_i_siffs(raydium_gui_edit_create);
PHP_i_siffsi(raydium_gui_check_create);
PHP_i_siffsi(raydium_gui_combo_create);
PHP_i_ssS(raydium_gui_read_name);
PHP_i_v(raydium_gui_button_clicked);

//osd.c
PHP_v_ff(raydium_osd_cursor_offset);

// shader.c
PHP_v_ssi(raydium_shader_var_i_name);
PHP_v_ssf(raydium_shader_var_f_name);
PHP_v_ssff(raydium_shader_var_2f_name);
PHP_v_ssfff(raydium_shader_var_3f_name);
PHP_v_ssffff(raydium_shader_var_4f_name);

// Part 2: register functions
void raydium_register_api(void)
{
static int done=0;

if(done)
    {
    raydium_log("RegAPI: PASSED (already done previously)");
    return;
    }

// common.c
raydium_register_variable_const_i(RAYDIUM_MAJOR,"RAYDIUM_MAJOR");
raydium_register_variable_const_i(RAYDIUM_MINOR,"RAYDIUM_MINOR");

// file.c
raydium_register_function(C2PHP(raydium_file_home_path_cpy),"raydium_file_home_path_cpy");
raydium_register_function(C2PHP(raydium_file_cache_flush),"raydium_file_cache_flush");

// path.c
raydium_register_function(C2PHP(raydium_path_package_cache_clear),"raydium_path_package_cache_clear");
//raydium_register_function(C2PHP(raydium_path_package_register),"raydium_path_package_register");
raydium_register_function(C2PHP(raydium_path_package_mode),"raydium_path_package_mode");
raydium_register_variable_const_i(RAYDIUM_PATH_PACKAGE_READONLY,"RAYDIUM_PATH_PACKAGE_READONLY");
raydium_register_variable_const_i(RAYDIUM_PATH_PACKAGE_READWRITE,"RAYDIUM_PATH_PACKAGE_READWRITE");

// light.c
raydium_register_function(C2PHP(raydium_light_enable),"raydium_light_enable");
raydium_register_function(C2PHP(raydium_light_disable),"raydium_light_disable");
raydium_register_function(C2PHP(raydium_light_on),"raydium_light_on");
raydium_register_function(C2PHP(raydium_light_off),"raydium_light_off");
raydium_register_function(C2PHP(raydium_light_switch),"raydium_light_switch");
raydium_register_function(C2PHP(raydium_light_move_3f),"raydium_light_move_3f");

// fog.c
raydium_register_function(C2PHP(raydium_fog_enable),"raydium_fog_enable");
raydium_register_function(C2PHP(raydium_fog_disable),"raydium_fog_disable");

// background.c
raydium_register_function(C2PHP(raydium_background_color_change),"raydium_background_color_change");

// sound.c
raydium_register_function(C2PHP(raydium_sound_load_music),"raydium_sound_load_music");
raydium_register_function(C2PHP(raydium_sound_SetSourceGain),"raydium_sound_setsourcegain");

// render.c
raydium_register_function(C2PHP(raydium_rendering_wireframe),"raydium_rendering_wireframe");
raydium_register_function(C2PHP(raydium_rendering_normal),"raydium_rendering_normal");
raydium_register_function(C2PHP(raydium_rendering_displaylists_enable),"raydium_rendering_displaylists_enable");
raydium_register_function(C2PHP(raydium_rendering_displaylists_disable),"raydium_rendering_displaylists_disable");
raydium_register_function(C2PHP(raydium_render_lightmap_color_4f),"raydium_render_lightmap_color_4f");
raydium_register_variable(&raydium_render_fps,RAYDIUM_REGISTER_INT,"raydium_render_fps");
raydium_register_function(C2PHP(raydium_render_fps_limit),"raydium_render_fps_limit");
raydium_register_function(C2PHP(raydium_render_loading),"raydium_render_loading");
raydium_register_function(C2PHP(raydium_render_vblank),"raydium_render_vblank");

// window.c / projection
raydium_register_function(C2PHP(raydium_window_view_perspective),"raydium_window_view_perspective");
raydium_register_function(C2PHP(raydium_window_view_update),"raydium_window_view_update");

// sky.c
raydium_register_function(C2PHP(raydium_sky_box_name),"raydium_sky_box_name");
raydium_register_variable(&raydium_sky_force,RAYDIUM_REGISTER_SCHAR,"raydium_sky_force");

// console.c
raydium_register_function(C2PHP(raydium_console_exec_script),"raydium_console_exec_script");
raydium_register_function(C2PHP(raydium_console_event),"raydium_console_event");
raydium_register_variable(raydium_console_config_texture,RAYDIUM_REGISTER_STR,"raydium_console_config_texture");

// log.c
raydium_register_function(C2PHP(raydium_log),"raydium_log");

// capture.c
raydium_register_function(C2PHP(raydium_capture_frame),"raydium_capture_frame");

// key.c
raydium_register_variable(&raydium_key_last,RAYDIUM_REGISTER_INT,"raydium_key_last");
raydium_register_variable(&raydium_key_trace,RAYDIUM_REGISTER_SCHAR,"raydium_key_trace");
raydium_register_function(C2PHP(raydium_key_pressed),"raydium_key_pressed");

// mouse.c
raydium_register_variable(&raydium_mouse_x,RAYDIUM_REGISTER_INT,"raydium_mouse_x");
raydium_register_variable(&raydium_mouse_y,RAYDIUM_REGISTER_INT,"raydium_mouse_y");
raydium_register_variable(&raydium_mouse_click,RAYDIUM_REGISTER_INT,"raydium_mouse_click");
raydium_register_function(C2PHP(raydium_mouse_button_pressed),"raydium_mouse_button_pressed");
raydium_register_function(C2PHP(raydium_mouse_hide),"raydium_mouse_hide");
raydium_register_function(C2PHP(raydium_mouse_show),"raydium_mouse_show");

// clear.c
raydium_register_function(C2PHP(raydium_clear_frame),"raydium_clear_frame");

// camera.c
raydium_register_function(C2PHP(raydium_camera_replace),"raydium_camera_replace");
raydium_register_variable(&raydium_camera_freemove_speed,RAYDIUM_REGISTER_FLOAT,"raydium_camera_freemove_speed");
raydium_register_variable(&raydium_camera_freemove_sensibility,RAYDIUM_REGISTER_FLOAT,"raydium_camera_freemove_sensibility");
//Use with echo => echo $raydium_camera_x
raydium_register_variable(&raydium_camera_x,RAYDIUM_REGISTER_FLOAT,"raydium_camera_x");
raydium_register_variable(&raydium_camera_y,RAYDIUM_REGISTER_FLOAT,"raydium_camera_y");
raydium_register_variable(&raydium_camera_z,RAYDIUM_REGISTER_FLOAT,"raydium_camera_z");

// object.c
raydium_register_function(C2PHP(raydium_object_draw_name),"raydium_object_draw_name");

// network.c
//raydium_register_function(C2PHP(raydium_network_internet_test),"raydium_network_internet_test");
raydium_register_function(C2PHP(raydium_network_client_connect_to),"raydium_network_client_connect_to");

// particle2.c
raydium_register_function(C2PHP(raydium_particle_state_dump),"raydium_particle_state_dump");
raydium_register_function(C2PHP(raydium_particle_state_restore),"raydium_particle_state_restore");
raydium_register_function(C2PHP(raydium_particle_generator_load),"raydium_particle_generator_load");
raydium_register_function(C2PHP(raydium_particle_generator_delete_name),"raydium_particle_generator_delete_name");
raydium_register_function(C2PHP(raydium_particle_generator_move_name_3f),"raydium_particle_generator_move_name_3f");

// shadow.c
raydium_register_function(C2PHP(raydium_shadow_enable),"raydium_shadow_enable");
raydium_register_function(C2PHP(raydium_shadow_disable),"raydium_shadow_disable");
raydium_register_function(C2PHP(raydium_shadow_mode),"raydium_shadow_mode");
raydium_register_function(C2PHP(raydium_shadow_camerabox_size),"raydium_shadow_camerabox_size");
raydium_register_variable_const_i(RAYDIUM_SHADOW_MODE_FULLSCENE,"RAYDIUM_SHADOW_MODE_FULLSCENE");
raydium_register_variable_const_i(RAYDIUM_SHADOW_MODE_CAMERABOX,"RAYDIUM_SHADOW_MODE_CAMERABOX");

// hdr.c
raydium_register_function(C2PHP(raydium_hdr_enable),"raydium_hdr_enable");
raydium_register_function(C2PHP(raydium_hdr_disable),"raydium_hdr_disable");

// lensflare.c
raydium_register_function(C2PHP(raydium_lensflare_enable),"raydium_lensflare_enable");
raydium_register_function(C2PHP(raydium_lensflare_disable),"raydium_lensflare_disable");

// gui.c
raydium_register_function(C2PHP(raydium_gui_theme_load),"raydium_gui_theme_load");
raydium_register_function(C2PHP(raydium_gui_show),"raydium_gui_show");
raydium_register_function(C2PHP(raydium_gui_hide),"raydium_gui_hide");
raydium_register_function(C2PHP(raydium_gui_isvisible),"raydium_gui_isvisible");
raydium_register_function(C2PHP(raydium_gui_window_delete_name),"raydium_gui_window_delete_name");
raydium_register_function(C2PHP(raydium_gui_widget_sizes),"raydium_gui_widget_sizes");
raydium_register_function(C2PHP(raydium_gui_window_create),"raydium_gui_window_create");
raydium_register_function(C2PHP(raydium_gui_button_create_simple),"raydium_gui_button_create_simple");
raydium_register_function(C2PHP(raydium_gui_label_create),"raydium_gui_label_create");
raydium_register_function(C2PHP(raydium_gui_track_create),"raydium_gui_track_create");
raydium_register_function(C2PHP(raydium_gui_edit_create),"raydium_gui_edit_create");
raydium_register_function(C2PHP(raydium_gui_check_create),"raydium_gui_check_create");
raydium_register_function(C2PHP(raydium_gui_combo_create),"raydium_gui_combo_create");
raydium_register_function(C2PHP(raydium_gui_read_name),"raydium_gui_read_name");
raydium_register_function(C2PHP(raydium_gui_button_clicked),"raydium_gui_button_clicked");

//osd.c
raydium_register_function(C2PHP(raydium_osd_cursor_offset),"raydium_osd_cursor_offset");

// parser
raydium_register_function(C2PHP(raydium_parser_db_set),"raydium_parser_db_set");
raydium_register_function(C2PHP(raydium_parser_db_get),"raydium_parser_db_get");


#ifdef ODE_SUPPORT
// ode.c
raydium_register_function(C2PHP(raydium_ode_callback),"raydium_ode_callback");
raydium_register_function(C2PHP(raydium_ode_draw_all),"raydium_ode_draw_all");
raydium_register_function(C2PHP(raydium_ode_element_camera_inboard_name),"raydium_ode_element_camera_inboard_name");
raydium_register_function(C2PHP(raydium_ode_element_find),"raydium_ode_element_find");
raydium_register_function(C2PHP(raydium_ode_element_delete),"raydium_ode_element_delete");
raydium_register_function(C2PHP(raydium_ode_element_delete_name),"raydium_ode_element_delete_name");
raydium_register_function(C2PHP(raydium_ode_element_gravity),"raydium_ode_element_gravity");
raydium_register_function(C2PHP(raydium_ode_element_gravity_name),"raydium_ode_element_gravity_name");
//raydium_register_function(C2PHP(raydium_ode_element_ground_texture_get_name),"raydium_ode_element_ground_texture_get_name");
raydium_register_function(C2PHP(raydium_ode_element_material_name),"raydium_ode_element_material_name");
raydium_register_function(C2PHP(raydium_ode_element_move_name_3f),"raydium_ode_element_move_name_3f");
raydium_register_function(C2PHP(raydium_ode_element_rotate_name_3f),"raydium_ode_element_rotate_name_3f");
raydium_register_function(C2PHP(raydium_ode_element_moveto_name),"raydium_ode_element_moveto_name");
raydium_register_function(C2PHP(raydium_ode_element_particle_name),"raydium_ode_element_particle_name");
raydium_register_function(C2PHP(raydium_ode_element_player_angle_name),"raydium_ode_element_player_angle_name");
raydium_register_function(C2PHP(raydium_ode_element_player_get),"raydium_ode_element_player_get");
raydium_register_function(C2PHP(raydium_ode_element_rotate_direction_name),"raydium_ode_element_rotate_direction_name");
//raydium_register_function(C2PHP(raydium_ode_element_rotateq_name),"raydium_ode_element_rotateq_name");
raydium_register_function(C2PHP(raydium_ode_element_rotfriction_name),"raydium_ode_element_rotfriction_name");
//raydium_register_function(C2PHP(raydium_ode_element_rotq_get_name),"raydium_ode_element_rotq_get_name");
raydium_register_function(C2PHP(raydium_ode_element_slip_name),"raydium_ode_element_slip_name");
raydium_register_function(C2PHP(raydium_ode_element_sound_update),"raydium_ode_element_sound_update");
raydium_register_function(C2PHP(raydium_ode_element_sound_update_name),"raydium_ode_element_sound_update_name");
raydium_register_function(C2PHP(raydium_ode_element_tag_get),"raydium_ode_element_tag_get");
raydium_register_function(C2PHP(raydium_ode_element_touched_get_name),"raydium_ode_element_touched_get_name");
raydium_register_function(C2PHP(raydium_ode_element_ttl_set_name),"raydium_ode_element_ttl_set_name");
raydium_register_function(C2PHP(raydium_ode_ground_set_name),"raydium_ode_ground_set_name");
raydium_register_function(C2PHP(raydium_ode_joint_attach_hinge_name),"raydium_ode_joint_attach_hinge_name");
raydium_register_function(C2PHP(raydium_ode_joint_attach_hinge2_name),"raydium_ode_joint_attach_hinge2_name");
raydium_register_function(C2PHP(raydium_ode_joint_attach_fixed),"raydium_ode_joint_attach_fixed");
raydium_register_function(C2PHP(raydium_ode_joint_break_force_name),"raydium_ode_joint_break_force_name");
raydium_register_function(C2PHP(raydium_ode_joint_hinge_limits_name),"raydium_ode_joint_hinge_limits_name");
raydium_register_function(C2PHP(raydium_ode_joint_hinge2_block_name),"raydium_ode_joint_hinge2_block_name");
raydium_register_function(C2PHP(raydium_ode_joint_suspension_name),"raydium_ode_joint_suspension_name");
raydium_register_function(C2PHP(raydium_ode_launcher_name_3f),"raydium_ode_launcher_name_3f");
raydium_register_function(C2PHP(raydium_ode_launcher_simple_name_3f),"raydium_ode_launcher_simple_name_3f");
raydium_register_function(C2PHP(raydium_ode_motor_angle_name),"raydium_ode_motor_angle_name");
raydium_register_function(C2PHP(raydium_ode_motor_attach_name),"raydium_ode_motor_attach_name");
raydium_register_function(C2PHP(raydium_ode_motor_create),"raydium_ode_motor_create");
raydium_register_function(C2PHP(raydium_ode_motor_gear_change_name),"raydium_ode_motor_gear_change_name");
raydium_register_function(C2PHP(raydium_ode_motor_power_max_name),"raydium_ode_motor_power_max_name");
raydium_register_function(C2PHP(raydium_ode_motor_rocket_orientation_name),"raydium_ode_motor_rocket_orientation_name");
raydium_register_function(C2PHP(raydium_ode_motor_rocket_playermovement_name),"raydium_ode_motor_rocket_playermovement_name");
raydium_register_function(C2PHP(raydium_ode_motor_rocket_set_name),"raydium_ode_motor_rocket_set_name");
raydium_register_function(C2PHP(raydium_ode_motor_speed_name),"raydium_ode_motor_speed_name");
raydium_register_function(C2PHP(raydium_ode_motor_speed_get_name),"raydium_ode_motor_speed_get_name");
raydium_register_function(C2PHP(raydium_ode_name_auto),"raydium_ode_name_auto");
raydium_register_function(C2PHP(raydium_ode_object_box_add),"raydium_ode_object_box_add");
raydium_register_function(C2PHP(raydium_ode_object_create),"raydium_ode_object_create");
raydium_register_function(C2PHP(raydium_ode_object_delete),"raydium_ode_object_delete");
raydium_register_function(C2PHP(raydium_ode_object_delete_name),"raydium_ode_object_delete_name");
raydium_register_function(C2PHP(raydium_ode_object_find),"raydium_ode_object_find");
raydium_register_function(C2PHP(raydium_ode_object_move_name_3f),"raydium_ode_object_move_name_3f");
raydium_register_function(C2PHP(raydium_ode_object_rotate_name_3f),"raydium_ode_object_rotate_name_3f");
raydium_register_function(C2PHP(raydium_ode_object_sphere_add),"raydium_ode_object_sphere_add");
raydium_register_function(C2PHP(raydium_ode_object_capsule_add),"raydium_ode_object_capsule_add");
raydium_register_function(C2PHP(raydium_ode_time_change),"raydium_ode_time_change");
raydium_register_function(C2PHP(raydium_ode_capture_3d),"raydium_ode_capture_3d");
raydium_register_function(C2PHP(raydium_ode_capture_record),"raydium_ode_capture_record");
raydium_register_function(C2PHP(raydium_ode_capture_record_stop),"raydium_ode_capture_record_stop");
raydium_register_function(C2PHP(raydium_ode_capture_play),"raydium_ode_capture_play");
raydium_register_function(C2PHP(raydium_ode_capture_stop),"raydium_ode_capture_stop");
raydium_register_function(C2PHP(raydium_ode_autodisable_set),"raydium_ode_autodisable_set");
raydium_register_variable_const_i(RAYDIUM_ODE_AUTODETECT,"RAYDIUM_ODE_AUTODETECT");
raydium_register_variable_const_i(RAYDIUM_ODE_STANDARD,"RAYDIUM_ODE_STANDARD");
raydium_register_variable_const_i(RAYDIUM_ODE_STATIC,"RAYDIUM_ODE_STATIC");
raydium_register_variable_const_i(RAYDIUM_ODE_FIXING,"RAYDIUM_ODE_FIXING");
raydium_register_variable_const_i(RAYDIUM_ODE_MOTOR_ENGINE,"RAYDIUM_ODE_MOTOR_ENGINE");
raydium_register_variable_const_i(RAYDIUM_ODE_MOTOR_ANGULAR,"RAYDIUM_ODE_MOTOR_ANGULAR");
raydium_register_variable_const_i(RAYDIUM_ODE_MOTOR_ROCKET,"RAYDIUM_ODE_MOTOR_ROCKET");
raydium_register_variable_const_f(RAYDIUM_ODE_SLIP_ICE,"RAYDIUM_ODE_SLIP_ICE");
raydium_register_variable_const_f(RAYDIUM_ODE_SLIP_PLAYER,"RAYDIUM_ODE_SLIP_PLAYER");
raydium_register_variable_const_f(RAYDIUM_ODE_SLIP_NORMAL,"RAYDIUM_ODE_SLIP_NORMAL");
raydium_register_variable_const_f(RAYDIUM_ODE_JOINT_FIXED,"RAYDIUM_ODE_JOINT_FIXED");
// ode_net.c
raydium_register_function(C2PHP(raydium_ode_network_element_isdistant),"raydium_ode_network_element_isdistant");
raydium_register_function(C2PHP(raydium_ode_network_element_send_all),"raydium_ode_netword_element_send_all");
raydium_register_function(C2PHP(raydium_ode_network_element_send_iterative),"raydium_ode_network_element_send_iterative");
raydium_register_function(C2PHP(raydium_ode_network_element_send_random),"raydium_ode_element_send_random");
#endif

// php.c
raydium_register_variable(raydium_php_rayphp_path,RAYDIUM_REGISTER_STR,"raydium_php_rayphp_path");

// shader.c
raydium_register_function(C2PHP(raydium_shader_var_i_name),"raydium_shader_var_i_name");
raydium_register_function(C2PHP(raydium_shader_var_f_name),"raydium_shader_var_f_name");
raydium_register_function(C2PHP(raydium_shader_var_2f_name),"raydium_shader_var_2f_name");
raydium_register_function(C2PHP(raydium_shader_var_3f_name),"raydium_shader_var_3f_name");
raydium_register_function(C2PHP(raydium_shader_var_4f_name),"raydium_shader_var_4f_name");


raydium_log("RegAPI: OK");
done=1;
}

#else
void raydium_register_api(void)
{
/* no PHP */
raydium_log("RegAPI: DISABLED (No PHP support)");
};
#endif

#else
void raydium_register_api(void)
{
/* no REG_API */
raydium_log("RegAPI: DISABLED (config.h: no REG_API symbol)");
};
#endif
#endif
