#ifndef GENETICS_CHEDULER_CORE_H
#define GENETICS_CHEDULER_CORE_H

#include <algorithm>
#include <limits>
#include <chrono>
#include <memory>
#include "boost/multi_array.hpp"

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
  void ant_explore(std::shared_ptr<AntTrail> ant);

  //uint32_t getBest() { return home_->update_score(config_ptr_->max_loop); };

private:
  int16_t evaluate(std::vector<int16_t> &task_sequence);
  void update_pheromone();
  void update_roulettes();
  
  void init_ant_table();
  void add_flag(int16_t i, int16_t j);


private:
  std::vector<Node> nodes_;
  Scheduler::Config *config_ptr_;
  std::shared_ptr<AntTrail> home_;
  std::vector<std::shared_ptr<AntTrail>> trails_;
  std::vector<std::vector<double>> ant_routing_table_;
  //boost::multi_array<double, 2> ant_routing_table_;
  std::vector<Roulette> roulettes_;

};

}

#endif
