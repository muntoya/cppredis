/*
 * RedisLua.cpp
 *
 *  Created on: Jan 15, 2015
 *      Author: yaoxing
 */

#include <cassert>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "RedisException.h"
#include "RedisTools.h"
#include "RedisLua.h"

const uint32_t RedisLua::reserve_len = 128;
const std::string RedisLua::eval = "EVAL";
const std::string RedisLua::evalsha = "EVALSHA";

RedisLua::RedisLua(const std::string& filename)
: file_name_(filename)
{
	std::string str = RedisTools::load_script(filename);
	sha1_ = RedisTools::to_sha1(str);
}

RedisLua::~RedisLua()
{

}

std::vector<std::string> RedisLua::commands(const std::vector<std::string>& keys,
		const std::vector<std::string>& args, bool sha)
{
	std::vector<std::string> cmds;
	if (sha) {
		cmds.push_back(evalsha);
		cmds.push_back(sha1_);
	} else {
		cmds.push_back(eval);
		cmds.push_back(RedisTools::load_script(file_name_));
	}

	cmds.push_back(std::to_string(keys.size()));

	for (const auto& i: keys) {
		cmds.push_back(i);
	}
	for (const auto& i: args) {
		cmds.push_back(i);
	}
	return std::move(cmds);
}

RedisReply RedisLua::exec(RedisCon* con,
		const std::vector<std::string>& keys,
		const std::vector<std::string>& args)
{
	std::vector<std::string> command = commands(keys, args, true);
	
	RedisReply reply = con->pipeliningSingle(command);
	assert(reply.isValid());
	if (!reply.noError() && reply.len() && boost::starts_with(reply.str(), "NOSCRIPT")) {
		command = commands(keys, args, false);
		reply = con->pipeliningSingle(command);
	}

	return std::move(reply);
}

