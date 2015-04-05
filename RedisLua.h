/*
 * RedisLua.h
 *
 *  Created on: Jan 15, 2015
 *      Author: yaoxing
 */

#ifndef REDISLUA_H_
#define REDISLUA_H_

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include "RedisCon.h"

class RedisLua: boost::noncopyable
{
public:
	explicit RedisLua(const std::string& filename);
	~RedisLua();

	RedisLua() = delete;

	//生成命令字符串
	std::vector<std::string> commands(const std::vector<std::string>& keys,
			const std::vector<std::string>& args, bool sha = true);

	//执行lua脚本
	RedisReply exec(RedisCon* con,
			const std::vector<std::string>& keys = std::vector<std::string>(),
			const std::vector<std::string>& args = std::vector<std::string>());


private:


	static const uint32_t reserve_len;
	static const std::string eval;
	static const std::string evalsha;

	std::string sha1_;
	std::string file_name_;
};

#endif /* REDISLUA_H_ */
