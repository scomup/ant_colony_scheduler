#include <iostream>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "AntColonyCore.h"

namespace Scheduler
{

AntColonySchedulerCore::AntColonySchedulerCore(std::vector<Node> nodes, Scheduler::Config *config_ptr)
    : nodes_(nodes), config_ptr_(config_ptr)
{
    ant_routing_table_.resize( nodes_.size() , std::vector<double>( nodes_.size() , 0 ) );
    init_ant_table();
    home_ = std::make_shared<AntTrail>(0);
    home_->update_schedulable_tasks(nodes_);
    //update_roulettes();
    Roulette r = Roulette(ant_routing_table_[50],1.);
    int i = 0;
    for (size_t j = 0; j < ant_routing_table_[50].size(); ++j)
    {
        std::cout << std::setw(3) << ant_routing_table_[50][j] << " ";
    }
    std::cout << "\n";
    std::vector<int> count(52, 0);
    while (i++ < 100000)
        count[r.spin_roulette()]++;

    for (size_t j = 0; j < count.size(); ++j)
    {
        std::cout << j << ":" << count[j] << "\n";
    }
};

void AntColonySchedulerCore::add_flag(int16_t a, int16_t b)
{
    if (a == b)
    {
        ant_routing_table_[a][a] = std::numeric_limits<float>::quiet_NaN();
    }
    for (int16_t c : nodes_[b].parent_nodes)
    {
        ant_routing_table_[a][c] = std::numeric_limits<float>::quiet_NaN();
        if (c == 0)
            return;
        add_flag(a, c);
    }
}

void AntColonySchedulerCore::init_ant_table()
{
    for (size_t i = 0; i < ant_routing_table_.size(); ++i)
    {
        add_flag(i, i);
    }
    
    //for (size_t i = 0; i < ant_routing_table_.size(); ++i)
    //{
    //    std::cout <<std::setw(2)<<i;
    //    for (size_t j = 0; j < ant_routing_table_[i].size(); ++j)
    //    {
    //        std::cout <<std::setw(3)<< ant_routing_table_[i][j] << " ";
    //    }
    //    std::cout << "\n";
    //}
    
}

void AntColonySchedulerCore::run()
{
    /*
    home_ = std::make_shared<AntTrail>(0);

    for (uint32_t loop = 0; loop < config_ptr_->max_loop; loop++)
    {
        for (size_t i = 0; i < config_ptr_->max_chromosome_num; i++)
        {
            auto trail = explore();
            trails_.push_back(trail);
        }
        update_pheromone();
        std::cout<<home_->update_score()<<std::endl;
    }*/
}

void AntColonySchedulerCore::update_roulettes(){
    return;

}

void AntColonySchedulerCore::ant_explore(std::shared_ptr<AntTrail> ant)
{

    //ant.
    /*
    auto trail = home_;
    while ((uint32_t)trail->taskID_ != nodes_.size() - 1)
    {
        if (trail->nextPath_.size() == 0)
        {
            trail->create_next_trail(nodes_, config_ptr_->alpha);
        }
        int idx = trail->roulette_->spin_roulette();
        trail = trail->nextPath_[idx];
    }
    return trail;
    */

}

void AntColonySchedulerCore::update_pheromone()
{
    /*
    for (std::shared_ptr<AntTrail> trail : trails_)
    {
        uint32_t scheduling_length = evaluate(trail->task_sequence);
        while (true)
        {
            trail->scores_[0] = std::min(trail->scores_[0], scheduling_length);
            if(trail->prevPath_ == nullptr)
                break;
            trail = trail->prevPath_;
        }
    }

    for (std::shared_ptr<AntTrail> trail : trails_)
    {
        while (trail->scores_[0] != max_score)
        {
            trail->scores_.insert(trail->scores_.begin(),max_score);
            if(trail->nextPath_.size() != 0)
                trail->roulette_->update_roulette(trail->nextPath_);
            if(trail->prevPath_ == nullptr)
                break;
            trail = trail->prevPath_; 
        }
    }*/
}


//-----------------------------------------------------------------------------
//This funciton try to assembly of the scheduling result according to the genetic information.

//We choose node by scheduling order carried by chromosome in each step.
//We calcuate the finished time for this node_i.
//  .time_a = Max(the finished time of all parent task of node_i)
//  .time_b = The most recent time when system can provide enough core resources for this node.
//  .The finished time of this node = Max(time_a, time_a) + the runing time of this node.
//Update the occupied time of selected cores.
//Finished if all node be processed.
//Return the longest occupied time of cores.
//-----------------------------------------------------------------------------

int16_t AntColonySchedulerCore::evaluate(std::vector<int16_t> &task_sequence)
{
    std::vector<int16_t> nodes_finish_time(nodes_.size());
    std::vector<int16_t> cores_ocuppied_time(config_ptr_->all_core_num);

    std::fill(nodes_finish_time.begin(), nodes_finish_time.end(), inf);
    std::fill(cores_ocuppied_time.begin(), cores_ocuppied_time.end(), 0);

    for (int16_t i : task_sequence)
    {
        int16_t schedulabe_time = 0;
        for (size_t j = 0; j < nodes_[i].parent_nodes.size(); j++)
        {
            int16_t sub_node_finished_time = nodes_finish_time[nodes_[i].parent_nodes[j]];
            assert(sub_node_finished_time != inf);
            schedulabe_time = std::max(sub_node_finished_time, schedulabe_time);
        }

        int16_t core_num = nodes_[i].core_num;
        std::sort(cores_ocuppied_time.begin(), cores_ocuppied_time.end());
        schedulabe_time = std::max(schedulabe_time, cores_ocuppied_time[core_num - 1]);
        nodes_finish_time[i] = schedulabe_time + nodes_[i].time;
        for (int16_t j = 0; j < nodes_[i].core_num; j++)
        {
            cores_ocuppied_time[j] = nodes_finish_time[i];
        }
    }
    std::sort(cores_ocuppied_time.begin(), cores_ocuppied_time.end());
    return cores_ocuppied_time.back();
}
}
