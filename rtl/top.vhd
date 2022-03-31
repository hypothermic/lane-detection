--!
--! @file top.vhd
--! @author Matthijs Bakker
--! @brief Top-level module for the hardware definition
--!
--! This module instantiates the Zynq 7 Processor System
--! and connects the hardwired IO and memory channels
--! to the relevant PS IP Core via the block design wrapper
--!

library ieee;
use ieee.std_logic_1164.all;

--! @brief Top level module entity
entity top is
	port (
		DDR_addr : inout std_logic_vector(15-1 downto 0);
		DDR_ba : inout std_logic_vector(3-1 downto 0);
		DDR_cas_n : inout std_logic;
		DDR_ck_n : inout std_logic;
		DDR_ck_p : inout std_logic;
		DDR_cke : inout std_logic;
		DDR_cs_n : inout std_logic;
		DDR_dm : inout std_logic_vector(4-1 downto 0);
		DDR_dq : inout std_logic_vector(32-1 downto 0);
		DDR_dqs_n : inout std_logic_vector(4-1 downto 0);
		DDR_dqs_p : inout std_logic_vector(4-1 downto 0);
		DDR_odt : inout std_logic;
		DDR_ras_n : inout std_logic;
		DDR_reset_n : inout std_logic;
		DDR_we_n : inout std_logic;
		FIXED_IO_ddr_vrn : inout std_logic;
		FIXED_IO_ddr_vrp : inout std_logic;
		FIXED_IO_mio : inout std_logic_vector(54-1 downto 0);
		FIXED_IO_ps_clk : inout std_logic;
		FIXED_IO_ps_porb : inout std_logic;
		FIXED_IO_ps_srstb : inout std_logic
	);
end top;

--! @brief The entity has a structural architecture with
--!        the IBD wrapper as underlying component
architecture structural of top is
	component bd_wrapper is
		port (
			DDR_addr : inout std_logic_vector(15-1 downto 0);
			DDR_ba : inout std_logic_vector(3-1 downto 0);
			DDR_cas_n : inout std_logic;
			DDR_ck_n : inout std_logic;
			DDR_ck_p : inout std_logic;
			DDR_cke : inout std_logic;
			DDR_cs_n : inout std_logic;
			DDR_dm : inout std_logic_vector(4-1 downto 0);
			DDR_dq : inout std_logic_vector(32-1 downto 0);
			DDR_dqs_n : inout std_logic_vector(4-1 downto 0);
			DDR_dqs_p : inout std_logic_vector(4-1 downto 0);
			DDR_odt : inout std_logic;
			DDR_ras_n : inout std_logic;
			DDR_reset_n : inout std_logic;
			DDR_we_n : inout std_logic;
			FIXED_IO_ddr_vrn : inout std_logic;
			FIXED_IO_ddr_vrp : inout std_logic;
			FIXED_IO_mio : inout std_logic_vector(54-1 downto 0);
			FIXED_IO_ps_clk : inout std_logic;
			FIXED_IO_ps_porb : inout std_logic;
			FIXED_IO_ps_srstb : inout std_logic
		);
	end component bd_wrapper;
begin
	u_bd_wrapper: component bd_wrapper
		port map (
			DDR_addr(15-1 downto 0) => DDR_addr(14 downto 0),
			DDR_ba(3-1 downto 0) => DDR_ba(3-1 downto 0),
			DDR_cas_n => DDR_cas_n,
			DDR_ck_n => DDR_ck_n,
			DDR_ck_p => DDR_ck_p,
			DDR_cke => DDR_cke,
			DDR_cs_n => DDR_cs_n,
			DDR_dm(4-1 downto 0) => DDR_dm(4-1 downto 0),
			DDR_dq(32-1 downto 0) => DDR_dq(32-1 downto 0),
			DDR_dqs_n(4-1 downto 0) => DDR_dqs_n(4-1 downto 0),
			DDR_dqs_p(4-1 downto 0) => DDR_dqs_p(4-1 downto 0),
			DDR_odt => DDR_odt,
			DDR_ras_n => DDR_ras_n,
			DDR_reset_n => DDR_reset_n,
			DDR_we_n => DDR_we_n,
			FIXED_IO_ddr_vrn => FIXED_IO_ddr_vrn,
			FIXED_IO_ddr_vrp => FIXED_IO_ddr_vrp,
			FIXED_IO_mio(54-1 downto 0) => FIXED_IO_mio(54-1 downto 0),
			FIXED_IO_ps_clk => FIXED_IO_ps_clk,
			FIXED_IO_ps_porb => FIXED_IO_ps_porb,
			FIXED_IO_ps_srstb => FIXED_IO_ps_srstb
		);
end structural;

