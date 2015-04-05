/*
 * RedisConnect.h
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#ifndef REDISCONNECT_H_
#define REDISCONNECT_H_

#include <string>
#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>
#include "RedisReply.h"

class RedisCon: boost::noncopyable
{
public:
	RedisCon(uint32_t epoch, const std::string& host, int32_t port = 6379);
	~RedisCon();

	RedisCon() = delete;

	//ִ�е�������
	RedisReply command(const char *format, ...);
	RedisReply command(const std::string& cmd);

	//ʹ��pipeliningִ�ж�������
	void append(const char* format, ...);
	void append(const std::string& cmd);
	std::vector<RedisReply> getReply(uint32_t cnt);
	std::vector<RedisReply> pipelining(const std::vector<std::string> cmds);

	//ʹ��pipeliningִ��һ������
	RedisReply pipeliningSingle(const std::vector<std::string> command);

	inline bool isValid()  const {return connect_->err == REDIS_OK;}

	uint32_t epoch() const {return epoch_;}

private:
	std::unique_ptr<redisContext, void(*)(redisContext*)> connect_;
	uint32_t epoch_;
};

#endif /* REDISCONNECT_H_ */
