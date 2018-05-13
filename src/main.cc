#include "FileReader.h"
#include "AntColonyCore.h"
#include "config.h"



int main(int argc, char *argv[])
{
    Scheduler::FileReader* p_reader;
    Scheduler::Config* config_ptr = new Scheduler::Config(std::string("/home/liu/workspace/ant_colony_scheduler/setting.txt"));
    if (argc == 2)
    {
        p_reader = new Scheduler::FileReader(argv[1], config_ptr->all_core_num);
    }
    else
    {
        p_reader = new Scheduler::FileReader(std::string("/home/liu/workspace/ant_colony_scheduler/STG/50/rand0003.stg"), config_ptr->all_core_num);
    }
    auto p0 = std::chrono::system_clock::now();
    Scheduler::AntColonySchedulerCore scheduler(p_reader->getNodes(), config_ptr);
    scheduler.run();
    auto p1 = std::chrono::system_clock::now();
    auto diff1 = p1 - p0;
    std::cout << "time:" << std::chrono::duration_cast<std::chrono::milliseconds>(diff1).count() << " millisec.  "
              << "best: " << scheduler.getBest() << std::endl;

}
