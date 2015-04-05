/*
 * RedisReply.h
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#ifndef REDISREPLY_H_
#define REDISREPLY_H_

#include <vector>
#include <memory>
#include <functional>
#include <boost/noncopyable.hpp>
#include "hiredis/hiredis.h"

class RedisReply: boost::noncopyable
{
public:
	using ReplyDeleter = std::function<void(void*)>;
	RedisReply(redisReply* reply,
			const ReplyDeleter& deleter = freeReplyObject);
	~RedisReply();

	RedisReply() {};

	RedisReply(RedisReply&& a);
	RedisReply& operator=(RedisReply&& a);

	//指针是否有效
	bool isValid() const {return redis_reply_ != nullptr;}

	//reply的结果是否有错误
	bool noError() const {return isValid() &&
			redis_reply_->type != REDIS_REPLY_ERROR &&
			redis_reply_->type != REDIS_REPLY_NIL;}

	bool readOnly();

	int32_t type() const {return redis_reply_->type;}
	int64_t integer() const {return redis_reply_->integer;}
	int32_t len() const {return redis_reply_->len;}
	const char * str() const {return redis_reply_->str;}
	const std::vector<RedisReply>& elements() const {return reply_element_;}

private:
	//解析全部列表
	void getElements();

private:
	std::unique_ptr<redisReply, ReplyDeleter> redis_reply_;

	std::vector<RedisReply> reply_element_;
};

#endif /* REDISREPLY_H_ */
