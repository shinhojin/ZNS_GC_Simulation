# Simulator_LSM_ZGC

Quick Start
============

This section describes how to run ZNS_GC_Simulation on Linux locally.

Note: This tool 

#1. Set Up ZNS_GC_Simulation
---------------------------

Clone the ZNS_GC_Simulation git repository and compile:

    git clone https://github.com/shinhojin/ZNS_GC_Simulation.git

    cd ZNS_GC_Simulation

    make clean

    make

#2. Run ZNS_GC_Simulation
-------------------------

Now you are ready to run ZNS_GC_Simulation! Let's learn how to use first.


    sudo ./Simulation {DEV_NAME} {DEV_NUM(1. M2, 2. U3)} {ZONE_COUNT} {SETTING_ZONE_UTILIZATION(%)} {GC_NUMBER(BASIC_ZGC/LSM_ZGC)} {WORKLOAD_TYPE(SEQ/RAND}

The commands shown below are examples of how to use them.

    sudo ./Simulation /dev/nvme0n1 2 512 60.0 BASIC_ZGC RAND


ZNS_GC_Simulation Configuration Parameters
==========================================

+ DEV_NAME - (required) A path to what device you use
    - EX. /dev/nvme0n1

+ DEV_NUM - (required) What kind of SSD you connect
    - EX. 1 (M2) / 2 (U3)

+ ZONE_COUNT - (required) How many zones to fill and start
    - EX. 512 (M2 - Max zone 530) / 15432 (U3 - Max zone 29172)

+ SETTING_ZONE_UTILIZATION - (required) How much to set the utility for each zone
    - EX. 60.0

+ GC_NUMBER - (required) Which garbage collection function to use
    - EX. BASIC_ZGC

+ WORKLOAD_TYPE - (required) Which workload to use for the zone, update method
    - EX. RAND

Note on ZNS_GC_Simulation
=========================

If you have any questions about this tool, please contact us at the address below.

E-MAIL : ghwls03s@gmail.com or hj03s@naver.com
