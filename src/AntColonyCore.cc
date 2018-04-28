#include <iostream>
#include <assert.h>
#include <fstream>

#include "AntColonyCore.h"

namespace Scheduler
{

AntColonySchedulerCore::AntColonySchedulerCore(std::vector<Node> nodes, Scheduler::Config *config_ptr)
    : nodes_(nodes), config_ptr_(config_ptr){

                                              };

void AntColonySchedulerCore::run()
{
    home_ = std::make_shared<AntPath>(0);

    for (uint32_t loop = 0; loop < config_ptr_->max_loop; loop++)
    {
        for (size_t i = 0; i < config_ptr_->max_chromosome_num; i++)
        {
            auto path = explore();
            paths_.push_back(path);
        }
        update_pheromone();
        std::cout<<home_->update_score()<<std::endl;
    }
}

std::shared_ptr<AntPath> AntColonySchedulerCore::explore()
{
    auto path = home_;
    while ((uint32_t)path->taskID_ != nodes_.size() - 1)
    {
        if (path->nextPath_.size() == 0)
        {
            path->create_next_path(nodes_, config_ptr_->alpha);
        }
        int idx = path->roulette_->spin_roulette();
        path = path->nextPath_[idx];
    }
    return path;

}

void AntColonySchedulerCore::update_pheromone()
{
    for (std::shared_ptr<AntPath> path : paths_)
    {
        uint32_t scheduling_length = evaluate(path->task_sequence);
        while (true)
        {
            path->scores_[0] = std::min(path->scores_[0], scheduling_length);
            if(path->prevPath_ == nullptr)
                break;
            path = path->prevPath_;
        }
    }

    for (std::shared_ptr<AntPath> path : paths_)
    {
        while (path->scores_[0] != max_score)
        {
            path->scores_.insert(path->scores_.begin(),max_score);
            if(path->nextPath_.size() != 0)
                path->roulette_->update_roulette(path->nextPath_);
            if(path->prevPath_ == nullptr)
                break;
            path = path->prevPath_; 
        }
    }
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
        for (size_t j = 0; j < nodes_[i].sub_nodes.size(); j++)
        {
            int16_t sub_node_finished_time = nodes_finish_time[nodes_[i].sub_nodes[j]];
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
