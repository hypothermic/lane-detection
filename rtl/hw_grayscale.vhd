library ieee;
use ieee.std_logic_1164.all;

entity hw_grayscale is
	port (
		clk : out std_logic
	);
end hw_grayscale;

architecture behavioral of hw_grayscale is
	constant clk_period : time := 20 ns;
begin
	clk_process: process
	begin
		clk <= '0';
		wait for clk_period/2;
		clk <= '1';
		wait for clk_period/2;
	end process;
end behavioral;
