#
# This script creates the block design from scratch without
# requiring user action in the Vivado IPI. It connects all
# components and sets memory addresses automatically
#
# It's included from the hardware build script (../hw.tcl)
# but can also be used in a stand-alone fashion by
# sourcing the script (e.g. in Vivado)
#

#
# Create the block design and write metadata to disk
#

create_bd_design bd
current_bd_design bd
current_bd_instance [get_bd_cells [get_bd_cells /]]

#
# Check if our IP Core is found. It should get included
# by the hw.tcl script or other calling scripts
#

if {"" eq [get_ipdefs -all com.arobs:lane_detection:vpu_accel:0.1]} {
	common::send_msg_id "BD_TCL-115" "ERROR" "VPU IP Core not found!"
	return 1
}

#
# Instantiate the Zynq-PS wrapper and a processor reset system
# to generate separate peripheral and interconnect resets signals
#

set processing_system7_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0 ]
set rst_ps7_0_100M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_ps7_0_100M ]

#
# Instantiate the VDMA and connect it to the PS using interconnects
# Interconnect 0 is for MM2S and interconnect 1 is for S2MM
#

set axi_dma_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_dma:7.1 axi_dma_0 ]
set axi_interconnect_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_0 ]
set axi_interconnect_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 axi_interconnect_1 ]

#
# Instantiate the data width converters that convert from the
# hard-IP 32 bit buses to 24 bit (8bpc * 3 channels) pixels
# by removing/adding padding to the data
#

set axis_dwidth_converter_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_dwidth_converter:1.1 axis_dwidth_converter_0 ]
set axis_dwidth_converter_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axis_dwidth_converter:1.1 axis_dwidth_converter_1 ]

#
# Instantiate our VPU IP Core
#

set vpu_accel_0 [ create_bd_cell -type ip -vlnv com.arobs:lane_detection:vpu_accel:0.1 vpu_accel_0 ]

#
# Configure the Zynq-PS using standard settings taken from TUL's
# board support files
#

set_property -dict [ list \
	CONFIG.PCW_ACT_APU_PERIPHERAL_FREQMHZ {666.666687} \
	CONFIG.PCW_ACT_CAN_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_DCI_PERIPHERAL_FREQMHZ {10.158730} \
	CONFIG.PCW_ACT_ENET0_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_ENET1_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_FPGA0_PERIPHERAL_FREQMHZ {100.000000} \
	CONFIG.PCW_ACT_FPGA1_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_FPGA2_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_FPGA3_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_PCAP_PERIPHERAL_FREQMHZ {200.000000} \
	CONFIG.PCW_ACT_QSPI_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_SDIO_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_SMC_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_SPI_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_TPIU_PERIPHERAL_FREQMHZ {200.000000} \
	CONFIG.PCW_ACT_TTC0_CLK0_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ACT_TTC0_CLK1_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ACT_TTC0_CLK2_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ACT_TTC1_CLK0_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ACT_TTC1_CLK1_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ACT_TTC1_CLK2_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ACT_UART_PERIPHERAL_FREQMHZ {10.000000} \
	CONFIG.PCW_ACT_WDT_PERIPHERAL_FREQMHZ {111.111115} \
	CONFIG.PCW_ARMPLL_CTRL_FBDIV {40} \
	CONFIG.PCW_CAN_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_CAN_PERIPHERAL_DIVISOR1 {1} \
	CONFIG.PCW_CLK0_FREQ {100000000} \
	CONFIG.PCW_CLK1_FREQ {10000000} \
	CONFIG.PCW_CLK2_FREQ {10000000} \
	CONFIG.PCW_CLK3_FREQ {10000000} \
	CONFIG.PCW_CPU_CPU_PLL_FREQMHZ {1333.333} \
	CONFIG.PCW_CPU_PERIPHERAL_DIVISOR0 {2} \
	CONFIG.PCW_DCI_PERIPHERAL_DIVISOR0 {15} \
	CONFIG.PCW_DCI_PERIPHERAL_DIVISOR1 {7} \
	CONFIG.PCW_DDRPLL_CTRL_FBDIV {32} \
	CONFIG.PCW_DDR_DDR_PLL_FREQMHZ {1066.667} \
	CONFIG.PCW_DDR_PERIPHERAL_DIVISOR0 {2} \
	CONFIG.PCW_ENET0_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_ENET0_PERIPHERAL_DIVISOR1 {1} \
	CONFIG.PCW_ENET1_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_ENET1_PERIPHERAL_DIVISOR1 {1} \
	CONFIG.PCW_FCLK0_PERIPHERAL_DIVISOR0 {4} \
	CONFIG.PCW_FCLK0_PERIPHERAL_DIVISOR1 {4} \
	CONFIG.PCW_FCLK1_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_FCLK1_PERIPHERAL_DIVISOR1 {1} \
	CONFIG.PCW_FCLK2_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_FCLK2_PERIPHERAL_DIVISOR1 {1} \
	CONFIG.PCW_FCLK3_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_FCLK3_PERIPHERAL_DIVISOR1 {1} \
	CONFIG.PCW_FPGA0_PERIPHERAL_FREQMHZ {100} \
	CONFIG.PCW_FPGA_FCLK0_ENABLE {1} \
	CONFIG.PCW_FPGA_FCLK1_ENABLE {0} \
	CONFIG.PCW_FPGA_FCLK2_ENABLE {0} \
	CONFIG.PCW_FPGA_FCLK3_ENABLE {0} \
	CONFIG.PCW_I2C_PERIPHERAL_FREQMHZ {25} \
	CONFIG.PCW_IOPLL_CTRL_FBDIV {48} \
	CONFIG.PCW_IO_IO_PLL_FREQMHZ {1600.000} \
	CONFIG.PCW_PCAP_PERIPHERAL_DIVISOR0 {8} \
	CONFIG.PCW_QSPI_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_SDIO_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_SMC_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_SPI_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_TPIU_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_UART_PERIPHERAL_DIVISOR0 {1} \
	CONFIG.PCW_UIPARAM_ACT_DDR_FREQ_MHZ {533.333374} \
	CONFIG.PCW_USE_S_AXI_HP0 {1} \
] $processing_system7_0

