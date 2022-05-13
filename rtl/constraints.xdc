# This file is used for mapping the logical pins to the FPGA pins

# UART
set_property -dict { PACKAGE_PIN W14 IOSTANDARD LVCMOS33 } [get_ports { uart_rtl_0_rxd }];
set_property -dict { PACKAGE_PIN Y14 IOSTANDARD LVCMOS33 } [get_ports { uart_rtl_0_txd }];

# LEDs
set_property -dict { PACKAGE_PIN L15 IOSTANDARD LVCMOS33 } [get_ports { gpio_rtl_0_tri_o[0] }];
set_property -dict { PACKAGE_PIN G17 IOSTANDARD LVCMOS33 } [get_ports { gpio_rtl_0_tri_o[1] }];
set_property -dict { PACKAGE_PIN N15 IOSTANDARD LVCMOS33 } [get_ports { gpio_rtl_0_tri_o[2] }];
set_property -dict { PACKAGE_PIN G14 IOSTANDARD LVCMOS33 } [get_ports { gpio_rtl_0_tri_o[3] }];
set_property -dict { PACKAGE_PIN L14 IOSTANDARD LVCMOS33 } [get_ports { gpio_rtl_0_tri_o[4] }];
set_property -dict { PACKAGE_PIN M15 IOSTANDARD LVCMOS33 } [get_ports { gpio_rtl_0_tri_o[5] }];

