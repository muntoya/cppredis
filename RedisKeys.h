/*
 * RedisKeys.h
 *
 *  Created on: Jan 16, 2015
 *      Author: yaoxing
 */

#ifndef REDISKEYS_H_
#define REDISKEYS_H_

#include <vector>
#include <string>
#include <mutex>

#define REDIS_KEYS (RedisKeys::getInstance())

class RedisKeys
{
public:
	RedisKeys();
	~RedisKeys();

	static RedisKeys* getInstance();

	enum KeyType: uint32_t
	{
		user = 0,

		tempfamily,			//临时群
		permfamily,			//永久群


		keytype_end,
	};

	enum Field: uint32_t
	{
		contact = 0,
		online,

		srvip,

		fieldtype_end,
	};

	enum HashType: uint32_t
	{
		user_info = 0,

		hashtype_end,
	};

	//生成key
	std::string key(KeyType key_type, uint32_t id, Field field);
	std::string key(KeyType key_type, Field field);

	//生成hash类型的key
	std::string hashKey(HashType hash_type, uint32_t id);
	std::string hashKey(HashType hash_type, const std::string& id);

	//从key中解析出id
	uint32_t key2id(const std::string& key, KeyType key_type);
	uint32_t key2id(const std::string& key, KeyType key_type, Field field);
	uint32_t hashkey2id(const std::string& key, HashType hash_type);

private:
	void initKeys();

private:
	static std::once_flag s_instance_flag;
	static RedisKeys* s_redis_keys;

	const static std::string separator;

	std::string key_type_map_[keytype_end];
	std::string field_type_map_[fieldtype_end];

	std::string hash_type_map_[hashtype_end];
};

#endif /* REDISKEYS_H_ */