#
# Configure the VDMA to disable the Scatter Gather engine
# and increase the burst size for higher throughput
#

set_property -dict [ list \
	CONFIG.c_include_sg {0} \
	CONFIG.c_sg_include_stscntrl_strm {0} \
	CONFIG.c_sg_length_width {26} \
	CONFIG.c_mm2s_burst_size {256} \
	CONFIG.c_s2mm_burst_size {256} \
] $axi_dma_0
set_property -dict [ list \
	CONFIG.NUM_MI {1} \
	CONFIG.NUM_SI {2} \
] $axi_interconnect_1

#
# Configure the data width converters to add/remove
# 8 bits of padding to/from the PS side
#

set_property -dict [ list \
	CONFIG.M_TDATA_NUM_BYTES {3} \
	CONFIG.S_TDATA_NUM_BYTES {4} \
] $axis_dwidth_converter_0
set_property -dict [ list \
	CONFIG.HAS_MI_TKEEP {1} \
	CONFIG.M_TDATA_NUM_BYTES {4} \
	CONFIG.S_TDATA_NUM_BYTES {1} \
] $axis_dwidth_converter_1

connect_bd_intf_net -intf_net S00_AXI_1 [get_bd_intf_pins axi_dma_0/M_AXI_MM2S] [get_bd_intf_pins axi_interconnect_1/S00_AXI]
connect_bd_intf_net -intf_net S01_AXI_1 [get_bd_intf_pins axi_dma_0/M_AXI_S2MM] [get_bd_intf_pins axi_interconnect_1/S01_AXI]
connect_bd_intf_net -intf_net axi_dma_0_M_AXIS_MM2S [get_bd_intf_pins axi_dma_0/M_AXIS_MM2S] [get_bd_intf_pins axis_dwidth_converter_0/S_AXIS]
connect_bd_intf_net -intf_net axi_interconnect_0_M00_AXI [get_bd_intf_pins axi_dma_0/S_AXI_LITE] [get_bd_intf_pins axi_interconnect_0/M00_AXI]
connect_bd_intf_net -intf_net axi_interconnect_0_M01_AXI [get_bd_intf_pins axi_interconnect_0/M01_AXI] [get_bd_intf_pins vpu_accel_0/s_axi_control]
connect_bd_intf_net -intf_net axi_interconnect_1_M00_AXI [get_bd_intf_pins axi_interconnect_1/M00_AXI] [get_bd_intf_pins processing_system7_0/S_AXI_HP0]
connect_bd_intf_net -intf_net axis_dwidth_converter_0_M_AXIS [get_bd_intf_pins axis_dwidth_converter_0/M_AXIS] [get_bd_intf_pins vpu_accel_0/in_r]
connect_bd_intf_net -intf_net axis_dwidth_converter_1_M_AXIS [get_bd_intf_pins axi_dma_0/S_AXIS_S2MM] [get_bd_intf_pins axis_dwidth_converter_1/M_AXIS]
connect_bd_intf_net -intf_net processing_system7_0_M_AXI_GP0 [get_bd_intf_pins axi_interconnect_0/S00_AXI] [get_bd_intf_pins processing_system7_0/M_AXI_GP0]
connect_bd_intf_net -intf_net vpu_accel_0_dst [get_bd_intf_pins axis_dwidth_converter_1/S_AXIS] [get_bd_intf_pins vpu_accel_0/out_r]

