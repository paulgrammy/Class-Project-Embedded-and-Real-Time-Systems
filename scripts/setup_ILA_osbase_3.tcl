set old_path [ pwd ]
set script_path [ file dirname [ file normalize [ info script ] ] ]
cd $script_path

open_hw_manager
connect_hw_server -allow_non_jtag
open_hw_target
current_hw_device [get_hw_devices xc7z020_1]
refresh_hw_device -update_hw_probes false [lindex [get_hw_devices xc7z020_1] 0]

set_property PROBES.FILE {../hw/ees_osbase_3_wrapper.ltx} [get_hw_devices xc7z020_1]
set_property FULL_PROBES.FILE {../hw/ees_osbase_3_wrapper.ltx} [get_hw_devices xc7z020_1]
refresh_hw_device [lindex [get_hw_devices xc7z020_1] 0]

run_hw_ila [get_hw_ilas -of_objects [get_hw_devices xc7z020_1] -filter {CELL_NAME=~"ees_osbase_3_i/system_ila_0/U0/ila_lib"}] -trigger_now
wait_on_hw_ila [get_hw_ilas -of_objects [get_hw_devices xc7z020_1] -filter {CELL_NAME=~"ees_osbase_3_i/system_ila_0/U0/ila_lib"}]
upload_hw_ila_data [get_hw_ilas -of_objects [get_hw_devices xc7z020_1] -filter {CELL_NAME=~"ees_osbase_3_i/system_ila_0/U0/ila_lib"}]
display_hw_ila_data 

## clear all the old waves
set OLDWAVES [ get_waves * ]
foreach oldwave $OLDWAVES {
    set_property NAME_STYLE long $oldwave
    remove_wave $oldwave
}

add_wave_divider -color "#777777" "INTERRUPTS"
add_wave -color #FF0000 -name "MB_FastInterrupt interrupt" ees_osbase_3_i/system_ila_0/U0/SLOT_2_MBINTERRUPT_interrupt_1
add_wave -color #FF0000 -name "MB_FastInterrupt address[0:31]" ees_osbase_3_i/system_ila_0/U0/SLOT_2_MBINTERRUPT_address_1
add_wave -color #FF0000 -name "MB_FastInterrupt ack[0:1]" ees_osbase_3_i/system_ila_0/U0/SLOT_2_MBINTERRUPT_ack_1

add_wave -name "AXI_Timer_A interrupt" ees_osbase_3_i/system_ila_0/U0/probe0_1
add_wave -name "AXI_Timer_B interrupt" ees_osbase_3_i/system_ila_0/U0/probe1_1
add_wave -name "AXI_Uartlite_0 interrupt" ees_osbase_3_i/system_ila_0/U0/probe2_1
add_wave -name "AXI_GPIO_BTN interrupt" ees_osbase_3_i/system_ila_0/U0/probe3_1

add_wave -name "AXI_IIC/I2C interrupt" ees_osbase_3_i/system_ila_0/U0/probe14_1
add_wave -name "AXI_Uartlite_230k interrupt" ees_osbase_3_i/system_ila_0/U0/probe15_1
add_wave -name "EES TwinSPI interrupt" ees_osbase_3_i/system_ila_0/U0/probe16_1
add_wave -name "PMOD ACL C INT1 interrupt" ees_osbase_3_i/system_ila_0/U0/probe22_1
add_wave -name "PMOD ACL A INT1 interrupt" ees_osbase_3_i/system_ila_0/U0/probe29_1


add_wave_divider -color "#777777" "PROFILER"
add_wave -name "EES Profiler count_A" ees_osbase_3_i/system_ila_0/U0/probe4_1
add_wave -name "EES Profiler count_B" ees_osbase_3_i/system_ila_0/U0/probe5_1
add_wave -name "EES Profiler count_C" ees_osbase_3_i/system_ila_0/U0/probe6_1
add_wave -name "EES Profiler count_D" ees_osbase_3_i/system_ila_0/U0/probe7_1
add_wave -name "EES Profiler count_E" ees_osbase_3_i/system_ila_0/U0/probe8_1
add_wave -name "EES Profiler count_F" ees_osbase_3_i/system_ila_0/U0/probe9_1

add_wave_divider -color "#777777" "UART"
add_wave -name "AXI_UartLite TX" ees_osbase_3_i/system_ila_0/U0/probe11_1
add_wave -name "AXI_UartLite RX" ees_osbase_3_i/system_ila_0/U0/probe13_1
add_wave -color #4040FF -name "AXI_UartLite TX 230k" ees_osbase_3_i/system_ila_0/U0/probe18_1
add_wave -color #4040FF -name "AXI_UartLite RX 230k" ees_osbase_3_i/system_ila_0/U0/probe17_1

add_wave_divider -color "#777777" "SPI"
add_wave -name "SPI SCLK" ees_osbase_3_i/system_ila_0/U0/probe21_1
add_wave -name "SPI MOSI A" ees_osbase_3_i/system_ila_0/U0/probe19_1
add_wave -name "SPI /SS" ees_osbase_3_i/system_ila_0/U0/probe20_1

add_wave_divider -color "#777777" "I2C"
add_wave -name "I2C SDA in A" ees_osbase_3_i/system_ila_0/U0/probe31_1
add_wave -name "I2C SDA in C" ees_osbase_3_i/system_ila_0/U0/probe30_1
add_wave -name "I2C SDA in" ees_osbase_3_i/system_ila_0/U0/probe27_1
add_wave -name "I2C SDA out" ees_osbase_3_i/system_ila_0/U0/probe26_1
add_wave -name "I2C SCL" ees_osbase_3_i/system_ila_0/U0/probe24_1

add_wave_divider -color "#777777" "MB_TRACEPORT"
add_wave -color #FFFF00 -name "MBTRACE_valid_instr" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_valid_instr_1
add_wave -color #8080FF -name "MBTRACE_PC[0:31]" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_pc_1
add_wave -color #4040FF -name "MBTRACE_instruction[0:31]" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_instruction_1
add_wave -color #00FF00 -name "MBTRACE_data_read" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_data_read_1
add_wave -color #00FF00 -name "MBTRACE_data_write" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_data_write_1
add_wave -color #00FF00 -name "MBTRACE_data_address[0:31]" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_data_address_1
add_wave -color #FF0000 -name "MBTRACE_exception_taken" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_exception_taken_1
add_wave -color #FF0000 -name "MBTRACE_exception_type[0:4]" ees_osbase_3_i/system_ila_0/U0/SLOT_1_MBTRACE_exception_kind_1

add_wave_divider -color "#777777" "MISC"
add_wave -name "AXI_GPIO_LED out[3:0]" ees_osbase_3_i/system_ila_0/U0/SLOT_0_GPIO_tri_o_1
add_wave -name "AXI_GPIO Button 3" ees_osbase_3_i/system_ila_0/U0/probe12_1
add_wave -name "EES Profiler subsample" ees_osbase_3_i/system_ila_0/U0/probe10_1
#add_wave -name "Q[15:0] (Counter for sample division with Capture)" ees_osbase_3_i/system_ila_0/U0/probe9_1




cd $old_path
