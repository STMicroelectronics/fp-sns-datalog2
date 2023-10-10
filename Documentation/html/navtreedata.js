/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "FP-SNS-DATALOG2", "index.html", [
    [ "Documentation for FP-SNS-DATALOG2 STM32Cube function pack", "index.html", null ],
    [ "eLooM framework", "e_loo_m_main.html", [
      [ "Overview", "e_loo_m_main.html#eloom_overview", [
        [ "Firmware architecture", "e_loo_m_main.html#eloom_fw_architecture_sec", null ]
      ] ],
      [ "System initialization", "e_loo_m_main.html#eloom_sys_init", [
        [ "The Application Context", "e_loo_m_main.html#eloom_applicatrion_context", null ],
        [ "Task's hardware initialization", "e_loo_m_main.html#eloom_task_hw_init", null ],
        [ "Task creation and software resources initialization.", "e_loo_m_main.html#eloom_task_sw_init", null ],
        [ "Other application specific initialization", "e_loo_m_main.html#eloom_app_init", null ],
        [ "INIT task command loop", "e_loo_m_main.html#eloom_init_command_loop", null ]
      ] ],
      [ "Power Management", "e_loo_m_main.html#eloom_power_management", [
        [ "Power Mode switch implementation", "e_loo_m_main.html#eloom_power_mode_implementation", null ],
        [ "Managed task classification during the PM transaction", "e_loo_m_main.html#eloom_power_mode_pmclass", null ],
        [ "Other considerations on the low power mode", "e_loo_m_main.html#eloom_power_mode_other", null ]
      ] ],
      [ "Error Management", "e_loo_m_main.html#eloom_error_simple", [
        [ "Simple support", "e_loo_m_main.html#error_generic", null ],
        [ "Advanced support", "e_loo_m_main.html#eloom_error_advanced", null ],
        [ "AppErrorManager and the WWDG", "e_loo_m_main.html#eloom_error_wwdg", null ]
      ] ],
      [ "eLooM and STM32Cube", "e_loo_m_main.html#eloom_cube_integration", [
        [ "How to use STM32CubeMX in the eLooM development workflow", "e_loo_m_main.html#eloom_cube_mx", null ],
        [ "eLooM objects and STM32 HAL callbacks", "e_loo_m_main.html#eloom_cube_irq", null ]
      ] ],
      [ "Support for different RTOS", "e_loo_m_main.html#eloom_rtos", [
        [ "eLooM base services", "e_loo_m_main.html#eloom_basesrv", null ]
      ] ],
      [ "Getting Started", "eloom_gs_page.html", [
        [ "Introduction", "eloom_gs_page.html#eloom_gs_intro", [
          [ "Folder organization", "eloom_gs_page.html#eloom_gs_folder_organization", null ]
        ] ],
        [ "Application mandatory files", "eloom_gs_page.html#eloom_gs_mandatory_files", null ],
        [ "The App.c file", "eloom_gs_page.html#eloom_gs_app_file", [
          [ "Other optional stuff for the App.c file", "eloom_gs_page.html#eloom_gs_appfile_other", null ]
        ] ],
        [ "How to implement the first Managed Task", "eloom_gs_page.html#eloom_gs_first_task", null ]
      ] ],
      [ "A Simple Demo", "eloom_da_page.html", [
        [ "Introduction", "eloom_da_page.html#eloom_app_intro", null ],
        [ "Application Tasks", "eloom_da_page.html#eloom_app_app_tasks", [
          [ "Hello World Task", "eloom_da_page.html#eloom_app_HelloWorldTask", [
            [ "Low Level Driver", "eloom_da_page.html#eloom_app_HelloWorldTask_LL", null ],
            [ "Service Layer", "eloom_da_page.html#eloom_app_HelloWorldTask_SL", null ],
            [ "Application Layer", "eloom_da_page.html#eloom_app_HelloWorldTask_AL", null ]
          ] ],
          [ "Push Button Task", "eloom_da_page.html#eloom_app_PushButtonTask", [
            [ "Low Level Driver", "eloom_da_page.html#eloom_app_PushButtonTask_LL", null ],
            [ "Service Layer", "eloom_da_page.html#eloom_app_PushButtonTask_SL", null ],
            [ "Application Layer'", "eloom_da_page.html#eloom_app_PushButtonTask_AL", null ]
          ] ]
        ] ],
        [ "Build Configurations", "eloom_da_page.html#eloom_app_build_configs", null ]
      ] ]
    ] ],
    [ "Digital Processing Unit v2 (DPU)", "dpu2_main.html", [
      [ "Introduction", "dpu2_main.html#dpu2_introduction", [
        [ "Main requirements", "dpu2_main.html#dpu2_requirements", null ]
      ] ],
      [ "DPU programming model", "dpu2_main.html#dpu2_prog_model", [
        [ "DPU and memory management", "dpu2_main.html#dpu2_memory", null ],
        [ "Example code", "dpu2_main.html#dpu2_example", null ]
      ] ],
      [ "Architecture of a DPU", "dpu2_main.html#dpu2_architecture", null ]
    ] ],
    [ "eLooM data format (EMData)", "emdata_main.html", [
      [ "Introduction", "emdata_main.html#emdata_introduction", [
        [ "The EMData_t class", "emdata_main.html#emdata_emdata_intro", null ],
        [ "Data event source and event listener", "emdata_main.html#emdata_event_intro", null ],
        [ "Circular Buffer", "emdata_main.html#emdata_cb_intro", null ],
        [ "Source observable", "emdata_main.html#emdata_source_bservable_intro", null ]
      ] ]
    ] ],
    [ "Sensor Manager", "sm_page.html", [
      [ "Design", "sm_page.html#design_sm", null ],
      [ "3-layer architecture", "sm_page.html#layer_sm", null ],
      [ "Sensor Data flow", "sm_page.html#dataflow_sm", [
        [ "Event/Listener design pattern", "sm_page.html#eventlistener_sm", null ],
        [ "Interfaces", "sm_page.html#interface_sm", null ]
      ] ]
    ] ],
    [ "Topics", "topics.html", "topics" ],
    [ "Data Structures", "annotated.html", [
      [ "Data Structures", "annotated.html", "annotated_dup" ],
      [ "Data Structure Index", "classes.html", null ],
      [ "Data Fields", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Variables", "functions_vars.html", "functions_vars" ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "Globals", "globals.html", [
        [ "All", "globals.html", "globals_dup" ],
        [ "Functions", "globals_func.html", "globals_func" ],
        [ "Variables", "globals_vars.html", "globals_vars" ],
        [ "Typedefs", "globals_type.html", "globals_type" ],
        [ "Enumerations", "globals_enum.html", null ],
        [ "Enumerator", "globals_eval.html", null ],
        [ "Macros", "globals_defs.html", "globals_defs" ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"_a_bus_i_f_8c.html",
"_b-_u585_i-_i_o_t02_a_2_applications_2_d_a_t_a_l_o_g2_2_core_2_inc_2_i_command_parse_8h.html",
"_b-_u585_i-_i_o_t02_a_2_applications_2_d_a_t_a_l_o_g2_2_pn_p_l_2_inc_2_acquisition___info___pn_p_l_8h.html#a7cdb9cd36051e6810106f76e96b4d2ab",
"_b-_u585_i-_i_o_t02_a_2_applications_2_d_a_t_a_l_o_g2_2mx_2_inc_2mx_8h.html#a9a5d11a287fa1cb56cb4049ec7965c3a",
"_b_l_e___gas_concentration_8c_source.html",
"_b_l_e___neai_anomaly_detection_8h.html#a088984da6b99896b722b2aac3b4402aa",
"_blue_n_r_g-_l_p_2includes_2bluenrg__conf__template_8h.html",
"_h_w_driver_map_8h.html#a8bdd8c012a7a553279d3faa377eee883",
"_i_i_o_driver_8h_source.html",
"_i_i_s3_d_w_b_task__vtbl_8h.html#a3f63485079fbdf00ca704a709b3dbf97",
"_i_s_m330_i_s_task_8c.html#a26ba5fb7bed74f65e845bc62e9387319",
"_l_i_s2_d_u12_task_8h_source.html",
"_lis2mdl___mag___pn_p_l__vtbl_8h.html#a090c5fa54709c22ce1eb753274e4b87c",
"_n_u_c_l_e_o-_h7_a3_z_i-_q_2_applications_2_d_a_t_a_l_o_g2_2_core_2_inc_2_i_command_parse_8h.html#a20e511e96bfdb094a365a4b63c0f5494",
"_n_u_c_l_e_o-_h7_a3_z_i-_q_2_applications_2_d_a_t_a_l_o_g2_2_pn_p_l_2_src_2_firmware___info___pn_p_l_8c.html",
"_n_u_c_l_e_o-_u575_z_i-_q_2_applications_2_d_a_t_a_l_o_g2_2_core_2_inc_2_i_stream_8h.html",
"_n_u_c_l_e_o-_u575_z_i-_q_2_applications_2_d_a_t_a_l_o_g2_2_pn_p_l_2_inc_2_iis2mdc___mag___pn_p_l__vtbl_8h_source.html",
"_n_u_c_l_e_o-_u575_z_i-_q_2_applications_2_d_a_t_a_l_o_g2_2mx_2_src_2stm32u5xx__hal__timebase__tim_8c_source.html",
"_s_query_8c.html#afb1b254d70067d258a8886709ccaca06",
"_s_t_m32_l4_r9_z_i-_s_t_w_i_n_2_applications_2_d_a_t_a_l_o_g2_2_core_2_inc_2services_2_b_c_protocol_8h.html#ad9e07b7426c24758cfc09716ec9307f4a47852a187b4972cf69b1fbe765e36ec6",
"_s_t_m32_l4_r9_z_i-_s_t_w_i_n_2_applications_2_d_a_t_a_l_o_g2_2_pn_p_l_2_inc_2_firmware___info___pn_p_l_8h.html#acfeff55eecff80d12c27d9df3f7b4d19",
"_s_t_m32_l4_r9_z_i-_s_t_w_i_n_2_applications_2_d_a_t_a_l_o_g2_2mx_2_inc_2gpio_8h_source.html",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_d_a_t_a_l_o_g2_2_core_2_inc_2_util_task__vtbl_8h.html#a948dc1ca8ffd0a546f4d2d9dadff0dfa",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_d_a_t_a_l_o_g2_2_core_2_src_2_datalog_app_task_8c.html#a17e378f28ffe93e6dcf85e30661dc0fc",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_d_a_t_a_l_o_g2_2_pn_p_l_2_inc_2_iis2mdc___mag___pn_p_l__vtbl_8h.html#a1d6bd0cfe35ee5f402daddc9c5d0f74b",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_d_a_t_a_l_o_g2_2_u_s_b_x_2_app_2ux__device__descriptors_8c_source.html",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_ultrasound_f_f_t_2_core_2_inc_2_i_command_parse_8h.html",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_ultrasound_f_f_t_2_pn_p_l_2_inc_2_acquisition___info___pn_p_l__vtbl_8h_source.html",
"_s_t_m32_u585_a_i-_s_t_w_i_n_8box_2_applications_2_ultrasound_f_f_t_2mx_2_src_2mdf_8c.html#af94f27eb63933c002fbc889276d591a7",
"_s_t_m32_u585_a_i-_sensor_tile_8box_pro_2_applications_2_d_a_t_a_l_o_g2_2_core_2_inc_2services_2_b_c_protocol_8h.html#a280d8b8505a65c7cbc0c9038a50c547fa1d88ee76147856b399f031652217e1b0",
"_s_t_m32_u585_a_i-_sensor_tile_8box_pro_2_applications_2_d_a_t_a_l_o_g2_2_core_2_src_2drivers_2_b_c_timer_driver_8c.html#a2ab520c7f03cb21a5c13f9bc31af4ba5",
"_s_t_m32_u585_a_i-_sensor_tile_8box_pro_2_applications_2_d_a_t_a_l_o_g2_2_u_s_b_x_2_app_2_p_c_d_driver_8c.html#a847df71fb96c21c8b5291967f948d7b0",
"_s_t_t_s22_h_task_8c.html#a95d6e2f9d246f719251c5edde2e6d790",
"_t1to_t2_data_builder_8h.html#a9810da11a35680172a4717a6fb475d0d",
"bluenrg1__hci__le_8c.html#ad2baca315a584d34c23224c52466210a",
"bluenrg__lp__gatt__aci_8c.html",
"bluenrg__lp__hci__le_8h.html#a277ca97f02be61730e03237d292804e2",
"dir_a343531c0ab293b0355d88b3d19b07f6.html",
"globals_func_e.html",
"group___g_a_p.html#ga7222735a5f33f433fe5c54cc48fa0af4",
"group___s_t_m32_h7xx___system___private___includes.html#gaaa8c76e274d0f6dd2cefb5d0b17fbc37",
"struct___a_bus_i_f.html#a24b0752c911b14cfdf0ee6f7191a9244",
"struct___e_m_data.html#a37e90f5e3bd99fac2021fb3a326607d4",
"struct___i_i_s3_d_w_b_task.html#a77bf152df3859d1ead14c044aabc55f4",
"struct___iis3dwb___acc___pn_p_l.html#a9a1c383be35f5376e14e17abe273ba4d",
"struct___m_x___a_d_c_params__t.html#a7f57bbe3c50efab841b13e6623caf4d0",
"struct___tags___info___pn_p_l.html#a9a1c383be35f5376e14e17abe273ba4d",
"struct_sensor_status_presence__t.html#afb71f78c967c5e9ff1481ab62d8f5989",
"ux__device__class__sensor__streaming__api_8c_source.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';