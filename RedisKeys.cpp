/*
 * RedisKeys.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: yaoxing
 */

#include <cassert>
#include <boost/regex.hpp>
#include "RedisKeys.h"

using std::string;

std::once_flag RedisKeys::s_instance_flag;
RedisKeys* RedisKeys::s_redis_keys = 0;
const std::string RedisKeys::separator = ":";

RedisKeys::RedisKeys()
{
	initKeys();
}

RedisKeys::~RedisKeys()
{
	// TODO Auto-generated destructor stub
}

RedisKeys* RedisKeys::getInstance()
{
	std::call_once(s_instance_flag, []() {assert(s_redis_keys == nullptr); s_redis_keys = new RedisKeys; });
	return s_redis_keys;
}

void RedisKeys::initKeys()
{
	key_type_map_[user] = "user";
	key_type_map_[permfamily] = "permfamily";
	key_type_map_[tempfamily] = "tempfamily";

	field_type_map_[contact] = "contact";
	field_type_map_[online] = "online";
	field_type_map_[srvip] = "srvip";

	hash_type_map_[user_info] = "userinfo";
}

std::string RedisKeys::key(KeyType key_type, uint32_t id, Field field)
{
	string key = key_type_map_[key_type] + separator + std::to_string(id) + separator + field_type_map_[field];
	return std::move(key);
}

std::string RedisKeys::key(KeyType key_type, Field field)
{
	string key = key_type_map_[key_type] + separator + field_type_map_[field];
	return std::move(key);
}

string RedisKeys::hashKey(HashType hash_type, uint32_t id)
{
	string key = hash_type_map_[hash_type] + separator +std::to_string(id);
	return std::move(key);
}

string RedisKeys::hashKey(HashType hash_type, const string& id)
{
	string key = hash_type_map_[hash_type] + separator + id;
	return std::move(key);
}

uint32_t RedisKeys::key2id(const std::string& key, KeyType key_type)
{
	boost::regex re(key_type_map_[key_type] + separator + R"((\d+))");
	boost::smatch m;
	boost::regex_match(key, m, re, boost::regex_constants::match_not_bow);
	if (m.size() > 1) {
		return std::stoul(m[1]);
	} else {
		return 0;
	}
}

uint32_t RedisKeys::key2id(const std::string& key, KeyType key_type, Field field)
{
	boost::regex re(key_type_map_[key_type] + separator + R"((\d+))" + separator + field_type_map_[field]);
	boost::smatch m;
	boost::regex_match(key, m, re, boost::regex_constants::match_not_bow);
	if (m.size() > 1) {
		return std::stoul(m[1]);
	} else {
		return 0;
	}
}

uint32_t RedisKeys::hashkey2id(const std::string& key, HashType hash_type)
{
	boost::regex re(hash_type_map_[hash_type] + separator + R"((\d+))");
	boost::smatch m;
	boost::regex_match(key, m, re, boost::regex_constants::match_not_bow);
	if (m.size() > 1) {
		return std::stoul(m[1]);
	} else {
		return 0;
	}
}

