#ifndef ANT_PATH_H
#define ANT_PATH_H

#include <algorithm>
#include <limits>
#include <chrono>
#include <memory>

#include "common/make_unique.h"
#include "common/common.h"
#include "config.h"
#include "node.h"
#include "roulette.h"

namespace Scheduler
{

class Roulette;

class AntPath : public std::enable_shared_from_this<AntPath>
{
  public:
    AntPath(int16_t taskID, std::shared_ptr<AntPath> prevPath = nullptr) : taskID_(taskID)
    {
        prevPath_ = prevPath;
        if (prevPath != nullptr)
            std::copy(prevPath->task_sequence.begin(),
                      prevPath->task_sequence.end(),
                      back_inserter(task_sequence));
        task_sequence.push_back(taskID);
        scores_.push_back(max_score);
    };

    int16_t taskID_;
    std::shared_ptr<AntPath> prevPath_;
    std::vector<std::shared_ptr<AntPath>> nextPath_;
    std::vector<uint32_t> scores_;
    uint32_t score;
    std::vector<int16_t> task_sequence;
    std::unique_ptr<Roulette> roulette_;

    void create_next_path(std::vector<Node> &nodes, double alpha)
    {
        for (uint32_t id = 1; id < nodes.size(); id++)
        {
            if (find(task_sequence, id))
                continue;
            bool is_candidate = true;
            for (int16_t parent_id : nodes[id].sub_nodes)
            {
                if (!find(task_sequence, parent_id))
                {
                    is_candidate = false;
                    break;
                }
            }
            if (is_candidate)
            {
                auto new_path = std::make_shared<AntPath>(id, shared_from_this());
                nextPath_.emplace_back(new_path);
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
