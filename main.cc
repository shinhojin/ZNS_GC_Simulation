#include "zns_simulation.h"
#include <string>

//Argument
#define ARGUMENT_COUNT 7      //All Arugment + filename
#define DEV_NAME 1
#define ZONE_COUNT 2
#define SETTING_DEV_UTILIZATION 3
#define GC_NUMBER 4
#define WORKLOAD_NUM 5
#define DEV_NUM 6

using namespace std;

int main(int argc, char * argv[]) {
    ZNS_Simulation * zns_simulation;

    if(argc != ARGUMENT_COUNT) {
        cout<< "Please, set argument" <<endl;
        cout<< "---> sudo ./Simulation {DEV_NAME} {DEV_NUM(1. M2, 2. U3)} {ZONE_COUNT(MAX_512)} {SETTING_ZONE_UTILIZATION(%)} {GC_NUMBER(1.BASIC_GC, 2.LSM_ZGC)} {WORKLOAD_TYPE(1.SEQ, 2.RAND, 3.ZIPFIAN)} " <<endl;
        cout<< "---> example : sudo ./Simulation /dev/nvme0n1 2 512 60.0 1 2" <<endl;
        exit(0);
    }
    
    //Print Setting Argument List
    cout<< endl << "Start Simulation!!" <<endl;
    cout<< "This Simulation is done in a single thread." <<endl;
    cout<< "Hot/Cold Standard = 70% Invalid Block(Hot)/ 30% Invaild Block(Cold)" <<endl;
    cout<< "------------------------------------------------------" <<endl;
    cout<< "Device Name : " << argv[DEV_NAME] <<endl;
    cout<< "------------------------------------------------------" <<endl<<endl;

    //1. Init ZNS SSD
    cout<< "1. Start Init ZNS SSD Simulation" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        zns_simulation = new ZNS_Simulation(argv[DEV_NAME], atoi(argv[ZONE_COUNT]), atoi(argv[SETTING_DEV_UTILIZATION]), atoi(argv[DEV_NUM]));
        //zns_simulation->print_segment_block_bitmap(0);
        
    cout<< "------------------------------------------------------" <<endl<<endl;
    
    //2. Create Workload
    cout<< "2. Start Create Workload" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        if (atoi(argv[WORKLOAD_NUM]) == 1) {
            cout << "Do Sequential" << endl;
            //zns_simulation->request_sequential_workload();
        } else if (atoi(argv[WORKLOAD_NUM]) == 2) {
            cout << "Do Random" << endl;
            //zns_simulation->request_random_workload();
        } else if (atoi(argv[WORKLOAD_NUM]) == 3) {
            cout << "Do Zipfian" << endl;
            //zns_simulation->request_zipfian_workload();
        }
        //workload_creator.get_workload_job(argv[SETTING_WORKLOAD_FILE]);
        //workload_creator.print_all_workload();
    cout<< "------------------------------------------------------" <<endl<<endl;

    //3. Start Workload Simualtion in memory.
    cout<< "3. Start Workload Simualtion in memory" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        cout << "Do Update" << endl;
        //zns_simulation->request_update_workload();
        //zns_simulation->print_segment_block_bitmap(0);
    cout<< "------------------------------------------------------" <<endl<<endl;

    //4. Start GC Simulation in ZNS SSD
    cout<< "4. Start GC Simulation in ZNS SSD" <<endl;
    cout<< "------------------------------------------------------" <<endl;
        if (atoi(argv[GC_NUMBER]) == 1) {
            cout << "Do Basic_zgc" << endl;
            //zns_simulation->basic_zgc();
        } else if (atoi(argv[GC_NUMBER]) == 2) {
            cout << "Do lsm_zgc" << endl;
            //zns_simulation->lsm_zgc();
        }
    cout<< "------------------------------------------------------" <<endl<<endl;












    //Test
    //zns_simulation->print_zone_block_bitmap(0);
    //zns_simulation->print_segment_block_bitmap(0);
    //zns_simulation->print_zone_segment_bitmap(0);
    //zns_simulation->setting_random_bitmap();

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
