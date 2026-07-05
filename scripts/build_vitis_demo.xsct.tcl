set old_path [ pwd ]
set script_path [file normalize [info script]]
set script_folder [file dirname $script_path]
cd $script_folder
cd ..

puts "########################################"
puts "make/set workspace  ./vitis_project_demo"
puts "########################################"
puts ""
after 1000

setws -switch ./vitis_project_demo

puts "##############################"
puts "create standalone platform/BSP"
puts "##############################"
puts ""
after 1000

platform create -name "A_osbase_3_standalone" -hw ./hw/ees_osbase_3_wrapper.xsa -os standalone
platform active A_osbase_3_standalone
domain create -name standalone_domain -os standalone -proc microblaze_0 -display-name "standalone on microblaze_0"
platform generate

puts "##########################"
puts "create 3 demo applications"
puts "##########################"
puts ""

puts "Adding app  A1_SPI_DA2_demo..."
app create -name A1_SPI_DA2_demo -platform A_osbase_3_standalone -domain standalone_domain -proc microblaze_0 -template "Empty Application(C)" -sysproj A1_SPI_DA2_demo_system
app config -name A1_SPI_DA2_demo -set linker-misc -Wl,--gc-keep-exported

puts "Adding app  A2_I2C_ACL_demo..."
app create -name A2_I2C_ACL_demo -platform A_osbase_3_standalone -domain standalone_domain -proc microblaze_0 -template "Empty Application(C)" -sysproj A2_I2C_ACL_demo_system
app config -name A2_I2C_ACL_demo -set linker-misc -Wl,--gc-keep-exported

puts "Adding app  A3_UARTscreen_demo..."
app create -name A3_UARTscreen_demo -platform A_osbase_3_standalone -domain standalone_domain -proc microblaze_0 -template "Empty Application(C)" -sysproj A3_UARTscreen_demo_system
app config -name A3_UARTscreen_demo -set linker-misc -Wl,--gc-keep-exported

puts ""
puts ""
puts "WARNING: csrc folder links and other settings need to be set manually because Eclipse sucks."
puts ""


cd $script_folder
cd $old_path