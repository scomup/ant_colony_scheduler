#ifndef ANT_PATH_H
#define ANT_PATH_H

#include <algorithm>
#include <limits>
#include <chrono>
#include <memory>
#include <unordered_set>

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
    AntTrail(int16_t current_task_id) : current_task_id_(current_task_id)
    {
        task_sequence_.push_back(current_task_id);
        used_tasks_.insert(current_task_id);
        
        /*
        prevPath_ = prevPath;
        if (prevPath != nullptr)
            std::copy(prevPath->task_sequence_.begin(),
                      prevPath->task_sequence_.end(),
                      back_inserter(task_sequence_));
        task_sequence_.push_back(taskID);
        scores_.push_back(max_score);
        */
    };



    int16_t current_task_id_;
    //std::shared_ptr<AntTrail> prevPath_;
    //std::vector<std::shared_ptr<AntTrail>> nextPath_;
    //std::vector<uint32_t> scores_;
    //uint32_t score;
    std::vector<int16_t> task_sequence_;
    std::unordered_set<int16_t> used_tasks_;
    std::unordered_set<int16_t> schedulable_tasks_;

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
