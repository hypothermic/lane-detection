"""!
@file vpu_driver.py
@author Matthijs Bakker
@brief PYNQ driver code for the IP core
"""

from typing import Final;
from pynq import DefaultIP, Overlay;

class VPUAccelDriverIP(DefaultIP):
    """!
    Driver code for the VPU IP Core.
    
    A simple wrapper class with utility methods to check if the IP is loaded correctly.
    The PYNQ IP base class (DefaultIP) allows us with memory read/write so we don't
    need to implement these actions ourselves.
    """
    
    """!
    The name which was used to instantiate the IP core.
    """
    HW_VPU_IP_NAME : Final = "vpu_accel_0";
        
    """!
    The VLNV (Vendor Library Name Version) identifier of the IP core.
    """
    HW_VPU_IP_VLNV : Final = "com.arobs:lane_detection:vpu_accel:0.1"
    
    """!
    The data registers that are used on the AXI-Lite interface.
    
    Not to be confused with the control & status register that
    resides on the same interface but is implemented by Vitis.
    """
    HW_VPU_REGISTERS : Final = ["in_width", "in_height"];
    
    """!
    Announce to PYNQ that we can bind to any IP with our VLNV identifier.
    """
    bindto = [HW_VPU_IP_VLNV];
    
    def __init__(self, description):
        """!
        Constructor which calls the base class constructor and passes the arguments to it
        """
        super().__init__(description);
        
    def check_integrity(self):
        """!
        Check if the VPU IP core is the right component by chekcing if the registers exist
        """
        for reg in VPUAccelDriverIP.HW_VPU_REGISTERS:
            assert hasattr(super().register_map, reg), "Register {} not found".format(reg);
