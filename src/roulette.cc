#include "roulette.h"

#include <algorithm>
#include <assert.h>
#include <omp.h>

namespace Scheduler
{
Roulette::Roulette(std::vector<double> &scores, double alpha)
{
    alpha_ = alpha;
    roulette_.resize(scores.size());
    prob_.resize(scores.size());
    update_roulette(scores);
}
void Roulette::update_roulette(std::vector<double> &scores)
{
    
    double best_score = 0;
    for (double score : scores)
    {
        best_score = std::max(best_score, score);
    }
    //if(best_score==0)
    //    return;

    double sum = 0;
    int i = 0;
    for (double score : scores)
    {
        if(std::isnan(score)){
            roulette_[i] = 0;
            i++;
            continue;
        }
        if(best_score == 0 || score<1.)
            score = 1.;
        //uint32_t diff = best_score - score;
        //assert(diff >= 0);
        //roulette_[i] = exp(-(alpha_ * diff));
        prob_[i] = score;
        sum += prob_[i];
        i++;
    }
    assert(sum != 0);
/*
    for (size_t i = 1; i < roulette_.size(); i++)
    {
        prob_[i] = prob_[i] / sum;
    }*/

    roulette_[0] = prob_[0]/sum;
    for (size_t i = 1; i < roulette_.size(); i++)
    {
        roulette_[i] = roulette_[i - 1] + prob_[i]/sum;
    }
    roulette_[roulette_.size()-1] = 1;
}

int32_t Roulette::spin_roulette()
{
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    int32_t index = (roulette_.size()-1)/2;
    int32_t step = index;
    while (true)
    {
        if (step > 1)
            step /= 2;

        if (index == 0 || (r >= roulette_[index - 1] && r < roulette_[index]))
        {
            break;
        }
        if (r >= roulette_[index - 1])
        {
            index += step;
        }
        else
        {
            index -= step;
        }
    }
    return index;
}

}
