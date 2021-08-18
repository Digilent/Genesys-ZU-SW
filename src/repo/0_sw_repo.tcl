# This script will add a path to the workspace software repositories
# It seems that repo -set needs platform-specific path separators, ie. \ on Windows
# Workspace should be set externally

set script [info script] 
set script_dir [file normalize [file dirname $script]]

puts "INFO: Running $script"

set repo_dir [file normalize $script_dir/../../repo]

if { [file exists $repo_dir] && [llength [glob -dir $repo_dir -nocomplain -join *]] != 0 &&
    [llength [glob -dir $repo_dir -nocomplain -join * *]] != 0 } {
	puts "INFO: Adding $repo_dir to software repositories."
	repo -set [file nativename $repo_dir]
	repo -scan
} else {
	return -code error "Repo directory $repo_dir is inexistent or empty. Perhaps you forgot to check out the submodule? Try git submodule update."
}