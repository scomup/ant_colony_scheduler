#ifndef ANT_PATH_H
#define ANT_PATH_H

#include <algorithm>
#include <limits>
#include <chrono>
#include <memory>
#include <unordered_set>
#include <fstream>
#include <iostream>
#include <iomanip>


#include "common/make_unique.h"
#include "common/common.h"
#include "config.h"
#include "node.h"
#include "roulette.h"

namespace Scheduler
{

class AntTrail : public std::enable_shared_from_this<AntTrail>
{
  public:
    AntTrail(std::vector<Node> &nodes) : current_task_id_(0)
    {
        task_sequence_.push_back(0);
        used_tasks_.insert(0);
        update_schedulable_tasks(nodes);
    };

    int16_t current_task_id_;
    std::vector<int16_t> task_sequence_;
    std::unordered_set<int16_t> used_tasks_;
    std::unordered_set<int16_t> schedulable_tasks_;
    int32_t scheduling_length_;

    int32_t get_scheduling_length(){return scheduling_length_;}
    std::vector<int16_t>& get_task_sequence(){return task_sequence_;}
    void set_scheduling_length(int32_t l){scheduling_length_ = l;}
    
    bool is_schedulable(int16_t id){
        return schedulable_tasks_.find(id) != schedulable_tasks_.end();
    }


    void update_schedulable_tasks(std::vector<Node> &nodes)
    {
        schedulable_tasks_.clear();
        for (uint32_t id = 1; id < nodes.size(); id++)
        {
            if (used_tasks_.find(id) != used_tasks_.end())
                continue;
            bool schedulable_falg = true;
            for (int16_t parent_id : nodes[id].parent_nodes)
            {
                if (used_tasks_.find(parent_id) == used_tasks_.end())
                {
                    schedulable_falg = false;
                    break;
                }
            }
            if (schedulable_falg)
                schedulable_tasks_.insert(id);
        }
    }

    void go_next_state(std::vector<Node> &nodes, int16_t next_id)
    {
        current_task_id_ = next_id;
        task_sequence_.push_back(next_id);
        used_tasks_.insert(next_id);
        update_schedulable_tasks(nodes);
    }

    //std::unique_ptr<Roulette> roulette_;

/*
    void create_next_trail(std::vector<Node> &nodes, double alpha)
    {
        for (uint32_t id = 1; id < nodes.size(); id++)
        {
            if (find(task_sequence_, id))
                continue;
            bool is_candidate = true;
            for (int16_t parent_id : nodes[id].parent_nodes)
            {
                if (!find(task_sequence_, parent_id))
                {
                    is_candidate = false;
                    break;
                }
            }
            if (is_candidate)
            {
                auto new_trail = std::make_shared<AntTrail>(id, shared_from_this());
                nextPath_.emplace_back(new_trail);
            }
        }
        roulette_ = std::move(common::make_unique<Roulette>(nextPath_, alpha));
    }

    uint32_t update_score(int max_l = 10)
    {
        int32_t l = scores_.size();
        l = std::min(l, max_l);
        score = *std::min_element(scores_.begin(), scores_.begin() + l);
        return score;
    }
    */

    friend std::ostream &operator<<(std::ostream &os, const AntTrail &ant)
    {
        for (int16_t id : ant.task_sequence_)
        {
            //os << id <<std::setfill('0')<<std::setw(2)<< " ";
            os << std::setw(3) << std::left << id ;
        }
        os <<" ("<<ant.scheduling_length_<<")"<<std::endl;
        return os;
    }

  private:
    bool find(std::vector<int16_t> &v, int16_t d)
    {
        std::vector<int16_t>::iterator cIter = std::find(v.begin(), v.end(), d);
        if (cIter != v.end())
            return true;
        else
            return false;
    }
};
}

#endif
