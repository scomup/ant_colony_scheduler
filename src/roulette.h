#ifndef ROULETTE_H
#define ROULETTE_H

#include <vector>

#include "node.h"
#include "AntTrail.h"

namespace Scheduler
{

class Roulette
{
  public:
    Roulette(std::vector<double> &scores, double alpha);
    int32_t spin_roulette();
    void update_roulette(std::vector<double> &scores);


  private:
    std::vector<double> roulette_;
    double alpha_;

};

}

#endif