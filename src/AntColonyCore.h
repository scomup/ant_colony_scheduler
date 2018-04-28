#ifndef GENETICS_CHEDULER_CORE_H
#define GENETICS_CHEDULER_CORE_H

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

constexpr int16_t inf = -1;


class AntColonySchedulerCore
{
public:
  AntColonySchedulerCore(std::vector<Node> nodes, Scheduler::Config *config_ptr);
  void run();
  std::shared_ptr<AntPath> explore();
  uint32_t getBest() { return home_->update_score(config_ptr_->max_loop); };

private:
  int16_t evaluate(std::vector<int16_t> &task_sequence);
  void update_pheromone();


private:
  std::vector<Node> nodes_;
  Scheduler::Config *config_ptr_;
  std::shared_ptr<AntPath> home_;
  std::vector<std::shared_ptr<AntPath>> paths_;
};

}

#endif
