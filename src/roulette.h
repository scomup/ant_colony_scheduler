#ifndef ROULETTE_H
#define ROULETTE_H

#include <vector>

#include "node.h"
#include "AntPath.h"

namespace Scheduler
{

class Roulette
{
  public:
    Roulette(std::vector<std::shared_ptr<AntPath>> &path, double alpha);
    int32_t spin_roulette();
    void update_roulette(std::vector<std::shared_ptr<AntPath>> &paths);


  private:
    std::vector<double> roulette_;
    double alpha_;

};

}

#endif