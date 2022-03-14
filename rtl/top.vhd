library ieee;
use ieee.std_logic_1164.all;

entity top is
	port (
		led : out std_logic;
		sw : in std_logic
	);
end top;

architecture structural of top is
begin
	led <= sw;
end structural;

