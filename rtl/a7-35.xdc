# Pin constraints for Arty A7-35 rev. D

# sys_clk
set_property -dict { PACKAGE_PIN E3    IOSTANDARD LVCMOS33 } [get_ports { sys_clk }]; #IO_L12P_T1_MRCC_35 Sch=gclk[100]
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports { sys_clk }];

set_property -dict { PACKAGE_PIN H5    IOSTANDARD LVCMOS33 } [get_ports { led }]; #IO_L24N_T3_35 Sch=led[4]
set_property -dict { PACKAGE_PIN A8    IOSTANDARD LVCMOS33 } [get_ports { sw }]; #IO_L12N_T1_MRCC_16 Sch=sw[0]
