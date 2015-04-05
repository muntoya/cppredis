/*
 * RedisError.h
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#ifndef REDISERROR_H_
#define REDISERROR_H_

#include <string>
#include <map>
#include <mutex>
#include <boost/noncopyable.hpp>

#define REDIS_ERROR RedisError::getInstance()

class RedisError: boost::noncopyable
{
	RedisError();
	virtual ~RedisError();
public:

	const std::string& getErrorString(int32_t err);

public:
	static RedisError* getInstance();

private:
	static std::once_flag s_instance_flag;
	static RedisError* s_redis_error;

private:
	using ErrorMap = std::map<int32_t, std::string>;
	ErrorMap error_map_;

	std::string no_error_;
};

#endif /* REDISERROR_H_ */
