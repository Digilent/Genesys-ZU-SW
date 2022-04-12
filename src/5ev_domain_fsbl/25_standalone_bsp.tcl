# This script will create a standalone domain for a processor and its architecture
# Workspace should be set externally

set script [info script] 
set script_dir [file normalize [file dirname $script]]

puts "INFO: Running $script"

set domain_name [file tail $script_dir]

# Modify these for custom domain/BSP settings
set arch "64-bit"
set os "standalone"
set proc "psu_cortexa53_0"

# Destination platform needs to be made active first
platform active "5ev_hw_pf"

domain create -name $domain_name -proc $proc -arch $arch -os $os

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
bsp config zynqmp_fsbl_bsp "true"
bsp config archiver "aarch64-none-elf-ar"
bsp config assembler "aarch64-none-elf-as"
bsp config compiler "aarch64-none-elf-gcc"
bsp config compiler_flags "-O2 -c"
bsp config exec_mode "aarch64"
bsp config extra_compiler_flags "-g -Wall -Wextra -DUHS_MODE_ENABLE"

bsp setlib -name xilffs
bsp config read_only "true"
bsp config use_mkfs "false"

bsp setlib -name xilsecure
bsp setlib -name xilpm