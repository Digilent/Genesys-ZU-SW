# This script will create a standalone domain for a processor and its architecture
# Workspace should be set externally

set script [info script] 
set script_dir [file normalize [file dirname $script]]

puts "INFO: Running $script"

set domain_name [file tail $script_dir]

# Modify these for custom domain/BSP settings
set os "standalone"
set proc "psu_pmu_0"

# Destination platform needs to be made active first
platform active "5ev_hw_pf"

domain create -name $domain_name -proc $proc -os $os

# Customize BSP, this replaces *.mss file
bsp config clocking "false"
bsp config enable_sw_intrusive_profiling "false"
bsp config hypervisor_guest "false"
bsp config lockstep_mode_debug "false"
bsp config microblaze_exceptions "false"
bsp config predecode_fpu_exceptions "false"
bsp config profile_timer "none"
bsp config sleep_timer "none"
bsp config stdin "psu_uart_0"
bsp config stdout "psu_uart_0"
bsp config ttc_select_cntr "2"
bsp config zynqmp_fsbl_bsp "false"
bsp config archiver "mb-ar"
bsp config assembler "mb-as"
bsp config compiler "mb-gcc"
bsp config compiler_flags "-O2 -c"
bsp config extra_compiler_flags "-g -ffunction-sections -fdata-sections -Wall -Wextra"

bsp setlib -name xilsecure
bsp setlib -name xilfpga
bsp setlib -name xilskey