#
# 125MHz sys clock net
#

connect_bd_net -net processing_system7_0_FCLK_CLK0 [get_bd_pins axi_dma_0/m_axi_mm2s_aclk] [get_bd_pins axi_dma_0/m_axi_s2mm_aclk] [get_bd_pins axi_dma_0/s_axi_lite_aclk] [get_bd_pins axi_interconnect_0/ACLK] [get_bd_pins axi_interconnect_0/M00_ACLK] [get_bd_pins axi_interconnect_0/M01_ACLK] [get_bd_pins axi_interconnect_0/S00_ACLK] [get_bd_pins axi_interconnect_1/ACLK] [get_bd_pins axi_interconnect_1/M00_ACLK] [get_bd_pins axi_interconnect_1/S00_ACLK] [get_bd_pins axi_interconnect_1/S01_ACLK] [get_bd_pins axis_dwidth_converter_0/aclk] [get_bd_pins axis_dwidth_converter_1/aclk] [get_bd_pins processing_system7_0/FCLK_CLK0] [get_bd_pins processing_system7_0/M_AXI_GP0_ACLK] [get_bd_pins processing_system7_0/S_AXI_HP0_ACLK] [get_bd_pins vpu_accel_0/ap_clk] [get_bd_pins rst_ps7_0_100M/slowest_sync_clk]

#
# Connect the processor reset to the reset component
# which will manage the peripheral/interconnect resets
#

connect_bd_net -net processing_system7_0_FCLK_RESET0_N [get_bd_pins processing_system7_0/FCLK_RESET0_N] [get_bd_pins rst_ps7_0_100M/ext_reset_in]

#
# Connect the interconnect reset from the PSR to
# all of the AXI infra components
#

connect_bd_net -net rst_ps7_0_100M_interconnect_aresetn [get_bd_pins axi_interconnect_0/ARESETN] [get_bd_pins axi_interconnect_1/ARESETN] [get_bd_pins axis_dwidth_converter_0/aresetn] [get_bd_pins axis_dwidth_converter_1/aresetn] [get_bd_pins rst_ps7_0_100M/interconnect_aresetn]

#
# Connect the periph reset from the PSR to
# all of the stateful components
#

connect_bd_net -net rst_ps7_0_100M_peripheral_aresetn [get_bd_pins axi_dma_0/axi_resetn] [get_bd_pins axi_interconnect_0/M00_ARESETN] [get_bd_pins axi_interconnect_0/M01_ARESETN] [get_bd_pins axi_interconnect_0/S00_ARESETN] [get_bd_pins axi_interconnect_1/M00_ARESETN] [get_bd_pins axi_interconnect_1/S00_ARESETN] [get_bd_pins axi_interconnect_1/S01_ARESETN] [get_bd_pins vpu_accel_0/ap_rst_n] [get_bd_pins rst_ps7_0_100M/peripheral_aresetn]

#
# System memory needs to start at the very beginning
# of the address space on Cortex 9, so make sure it
# is mapped to 0x0. It's 512MiB long so set the
# range to 0x20000000.
#

create_bd_addr_seg -range 0x20000000 -offset 0x00000000 [get_bd_addr_spaces axi_dma_0/Data_MM2S] [get_bd_addr_segs processing_system7_0/S_AXI_HP0/HP0_DDR_LOWOCM] SEG_processing_system7_0_HP0_DDR_LOWOCM
create_bd_addr_seg -range 0x20000000 -offset 0x00000000 [get_bd_addr_spaces axi_dma_0/Data_S2MM] [get_bd_addr_segs processing_system7_0/S_AXI_HP0/HP0_DDR_LOWOCM] SEG_processing_system7_0_HP0_DDR_LOWOCM

#
# Map the registers from the DMA-control and VPU-control ports to userspace addresses
#

create_bd_addr_seg -range 0x00010000 -offset 0x40400000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs axi_dma_0/S_AXI_LITE/Reg] SEG_axi_dma_0_Reg
create_bd_addr_seg -range 0x00010000 -offset 0x43C00000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs vpu_accel_0/s_axi_control/Reg] SEG_vpu_accel_0_Reg

#
# Connect the Zynq-PS to the Hard-IP DDR3 and IO ports
#

apply_bd_automation -rule xilinx.com:bd_rule:processing_system7 -config {make_external "FIXED_IO, DDR" Master "Disable" Slave "Disable" } [get_bd_cells processing_system7_0]

save_bd_design

common::send_msg_id "BD_TCL-69" "INFO" "Done with integrating the block design"

