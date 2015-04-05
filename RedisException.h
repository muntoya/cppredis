/*
 * Exception.h
 *
 *  Created on: Jan 6, 2015
 *      Author: yaoxing
 */

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>
#include <string>
#include <hiredis/hiredis.h>
#include "RedisError.h"

class redis_exception: public std::exception
{
public:
	redis_exception(int32_t err = REDIS_ERR)
	{
		errstr = REDIS_ERROR->getErrorString(err);
	}

	redis_exception(const std::string& err)
	{
		errstr = err;
	}

	virtual ~redis_exception() {}

	virtual const char* what() const _GLIBCXX_USE_NOEXCEPT override
	{
		return errstr.c_str();
	}

private:
	std::string errstr;
};

#define REDIS_UTIL_EXCEPTION(name) class name: public redis_exception\
{\
public:\
name(int32_t err = REDIS_ERR)\
: redis_exception(err)\
{}\
name(const std::string& err)\
: redis_exception(err)\
{}\
virtual ~name() {}\
};

REDIS_UTIL_EXCEPTION(redis_connect_except)
REDIS_UTIL_EXCEPTION(sentinel_connect_except)
REDIS_UTIL_EXCEPTION(redis_transport_except)
REDIS_UTIL_EXCEPTION(redis_master_except)
REDIS_UTIL_EXCEPTION(redis_slave_except)
REDIS_UTIL_EXCEPTION(redis_lua_except)


#endif /* EXCEPTION_H_ */
