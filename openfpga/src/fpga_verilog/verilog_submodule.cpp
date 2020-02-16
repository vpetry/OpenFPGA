/*********************************************************************
 * This file includes top-level function to generate Verilog primitive modules
 * and print them to files
 ********************************************************************/

/* Headers from vtrutil library */
#include "vtr_assert.h"
#include "vtr_log.h"

#include "verilog_submodule_utils.h"
#include "verilog_essential_gates.h"
#include "verilog_decoders.h"
#include "verilog_mux.h"
#include "verilog_lut.h"
#include "verilog_wire.h"
#include "verilog_memory.h"
#include "verilog_writer_utils.h"

#include "verilog_constants.h"
#include "verilog_submodule.h"

/* begin namespace openfpga */
namespace openfpga {

/*********************************************************************
 * Top-level function to generate primitive modules:
 * 1. Logic gates: AND/OR, inverter, buffer and transmission-gate/pass-transistor
 * 2. Routing multiplexers
 * 3. Local encoders for routing multiplexers
 * 4. Wires
 * 5. Configuration memory blocks
 * 6. Verilog template
 ********************************************************************/
void print_verilog_submodule(ModuleManager& module_manager, 
                             const MuxLibrary& mux_lib,
                             const CircuitLibrary& circuit_lib, 
                             const std::string& verilog_dir, 
                             const std::string& submodule_dir, 
                             const FabricVerilogOption& fpga_verilog_opts) {

  /* Register all the user-defined modules in the module manager
   * This should be done prior to other steps in this function, 
   * because they will be instanciated by other primitive modules 
   */
  //add_user_defined_verilog_modules(module_manager, circuit_lib);

  /* Create a vector to contain all the Verilog netlist names that have been generated in this function */
  std::vector<std::string> netlist_names;


  print_verilog_submodule_essentials(const_cast<const ModuleManager&>(module_manager), 
                                     netlist_names,
                                     verilog_dir, 
                                     submodule_dir,
                                     circuit_lib);

  /* Routing multiplexers */
  /* NOTE: local decoders generation must go before the MUX generation!!! 
   *       because local decoders modules will be instanciated in the MUX modules 
   */
  print_verilog_submodule_mux_local_decoders(const_cast<const ModuleManager&>(module_manager),
                                             netlist_names, 
                                             mux_lib, circuit_lib, 
                                             verilog_dir, submodule_dir);
  print_verilog_submodule_muxes(module_manager, netlist_names, mux_lib, circuit_lib,
                                verilog_dir, submodule_dir,
                                fpga_verilog_opts.explicit_port_mapping());

 
  /* LUTes */
  print_verilog_submodule_luts(const_cast<const ModuleManager&>(module_manager),
                               netlist_names, circuit_lib,
                               verilog_dir, submodule_dir,
                               fpga_verilog_opts.explicit_port_mapping());

  /* Hard wires */
  print_verilog_submodule_wires(const_cast<const ModuleManager&>(module_manager),
                                netlist_names, circuit_lib,
                                verilog_dir, submodule_dir);

  /* 4. Memories */
  print_verilog_submodule_memories(const_cast<const ModuleManager&>(module_manager),
                                   netlist_names,
                                   mux_lib, circuit_lib, 
                                   verilog_dir, submodule_dir,
                                   fpga_verilog_opts.explicit_port_mapping());

  /* 5. Dump template for all the modules */
  if (true == fpga_verilog_opts.print_user_defined_template()) { 
    print_verilog_submodule_templates(const_cast<const ModuleManager&>(module_manager),
                                      circuit_lib,
                                      verilog_dir, submodule_dir);
  }

  /* Create a header file to include all the subckts */
  print_verilog_netlist_include_header_file(netlist_names,
                                            submodule_dir.c_str(),
                                            SUBMODULE_VERILOG_FILE_NAME);
}

} /* end namespace openfpga */
