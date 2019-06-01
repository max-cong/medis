/*
 * Copyright (c) 2016-20019 Max Cong <savagecm@qq.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include "lbStrategy.hpp"
#include <random>
#include <mutex>
namespace lbStrategy
{
template <typename DIST_OBJ>
class discreteProbability : public lbStrategy<DIST_OBJ>
{
public:
    discreteProbability() : _gen(_rd())
    {
    }
    virtual bool init()
    {
        return true;
    }
    // note: there is no lock here
    // for performance, please check the return code during usage
    DBW_OPT<DIST_OBJ> getObj() override
    {

        if (this->_obj_vector.empty())
        {
            if (CHECK_LOG_LEVEL(warn))
            {
                __LOG(warn, "there is no object to get!");
            }
            return DBW_NONE_OPT;
        }
        DIST_OBJ obj;
        try
        {
            obj = std::get<0>(this->_obj_vector.at(_dist(_gen)));
        }
        catch (const std::out_of_range &oor)
        {
            if (CHECK_LOG_LEVEL(error))
            {
                __LOG(error, "Out of Range error: " << oor.what());
            }
            return DBW_NONE_OPT;
        }
        return obj;
    }

    medis::retStatus update() override
    {
        int vector_size = this->_obj_vector.size();
        if (!vector_size)
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "this->_obj_vector is empty!");
            }
            return medis::retStatus::NO_ENTRY;
        }
        std::vector<double> init_list;
        if (CHECK_LOG_LEVEL(debug))
        {
            __LOG(debug, "weight is :");
        }
        for (int i = 0; i < vector_size; i++)
        {
            init_list.push_back(std::get<1>(this->_obj_vector[i]));
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "--> " << std::get<1>(this->_obj_vector[i]));
            }
        }
        std::discrete_distribution<int> second_dist(init_list.begin(), init_list.end());
        auto _param = second_dist.param();
        _dist.param(_param);
        _dist.reset();

        int _max_index = this->_obj_vector.size();
        return ((_max_index > 0) ? medis::retStatus::SUCCESS : medis::retStatus::NO_ENTRY);
    }

    std::random_device _rd;
    std::mt19937 _gen;
    std::discrete_distribution<int> _dist;
};
} // namespace lbStrategy