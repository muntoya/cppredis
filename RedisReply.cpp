/*
 * RedisReply.cpp
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#include <hiredis/hiredis.h>
#include <cassert>
#include <boost/algorithm/string.hpp>
#include "RedisException.h"
#include "RedisReply.h"

RedisReply::RedisReply(redisReply* r, const std::function<void(void*)>& deleter)
: redis_reply_(r, deleter)
{
	getElements();
}

RedisReply::RedisReply(RedisReply&& a)
: redis_reply_(std::move(a.redis_reply_))
, reply_element_(std::move(a.reply_element_))
{

}

RedisReply& RedisReply::operator=(RedisReply&& a)
{
	if (this == &a) {
		return *this;
	}

	redis_reply_ = std::move(a.redis_reply_);
	reply_element_ = std::move(a.reply_element_);
	return *this;
}

RedisReply::~RedisReply()
{
}

void RedisReply::getElements()
{
	reply_element_.clear();
	if (redis_reply_ != nullptr && redis_reply_->type == REDIS_REPLY_ARRAY) {
		for (size_t i = 0;i < redis_reply_->elements; ++i) {
			reply_element_.emplace_back(RedisReply(redis_reply_->element[i], [] (void*) {}));
		}
	}
}

bool RedisReply::readOnly()
{
	return isValid() &&
			redis_reply_->type == REDIS_REPLY_ERROR &&
			boost::algorithm::starts_with(str(), "READONLY");
}

