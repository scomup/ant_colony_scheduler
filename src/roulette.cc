#include "roulette.h"

#include <algorithm>
#include <assert.h>
#include <omp.h>

namespace Scheduler
{
Roulette::Roulette(std::vector<std::shared_ptr<AntPath>> &paths, double alpha)
{
    alpha_ = alpha;
    roulette_.resize(paths.size());
    update_roulette(paths);
}
void Roulette::update_roulette(std::vector<std::shared_ptr<AntPath>> &paths)
{
    uint32_t best_score = max_score;
    for (std::shared_ptr<AntPath> path : paths)
    {
        path->update_score();
        best_score = std::min(best_score, path->score);
    }

    double sum = 0;
    int i = 0;
    for (std::shared_ptr<AntPath> path : paths)
    {
        uint32_t current_score = std::min(path->score, best_score + 10);
        uint32_t diff = current_score - best_score;
        assert(diff >= 0);
        roulette_[i] = exp(-(alpha_ * diff));
        sum += roulette_[i];
        i++;
    }

    assert(sum != 0);
    roulette_[0] = roulette_[0] / sum;
    for (size_t i = 1; i < roulette_.size(); i++)
    {
        roulette_[i] = roulette_[i - 1] + roulette_[i] / sum;
    }
}

int32_t Roulette::spin_roulette()
{
    double r = static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
    int32_t index = roulette_.size();
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
