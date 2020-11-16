#include "workload_creator.h"
#include "zns_simulation.h"
#include <map>
#include <string>

//Argument
#define ARGUMENT_COUNT 4        //All Arugment + filename
#define DEV_NAME 1
#define SETTING_ZONE_COUNT 2
#define SETTING_WORKLOAD_FILE 3

using namespace std;

int main(int argc, char * argv[]) {
    ZNS_Simulation * zns_simulation;
    Workload_Creator workload_creator;
    list<Workload_Data * > * workload_list;

    if(argc != ARGUMENT_COUNT) {
        cout<< "Please, set argument\n" <<endl;
        cout<< "---> sudo ./Simulation {DEV_NAME} {SETTING_ZONE_COUNT} {SETTING_WORKLOAD_FILE}" <<endl;
        cout<< "---> example : sudo ./Simulation /dev/nvme0n1 530 ./test_data/Entries_100000.txt" <<endl;
    }
    
    //Print Setting Argument List
    cout<< "Start Simulation!!" <<endl;
    cout<< "------------------------------------------------------" <<endl;
    cout<< "Device Name : " << argv[DEV_NAME] <<endl;
    cout<< "Zone Count : " << argv[SETTING_ZONE_COUNT] <<endl;
    cout<< "Workload File : " << argv[SETTING_WORKLOAD_FILE] <<endl;
    cout<< "------------------------------------------------------" <<endl<<endl;

    //1. Create Workload
    cout<< "1. Start Create Workload!!" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        workload_creator.get_workload_job(argv[SETTING_WORKLOAD_FILE]);
        //workload_creator.print_all_workload();
    cout<< "------------------------------------------------------" <<endl<<endl;

    //2. Init ZNS SSD
    cout<< "2. Start Init ZNS SSD Simulation" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        zns_simulation = new ZNS_Simulation(argv[DEV_NAME], atoi(argv[SETTING_ZONE_COUNT]), workload_creator.getWorkloadlist());
        
    cout<< "------------------------------------------------------" <<endl<<endl;

    //3. Start Workload Simualtion in memory
    cout<< "3. Start Workload Simualtion in memory" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        //Simulation Random workload Test (not realworkload)
        //zns_simulation->setting_random_bitmap();
        //zns_simulation->print_segment_block_bitmap(0);
        //zns_simulation->print_zone_block_bitmap(0);
        //zns_simulation->print_zone_segment_bitmap(0);
        

    cout<< "------------------------------------------------------" <<endl<<endl;

    //4. Start GC Simulation in ZNS SSD
    cout<< "4. Start GC Simulation in ZNS SSD" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        zns_simulation->basic_zgc();
        //zns_simulation->lsm_zgc();
    cout<< "------------------------------------------------------" <<endl<<endl;












    //Test
    //zns_simulation->print_zone_block_bitmap(0);
    //zns_simulation->print_segment_block_bitmap(0);
    //zns_simulation->print_zone_segment_bitmap(0);
    zns_simulation->setting_random_bitmap();

    //Init Simulation
    //zns_simulation = new ZNS_Simulation("/dev/nvme0n1", workload_creator.getWorkloadlist());


    //zns_simulation->init_block_bitmap();
    //zns_simulation->init_zone_reset(1);
    //zns_simulation->init_zones_write(1);
    //zns_simulation->print_bitmap(1);
    
    //zns_simulation->request_write(100, 16);
    //zns_simulation->request_write(100, 12);

    /*
    workload_list = workload_creator.getWorkloadlist();
    workload_list->front()->print_workload_data();
    */

    return 0;
}
