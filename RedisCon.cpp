/*
 * RedisConnect.cpp
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#include <hiredis/hiredis.h>
#include "RedisException.h"
#include "RedisReply.h"
#include "RedisCon.h"


RedisCon::RedisCon(uint32_t epoch, const std::string& host, int32_t port)
: connect_(redisConnect(host.c_str(), port), redisFree)
, epoch_(epoch)
{
	if (connect_ == nullptr) {
		throw redis_connect_except();
	} else if (!isValid()) {
		throw redis_connect_except(connect_->errstr);
	}
}

RedisCon::~RedisCon()
{

}

RedisReply RedisCon::command(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	redisReply *r = reinterpret_cast<redisReply*>(redisvCommand(connect_.get(), format, ap));
	va_end(ap);

	if (!isValid()) {
		throw redis_transport_except(connect_->errstr);
	}

	return std::move(RedisReply(r));
}

RedisReply RedisCon::command(const std::string& cmd)
{
	redisReply *r = reinterpret_cast<redisReply*>(redisCommand(connect_.get(), cmd.c_str()));

	if (!isValid()) {
		throw redis_transport_except(connect_->errstr);
	}

	return std::move(RedisReply(r));
}

void RedisCon::append(const char* format, ...)
{
	va_list ap;
	va_start(ap, format);
	int ret = redisvAppendCommand(connect_.get(), format, ap);
	va_end(ap);

	if (ret != REDIS_OK) {
		throw redis_transport_except(connect_->errstr);
	}
}

void RedisCon::append(const std::string& cmd)
{
	int ret = redisAppendCommand(connect_.get(), cmd.c_str());

	if (ret != REDIS_OK) {
		throw redis_transport_except(connect_->errstr);
	}
}

std::vector<RedisReply> RedisCon::getReply(uint32_t cnt)
{
    std::vector<RedisReply> replys;
    replys.reserve(cnt);
    for (size_t i = 0; i < cnt; ++i) {
    	redisReply *r;
    	int err = redisGetReply(connect_.get(), reinterpret_cast<void**>(&r));
    	RedisReply reply(r);
    	if (err != REDIS_OK) {
        	throw redis_transport_except(connect_->errstr);
        }
    	replys.emplace_back(std::move(reply));
    }

    return std::move(replys);
}

std::vector<RedisReply> RedisCon::pipelining(const std::vector<std::string> cmds)
{
    //send all commands
    for (auto& i: cmds) {
    	redisAppendCommand(connect_.get(), i.c_str());
    }

    return std::move(getReply(cmds.size()));
}

RedisReply RedisCon::pipeliningSingle(const std::vector<std::string> command)
{
    std::vector<const char*> argv;
    argv.reserve(command.size());
    std::vector<size_t> argvlen;
    argvlen.reserve(command.size());

    for (std::vector<std::string>::const_iterator it = command.begin(); it != command.end(); ++it) {
        argv.push_back(it->c_str());
        argvlen.push_back(it->size());
    }

    int err = redisAppendCommandArgv(connect_.get(), static_cast<int>(command.size()), argv.data(), argvlen.data());
	if (err != REDIS_OK) {
    	throw redis_transport_except(connect_->errstr);
    }

	redisReply *r;
	err = redisGetReply(connect_.get(), reinterpret_cast<void**>(&r));
	RedisReply reply(r);
	if (err != REDIS_OK) {
    	throw redis_transport_except(connect_->errstr);
    }
    return std::move(reply);
}

