set old_path [ pwd ]
set script_path [file normalize [info script]]
set script_folder [file dirname $script_path]
cd $script_folder
cd ..

puts "###################################"
puts "make/set workspace  ./vitis_project"
puts "###################################"
puts ""
after 1000

setws -switch ./vitis_project_ees

puts "########################"
puts "create FreeRTOS platform"
puts "########################"
puts ""
after 1000
platform create -name "B_project_ees_freertos" -hw ./hw/ees_osbase_3_wrapper.xsa -os standalone
platform active B_project_ees_freertos
domain create -name freertos10_xilinx_domain -os freertos10_xilinx -proc microblaze_0 -display-name "freertos10_xilinx on microblaze_0"
bsp config use_idle_hook true
platform generate -domains freertos10_xilinx_domain

puts "#####################"
puts "create 2 applications"
puts "#####################"
puts ""

puts "Adding app  B1_project_signalgen..."
app create -name B1_project_signalgen -platform B_project_ees_freertos -domain freertos10_xilinx_domain -proc microblaze_0 -template "Empty Application(C)" -sysproj B1_project_signalgen_system
app config -name B1_project_signalgen -set linker-misc -Wl,--gc-keep-exported

puts "Adding app  B2_project_game..."
app create -name B2_project_game -platform B_project_ees_freertos -domain freertos10_xilinx_domain -proc microblaze_0 -template "Empty Application(C)" -sysproj B2_project_game_system
app config -name B2_project_game -set linker-misc -Wl,--gc-keep-exported


cd $script_folder
cd $old_path