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

#include "loop/loop.hpp"
#include "hiredis/async.h"
#include "buildRedisCommand/buildRedisCommand.hpp"
#include "logger/boost_logger.hpp"
#include "task/task.hpp"
#include "util/nonCopyable.hpp"
#include "util/defs.hpp"

#include <string>
#include <memory>

class redisAsyncClient : public nonCopyable
{
public:
    bool init()
    {

        _loop_sptr.reset(new loop::loop(), [](loop::loop *innerLoop) {
            if (innerLoop)
            {
                innerLoop->stop(true);
                delete innerLoop;
            }
        });
        if (!_loop_sptr)
        {
            __LOG(error, "init loop fail");
            return false;
        }

        // start task
        _task_sptr = std::make_shared<task::taskImp>(_loop_sptr);

        if (!_task_sptr)
        {
            __LOG(error, "create task fail");
            return false;
        }
        _task_sptr->setGeneticGene(getThis());
        _task_sptr->init();

        medis::taskSaver<void *, std::shared_ptr<task::taskImp>>::instance()->save(getThis(), _task_sptr);
        return _loop_sptr->start(true);
    }
    template <typename COMMAND_KEY, typename COMMAND_VALUE>
    bool put(COMMAND_KEY&& key, COMMAND_VALUE&& value, void *usr_data, redisCallbackFn *fn)
    {
        if (!getConnStatus())
        {
            return false;
        }
        std::string command2send = std::move(buildRedisCommand::buildRedisCommand<COMMAND_KEY, COMMAND_VALUE>::get_format_command(REDIS_COMMAND_TYPE::TASK_REDIS_PUT, std::forward<COMMAND_KEY>(key), std::forward<COMMAND_VALUE>(value)));
        __LOG(debug, "get command :\n"
                         << command2send);
        if (command2send.empty())
        {
            __LOG(warn, "did not get redis command, please check the key type and value type");
            return false;
        }
        return sendFormatRawCommand(std::move(command2send), usr_data, fn);
    }
    template <typename COMMAND_KEY, typename COMMAND_VALUE>
    bool get(COMMAND_KEY &&key, COMMAND_VALUE &&value, void *usr_data, redisCallbackFn *fn)
    {
        if (!getConnStatus())
        {
            return false;
        }
        std::string command2send = std::move(buildRedisCommand::buildRedisCommand<COMMAND_KEY, COMMAND_VALUE>::get_format_command(REDIS_COMMAND_TYPE::TASK_REDIS_GET, std::forward<COMMAND_KEY>(key), std::forward<COMMAND_VALUE>(value)));
        __LOG(debug, "get command :\n"
                         << command2send);
        if (command2send.empty())
        {
            __LOG(warn, "did not get redis command, please check the key type and value type");
            return false;
        }
        return sendFormatRawCommand(std::move(command2send), usr_data, fn);
    }

    template <typename COMMAND_KEY, typename COMMAND_VALUE>
    bool del(COMMAND_KEY&& key, COMMAND_VALUE&& value, void *usr_data, redisCallbackFn *fn)
    {
        if (!getConnStatus())
        {
            return false;
        }
        std::string command2send = std::move(buildRedisCommand::buildRedisCommand<COMMAND_KEY, COMMAND_VALUE>::get_format_command(REDIS_COMMAND_TYPE::TASK_REDIS_DEL, std::forward<COMMAND_KEY>(key), std::forward<COMMAND_VALUE>(value)));
        __LOG(debug, "del command :\n"
                         << command2send);
        if (command2send.empty())
        {
            __LOG(warn, "did not get redis command, please check the key type and value type");
            return false;
        }
        return sendFormatRawCommand(std::move(command2send), usr_data, fn);
    }

    bool sendFormatRawCommand(std::string &&command, void *usr_data, redisCallbackFn *fn)
    {
        std::shared_ptr<task::TASK_REDIS_FORMAT_RAW_MSG_BODY> msg = std::make_shared<task::TASK_REDIS_FORMAT_RAW_MSG_BODY>();
        msg->fn = fn;
        msg->body = command;
        msg->usr_data = usr_data;
        return _task_sptr->sendMsg(task::taskMsgType::TASK_REDIS_FORMAT_RAW, msg);
    }
    bool sendRawCommand(std::string &&command, void *usr_data, redisCallbackFn *fn)
    {
        std::shared_ptr<task::TASK_REDIS_RAW_MSG_BODY> msg = std::make_shared<task::TASK_REDIS_RAW_MSG_BODY>();
        msg->fn = fn;
        msg->body = command;
        msg->usr_data = usr_data;
        return _task_sptr->sendMsg(task::taskMsgType::TASK_REDIS_RAW, msg);
    }
    void *getThis()
    {
        return (void *)this;
    }
    bool getConnStatus()
    {
        return _task_sptr->getConnStatus();
    }
    std::shared_ptr<loop::loop> _loop_sptr;
    std::shared_ptr<task::taskImp> _task_sptr;
};
