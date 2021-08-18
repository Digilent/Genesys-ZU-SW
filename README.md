# Genesys-ZU Software Repository


This repository contains Vitis workspaces for all software demos for the Genesys ZU.


Each demo contained in this repository is documented on the Digilent Wiki, links in the table below.

| Name and Wiki Link | Description |
|--------------------|-------------|
| [Genesys ZU-5EV HDMI Demo](https://reference.digilentinc.com/learn/programmable-logic/tutorials/genesys-zu-5ev-demo-hdmi/start) | A project that demonstrates the usage of the Genesys ZU-5EV's HDMI in and HDMI out ports.  |
| [Genesys ZU-5EV Hello World Demo](https://reference.digilentinc.com/learn/programmable-logic/tutorials/genesys-zu-demo-hello-world/start) | Simple project that configures the Zynq Ultrascale+ MPSoc with the given board file, and outputs "Hello World" on the serial terminal.  |
| [Genesys ZU-3EG Hello World Demo](https://reference.digilentinc.com/learn/programmable-logic/tutorials/genesys-zu-demo-hello-world/start) | Simple project that configures the Zynq Ultrascale+ MPSoc with the given board file, and outputs "Hello World" on the serial terminal.  |

For more information about the Genesys-ZU , visit its [Resource Center](https://reference.digilentinc.com/programmable-logic/genesys-zu/start) on the Digilent Wiki.

## Repository Description

This repository contains the Vitis workspace and software sources for all of the software demos that we provide for the Genesys-ZU. As each of these demos also requires a hardware design contained in a Vivado project, this repository should not be used directly. The [Genesys-ZU](https://github.com/Digilent/Genesys-ZU) repository contains all sources for these demos across all tools, and pulls in all of this repository's sources by using it as a submodule.

For instructions on how to use this repository with git, and for additional documentation on the submodule and branch structures used, please visit [Digilent FPGA Demo Git Repositories](https://reference.digilentinc.com/reference/programmable-logic/documents/git) on the Digilent Wiki. Note that use of git is not required to use this demo.
