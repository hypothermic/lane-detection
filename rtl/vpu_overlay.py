"""!
@file vpu_overlay.py
@author Matthijs Bakker
@brief PYNQ Overlay wrapper for loading the hardware bitstream and handoff files
"""

from typing import Final;
from pynq import DefaultIP, Overlay;
from vpu_driver import VPUAccelDriverIP;

class VPUAccelOverlay(Overlay):
    """!
    PYNQ Overlay wrapper for loading the bitstream and HWH files
    """
   
    """!
    The least required major version number ("X.y")
    """
    VERSION_MAJOR : Final = 0;
    
    """!
    The least required major version number ("x.Y")
    """
    VERSION_MINOR : Final = 1;

    def __init__(self, bitfile, **kwargs):
        """!
        Constructor which calls the base class constructor and passes the arguments to it
        """
        super().__init__(bitfile, **kwargs);
                
    def check_version(self):
        """!
        Verify that the version of the IP core is supported by the driver
        """
        
        """
        This line parses the VLNV of the IP core (e.g. "com.arobs:lane_detection:vpu_accel:0.1")
        into a list with the major and minor version numbers and returns them as variables
        """
        major, minor = list(map(int, self.ip_dict[VPUAccelDriverIP.HW_VPU_IP_NAME]["type"].split(":")[-1].split(".")));

        """!
        Check if the VPU IP core version is equal or greater than the required version
        """
        assert major >= VPUAccelOverlay.VERSION_MAJOR, "VPU Core major version mismatch";
        assert minor >= VPUAccelOverlay.VERSION_MINOR, "VPU Core minor version mismatch";
