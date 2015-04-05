/*
 * RedisError.cpp
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#include <cassert>
#include <hiredis/hiredis.h>
#include "RedisError.h"

std::once_flag RedisError::s_instance_flag;
RedisError* RedisError::s_redis_error = nullptr;

RedisError::RedisError()
{
	error_map_[REDIS_ERR] = "Redis error";
	error_map_[REDIS_OK] = "Redis ok";
	error_map_[REDIS_ERR_IO] = "Redis error: Error in read or write";
	error_map_[REDIS_ERR_EOF] = "Redis error: End of file";
	error_map_[REDIS_ERR_PROTOCOL] = "Redis error: Protocol error";
	error_map_[REDIS_ERR_OOM] = "Redis error: Out of memory";
	error_map_[REDIS_ERR_OTHER] = "Redis error: Everything else...";

	no_error_ = "can't find matching error!";
}

RedisError::~RedisError()
{

}
RedisError* RedisError::getInstance()
{
	std::call_once(s_instance_flag, []() {assert(s_redis_error == nullptr); s_redis_error = new RedisError; });

	return s_redis_error;
}

const std::string& RedisError::getErrorString(int32_t err)
{
	auto ifind = error_map_.find(err);
	if (ifind == error_map_.end()) {
		return no_error_;
	} else {
		return ifind->second;
	}
}

