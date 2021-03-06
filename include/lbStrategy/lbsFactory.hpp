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
#include <memory>
#include "lbStrategy.hpp"
#include "lbStrategy/discreteProbability.hpp"
#include "lbStrategy/revertDiscreteProbability.hpp"
#include "lbStrategy/roundRobbin.hpp"
namespace lbStrategy
{
template <typename LB_OBJ>
class lbsFactory
{
public:
    lbsFactory(){};
    virtual ~lbsFactory(){};

    static std::shared_ptr<lbStrategy<LB_OBJ>> create(std::string name)
    {
        std::shared_ptr<lbStrategy<LB_OBJ>> ret = nullptr;
        if (!name.compare("DPD"))
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "DPD load balance strategy!");
            }
            ret = std::dynamic_pointer_cast<lbStrategy<LB_OBJ>>(std::make_shared<discreteProbability<LB_OBJ>>());
        }
        else if (!name.compare("RDPD"))
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "RDPD load balance strategy!");
            }
            ret = std::dynamic_pointer_cast<lbStrategy<LB_OBJ>>(std::make_shared<revertDiscreteProbability<LB_OBJ>>());
        }
        else if (!name.compare("RR"))
        {
            if (CHECK_LOG_LEVEL(debug))
            {
                __LOG(debug, "round roubin load balance strategy!");
            }
            ret = std::dynamic_pointer_cast<lbStrategy<LB_OBJ>>(std::make_shared<roundRobbin<LB_OBJ>>());
        }
        else
        {
            if (CHECK_LOG_LEVEL(warn))
            {
                __LOG(warn, "not support type! type is : " << name);
            }
        }
        return ret;
    }
};
} // namespace lbStrategy