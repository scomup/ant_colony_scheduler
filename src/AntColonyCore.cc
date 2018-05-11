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
    cont_.resize(nodes_.size() , 0);
    init_ant_table();
    update_roulettes();
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
}

void AntColonySchedulerCore::run()
{

    for (uint32_t loop = 0; loop < config_ptr_->max_loop; loop++)
    {
        trails_.clear();
        for (size_t i = 0; i < config_ptr_->max_chromosome_num; i++)
        {
            auto ant = std::make_shared<AntTrail>(nodes_);
            ant_explore(ant);
            trails_.push_back(ant);
        }
        update_pheromone();
        update_roulettes();
        std::fill (cont_.begin(),cont_.end(),0);
        std::cout<<"loop"<<loop<<std::endl;
    }
}

void AntColonySchedulerCore::update_roulettes()
{
    if (roulettes_.size() == 0)
    {
        for (size_t i = 0; i<ant_routing_table_.size()-1;i++)
        {
            roulettes_.emplace_back(Roulette(ant_routing_table_[i], .1));
        }
    }
    else
    {
        for (size_t i = 0; i<ant_routing_table_.size()-1;i++)
        {
            roulettes_[i].update_roulette(ant_routing_table_[i]);
        }
    }
}

void AntColonySchedulerCore::ant_explore(std::shared_ptr<AntTrail> ant)
{
    while ((uint32_t)ant->current_task_id_ != nodes_.size() - 1)
    {
        int16_t next_id;
        do{
            next_id = roulettes_[ant->current_task_id_].spin_roulette();
        }
        while(!ant->is_schedulable(next_id));
        if(ant->current_task_id_ == 0)
            cont_[next_id]++;
        ant->go_next_state(nodes_, next_id);
    }
    int16_t scheduling_length = evaluate(ant->get_task_sequence());
    ant->set_scheduling_length(scheduling_length);
}

void AntColonySchedulerCore::update_pheromone()
{
    int16_t best = 10000;
    for (std::shared_ptr<AntTrail> trail : trails_)
    {   
        int16_t l = trail->get_scheduling_length();
        best = std::min(l,best);
    }

    
    for (std::shared_ptr<AntTrail> trail : trails_)
    {   
        int16_t l = trail->get_scheduling_length();
        double pheromone = exp(-config_ptr_->alpha*(l-best))*10;


        for (size_t i = 1; i < trail->get_task_sequence().size(); i++)
        {
            int16_t id_a = trail->get_task_sequence()[i - 1];
            int16_t id_b = trail->get_task_sequence()[i];
            
            ant_routing_table_[id_a][id_b] += pheromone;
        }
    }
    bk_ant_routing_table_.push_back(ant_routing_table_);
    if (bk_ant_routing_table_.size() > 10)
    {
        for (size_t i = 0; i < ant_routing_table_.size(); ++i)
        {
            for (size_t j = 0; j < ant_routing_table_[i].size(); ++j)
            {
                ant_routing_table_[i][j] -= bk_ant_routing_table_.front()[i][j];
            }
        }
        bk_ant_routing_table_.pop_front();
    }

    std::cout << best << "\n";
    /*
    for (size_t i = 0; i < ant_routing_table_.size(); ++i)
    {
        std::cout <<std::setw(2)<<i;
        for (size_t j = 0; j < ant_routing_table_[i].size(); ++j)
        {
            std::cout <<std::fixed << std::setw(3) << std::setprecision( 1 ) << std::setfill('0') << ant_routing_table_[i][j] << " ";
        }
        std::cout << "\n";
    }
    getchar();
    */

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
