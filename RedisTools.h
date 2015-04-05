/*
 * RedisTools.h
 *
 *  Created on: Jan 21, 2015
 *      Author: yaoxing
 */

#ifndef REDISTOOLS_H_
#define REDISTOOLS_H_

#define REDIS_NIL_VALUE R"()"

class RedisTools
{
public:
	//生成sha1
	static std::string to_sha1(const std::string& lua);

	//加载脚本的全部代码
	static std::string load_script(const std::string& file);

private:
	static const std::string lua_path;
};

#endif /* REDISTOOLS_H_ */
