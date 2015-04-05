/*
 * IMRedis.cpp
 *
 *  Created on: Jan 15, 2015
 *      Author: yaoxing
 */

#include <cassert>
#include <iostream>
#include <chrono>
#include "RedisLua.h"
#include "RedisKeys.h"
#include "RedisException.h"
#include "RedisTools.h"
#include "IMRedis.h"

using std::string;
using std::vector;
using std::to_string;

std::once_flag IMRedis::s_instance_flag;
IMRedis* IMRedis::s_im_redis = nullptr;

const std::string IMRedis::s_lua_get_last_contacts = R"()";
const std::string IMRedis::s_lua_set_last_contacts = "set_last_contacts.lua";
const std::string IMRedis::s_lua_set_user_info = "set_user_info.lua";
const std::string IMRedis::s_lua_del_user_info = "del_user_info.lua";
const std::string IMRedis::s_lua_set_user_status = "set_user_status.lua";
const std::string IMRedis::s_lua_get_user_online = "get_user_online.lua";


IMRedis::IMRedis()
{
	RedisConPool::Attr attr;
	attr.slave_cnt = 10;
	attr.master_cnt = 10;
	attr.master_name = "mymaster";
	attr.reconnect_interval = 3;
	attr.sentinel_hosts["10.211.240.55"] = 26379;
	redis_con_pool_.reset(new RedisConPool(attr));
	redis_con_pool_->connect();

	initLua();
}

IMRedis::~IMRedis()
{

}

void IMRedis::initLua()
{
	luas_[get_last_contacts].reset(new RedisLua(s_lua_get_last_contacts));
	luas_[set_last_contacts].reset(new RedisLua(s_lua_set_last_contacts));
	luas_[set_user_info].reset(new RedisLua(s_lua_set_user_info));
	luas_[del_user_info].reset(new RedisLua(s_lua_del_user_info));
	luas_[set_user_status].reset(new RedisLua(s_lua_set_user_status));
	luas_[get_user_online].reset(new RedisLua(s_lua_get_user_online));
}

IMRedis* IMRedis::getInstance()
{
	std::call_once(s_instance_flag, []() {assert(s_im_redis == nullptr); s_im_redis = new IMRedis; });
	return s_im_redis;
}

std::string IMRedis::keyUserInfo(uint32_t id)
{
	return std::move(REDIS_KEYS->hashKey(RedisKeys::user_info, id));
}

std::string IMRedis::keyUserContacts(uint32_t id)
{
	return std::move(REDIS_KEYS->key(RedisKeys::user, id, RedisKeys::contact));
}

std::string IMRedis::keyUserOnline()
{
	return std::move(REDIS_KEYS->key(RedisKeys::user, RedisKeys::online));
}

uint32_t IMRedis::idFromUserinfo(const std::string& key)
{
	return REDIS_KEYS->hashkey2id(key, RedisKeys::user_info);
}

std::string IMRedis::keyTempFamilyIP(uint32_t id)
{
	return std::move(REDIS_KEYS->key(RedisKeys::tempfamily, id, RedisKeys::srvip));
}

std::string IMRedis::keyPermFamilyIP(uint32_t id)
{
	return std::move(REDIS_KEYS->key(RedisKeys::permfamily, id, RedisKeys::srvip));
}

int IMRedis::setUserInfo(const tagGSRV_ADDMODIFY_FULLUSERINFO_QUERY::UserInfo * pUserInfo,
		uint8 flag, uint8_t count)
{
	static const uint32_t s_field_count = 12;

	vector<string> keys, args;
	keys.reserve(count);
	args.reserve(count * s_field_count);

	for (uint32_t i = 0; i < count; ++i) {
		keys.push_back(keyUserInfo(pUserInfo[i].id));

		args.push_back(to_string(flag));
		args.push_back(to_string(pUserInfo[i].status));
		args.push_back(to_string(pUserInfo[i].rule));
		args.push_back(pUserInfo[i].username[0] != 0 ? pUserInfo[i].username : REDIS_NIL_VALUE);
		args.push_back(pUserInfo[i].nickname[0] != 0 ? pUserInfo[i].nickname : REDIS_NIL_VALUE);
		args.push_back(to_string(pUserInfo[i].clientip));
		args.push_back(to_string(pUserInfo[i].clientport));
		args.push_back(to_string(pUserInfo[i].sexy));
		args.push_back(to_string(pUserInfo[i].serverip));
		args.push_back(to_string(pUserInfo[i].lanip));
		args.push_back(pUserInfo[i].sessionkey[0] != 0 ? pUserInfo[i].sessionkey : REDIS_NIL_VALUE);
		args.push_back(pUserInfo[i].p2psessionkey[0] != 0 ? pUserInfo[i].p2psessionkey : REDIS_NIL_VALUE);
	}

	RedisReply reply;
	try {
		redis_con_pool_->execMaster(
				[&keys, &args, &reply, this] (RedisCon* con)
				{
					reply = luas_[set_user_info]->exec(con, keys, args);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (!reply.noError()) {
		return 1;
	}

	return 0;
}

int IMRedis::setUserInfo(CUser* pUser, uint8 flag)
{
	static const uint32_t s_field_count = 12;

	vector<string> keys, args;
	args.reserve(s_field_count);


	keys.push_back(keyUserInfo(pUser->m_nID));

	args.push_back(to_string(flag));
	args.push_back(to_string(pUser->m_nStatus));
	args.push_back(to_string(pUser->m_AllowRule));
	args.push_back(pUser->m_szUserName[0] != 0 ? pUser->m_szUserName : REDIS_NIL_VALUE);
	args.push_back(pUser->m_szNickName[0] != 0 ? pUser->m_szNickName : REDIS_NIL_VALUE);
	args.push_back(to_string(pUser->m_nIP));
	args.push_back(to_string(pUser->m_nPort));
	args.push_back(to_string(pUser->m_nSexy));
	args.push_back(to_string(im_info_.server_ip));
	args.push_back(to_string(pUser->m_nLanIP));
	args.push_back(pUser->m_SessionKey[0] != 0 ? pUser->m_SessionKey : REDIS_NIL_VALUE);
	args.push_back(pUser->m_P2PSessionKey[0] != 0 ? pUser->m_P2PSessionKey : REDIS_NIL_VALUE);


	RedisReply reply;
	try {
		redis_con_pool_->execMaster(
				[&keys, &args, &reply, this] (RedisCon* con)
				{
					reply = luas_[set_user_info]->exec(con, keys, args);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (!reply.noError()) {
		return 1;
	}

	return 0;
}

int IMRedis::delUserInfo(const uint32 * uid,uint8 & count,uint32 * aOKUserID)
{
	vector<string> keys;
	keys.reserve(count);
	for (uint32_t i = 0; i < count; ++i) {
		keys.push_back(keyUserInfo(uid[i]));
	}

	RedisReply reply;
	try {
		redis_con_pool_->execMaster(
				[&keys, &reply, this] (RedisCon* con)
				{
					reply = luas_[del_user_info]->exec(con, keys);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (!reply.noError()) {
		count = 0;
		return 1;
	}

	return 0;
}

int IMRedis::getUserInfo(const uint32_t * uid, uint8_t count,
		tagGSRV_GET_ONLINE_FULLINFO_ACK::UserInfo * pUserInfo)
{
	const static string s_pre_cmd = " flag status version username nickname clientip clientport"
			" sexy serverip sessionkey p2psessionkey";

	vector<string> commands;
	commands.reserve(count);

	for (uint8_t i = 0; i < count; ++i) {
		string command = "HMGET " + keyUserInfo(uid[i]) + s_pre_cmd;
		commands.emplace_back(std::move(command));
	}

	vector<RedisReply> replys;
	try {
		redis_con_pool_->execSlave(
				[&commands, &replys] (RedisCon* con)
				{
					replys = con->pipelining(commands);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	for (uint32_t i = 0; i < count; ++i) {
		pUserInfo[i].id = uid[i];

		const vector<RedisReply> &elements = replys[i].elements();
		if (!elements.empty()) {
			uint32_t flag = elements[0].len() ? std::atol(elements[0].str()) : 0;
			pUserInfo[i].status = elements[1].len() ? std::atol(elements[1].str()) : 0;
			pUserInfo[i].version = elements[2].len() ? std::atol(elements[2].str()) : 0;

			if (elements[3].len()) {
				strcpy(pUserInfo[i].username,elements[3].str());
			} else {
				pUserInfo[i].username[0] = 0;
			}

			if (elements[4].len()) {
				strcpy(pUserInfo[i].nickname,elements[3].str());
			} else {
				pUserInfo[i].nickname[0] = 0;
			}

			pUserInfo[i].clientip = elements[5].len() ? std::atol(elements[5].str()) : 0;
			pUserInfo[i].clientport = elements[6].len() ? std::atol(elements[6].str()) : 0;
			pUserInfo[i].sexy = elements[7].len() ? std::atol(elements[7].str()) : 0;
			pUserInfo[i].serverip = elements[8].len() ? std::atol(elements[8].str()) : 0;

			if (elements[9].len()) {
				strcpy(pUserInfo[i].sessionkey, elements[8].str());
			} else {
				pUserInfo[i].sessionkey[0] = 0;
			}

			if (elements[10].len()) {
				strcpy(pUserInfo[i].p2psessionkey, elements[9].str());
			} else {
				pUserInfo[i].p2psessionkey[0] = 0;
			}
		}
	}

	return 0;
}

int IMRedis::setUserStatus(const tagGSRV_MODIFY_ONLINE_STATUS_QUERY::tagUser * pUserStatus,
			uint8 & count,tagGSRV_MODIFY_ONLINE_STATUS_ACK::tagUser * pUStatusNow)
{
	vector<string> keys, args;
	keys.reserve(count);
	args.reserve(count);

	for (uint32_t i = 0; i < count; ++i) {
		keys.push_back(keyUserInfo(pUserStatus[i].id));
		args.push_back(to_string(pUserStatus[i].status));
	}

	RedisReply reply;
	try {
		redis_con_pool_->execMaster(
				[&keys, &args, &reply, this] (RedisCon* con)
				{
					reply = luas_[set_user_status]->exec(con, keys, args);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	return 0;
}

int IMRedis::getUserStatus(const uint32_t* uid,tagGSRV_GET_USER_STATUS_ACK::tagUserStatus * pUStatus,uint8_t& count)
{
	const static string s_pre_cmd = " status";

	vector<string> commands;
	commands.reserve(count);

	for (uint8_t i = 0; i < count; ++i) {
		string command = "HGET " + keyUserInfo(uid[i]) + s_pre_cmd;
		commands.emplace_back(std::move(command));
	}

	vector<RedisReply> replys;
	try {
		redis_con_pool_->execSlave(
				[&commands, &replys] (RedisCon* con)
				{
					replys = con->pipelining(commands);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	for (uint32_t i = 0; i < count; ++i) {
		pUStatus[i].id = uid[i];
		if (!replys[i].noError()) {
			pUStatus[i].status = XTREAM_OFFLINE;
		} else {
			pUStatus[i].status = atoi(replys[i].str());
		}
	}

	return 0;
}

int IMRedis::setLastContacts(uint32_t uid, uint32_t cid)
{
	using namespace std::chrono;
	system_clock::time_point time_now = system_clock::now();
	microseconds epoch =
			duration_cast<microseconds>(time_now.time_since_epoch());
	int64_t rank = epoch.count();

	vector<string> keys;
	keys.emplace_back(std::move(keyUserContacts(uid)));
	keys.emplace_back(to_string(rank));
	keys.emplace_back(std::move(to_string(cid)));

	RedisReply reply;
	try {
		redis_con_pool_->execMaster(
				[&keys, &reply, this] (RedisCon* con)
				{
					reply = luas_[set_last_contacts]->exec(con, keys);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	//std::cout << "reply " << reply.integer() << std::endl;
	return reply.noError() ? 0 : 1;
}

int IMRedis::getLastContacts(uint32_t uid, std::vector<uint32_t>& contacts, uint32_t count)
{
	contacts.clear();

	string command = "ZRANGE " + keyUserContacts(uid) + " -" + to_string(count) + " -1";
	RedisReply reply;
	try {
		redis_con_pool_->execSlave(
				[&command, &reply] (RedisCon* con)
				{
					reply = con->command(command);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (!reply.noError()) {
		return 1;
	} else {
		const vector<RedisReply>& elements = reply.elements();
		contacts.reserve(elements.size());
		for (const auto& i: elements) {
			contacts.push_back(std::stol(i.str()));
		}
	}

	return 0;
}

int IMRedis::getUserOnline(tagGSRV_SEARCH_ONLINE_USER_ACK_HEAD::tagUserInfo * pUserInfo,uint8 & count,uint32& nOnlineNum)
{

	vector<string> keys;
	keys.push_back(to_string(count));

	nOnlineNum = 0;
	count = 0;

	RedisReply reply;
	try {
		redis_con_pool_->execSlave(
				[&keys, &reply, this] (RedisCon* con)
				{
					reply = luas_[get_user_online]->exec(con, keys);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (!reply.noError()) {
		return 1;
	} else {
		const vector<RedisReply>& elements = reply.elements();
		if (elements.empty()) {
			return 1;
		}

		count = elements.size() - 1;

		for (uint32_t i = 0; i < elements.size() - 1; ++i) {
			const vector<RedisReply>& fields = elements[i].elements();
			if (!fields.empty()) {
				pUserInfo[i].clientid = idFromUserinfo(fields[0].str());
				pUserInfo[i].sexy = std::atol(fields[1].str());
				pUserInfo[i].status = std::atol(fields[2].str());
				pUserInfo[i].rule = std::atol(fields[3].str());
				if (fields[4].len()) {
					strcpy(pUserInfo[i].username, fields[4].str());
				} else {
					pUserInfo[i].username[0] = 0;
				}

				if (fields[5].len()) {
					strcpy(pUserInfo[i].nickname, fields[5].str());
				} else {
					pUserInfo[i].nickname[0] = 0;
				}
			}
		}

		nOnlineNum = elements.back().integer();
	}

	return 0;
}

int IMRedis::getUserControlInfo(const uint32 * uid,tagGSRV_USERCONTROLINFO_QUERY_ACK::tagUserControlInfo * pUserControlInfo,uint8 count)
{
	const static string s_pre_cmd = " serverip sessionkey p2psessionkey flag";

	vector<string> commands;
	commands.reserve(count);

	for (uint8_t i = 0; i < count; ++i) {
		string command = "HMGET " + keyUserInfo(uid[i]) + s_pre_cmd;
		commands.emplace_back(std::move(command));
	}

	vector<RedisReply> replys;
	try {
		redis_con_pool_->execSlave(
				[&commands, &replys] (RedisCon* con)
				{
					replys = con->pipelining(commands);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	for (uint32_t i = 0; i < count; ++i) {
		pUserControlInfo[i].uid = uid[i];

		const vector<RedisReply> &elements = replys[i].elements();
		if (!elements.empty()) {
			pUserControlInfo[i].srvip = elements[0].len() ? std::atoi(elements[0].str()) : 0;
			if (elements[1].len()) {
				strcpy(pUserControlInfo[i].sessionKey, elements[1].str());
			} else {
				pUserControlInfo[i].sessionKey[0] = 0;
			}

			if (elements[2].len()) {
				strcpy(pUserControlInfo[i].p2pSessionKey, elements[2].str());
			} else {
				pUserControlInfo[i].p2pSessionKey[0] = 0;
			}
			pUserControlInfo[i].flag = elements[3].len() ? std::atoi(elements[3].str()) : 0;
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
int IMRedis::getFamilyInfo(const uint32 * pFamilyID,uint8 & count,
		tagGSRV_FAMILYINFO_QUERY_ACK::tagFamilyInfo * pFamilyInfo,uint8 type)
{
	vector<string> commands;
	commands.reserve(count);

	for (uint32_t i = 0; i < count; ++i) {
		string key = type == FAMILYINFO_QUERY_TYPE_PERM ?
				keyPermFamilyIP(pFamilyID[i]) : keyTempFamilyIP(pFamilyID[i]);
		commands.push_back("GET " + key);
	}

	vector<RedisReply> replys;
	try {
		redis_con_pool_->execSlave(
				[&commands, &replys] (RedisCon* con)
				{
					replys = con->pipelining(commands);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	count = 0;
	for (uint32_t i = 0; i < replys.size(); ++i) {
		pFamilyInfo[i].fid = pFamilyID[i];
		pFamilyInfo[i].srvip = replys[i].len() ? std::atol(replys[i].str()) : 0;
		++count;
	}

	return 0;
}

int IMRedis::setFamilyInfo(const tagGSRV_FAMILYINFO_ADD_QUERY::tagFamilyInfo * pFamilyInfo,
		uint8 & count,uint32 * pFamilyID,uint8 type)
{
	vector<string> commands;
	commands.reserve(count);

	for (uint32_t i = 0; i < count; ++i) {
		string key = type == FAMILYINFO_QUERY_TYPE_PERM ?
				keyPermFamilyIP(pFamilyInfo[i].FamilyID) :
				keyTempFamilyIP(pFamilyInfo[i].FamilyID);
		commands.push_back("SET " + key + " " + to_string(pFamilyInfo[i].ServerIp));
	}

	vector<RedisReply> replys;
	try {
		redis_con_pool_->execMaster(
				[&commands, &replys] (RedisCon* con)
				{
					replys = con->pipelining(commands);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (pFamilyID != nullptr) {
		for (uint32_t i = 0; i < replys.size(); ++i) {
			if (replys[i].noError()) {
				pFamilyID[i] = pFamilyInfo[i].FamilyID;
			}
		}
	}

	return 0;
}

int IMRedis::delFamilyInfo(const uint32 * pDelFamilyID,uint8 & count,
		uint32 * pOKFamilyID,uint8 type)
{
	vector<string> commands;
	commands.reserve(count);

	for (uint32_t i = 0; i < count; ++i) {
		string key = type == FAMILYINFO_QUERY_TYPE_PERM ?
				keyPermFamilyIP(pDelFamilyID[i]) :
				keyTempFamilyIP(pDelFamilyID[i]);
		commands.push_back("DEL " + key);
	}

	vector<RedisReply> replys;
	try {
		redis_con_pool_->execMaster(
				[&commands, &replys] (RedisCon* con)
				{
					replys = con->pipelining(commands);
				});
	} catch (const redis_transport_except& e) {
		return 1;
	} catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
		return 1;
	}

	if (pOKFamilyID != nullptr) {
		for (uint32_t i = 0; i < replys.size(); ++i) {
			if (replys[i].noError()) {
				pOKFamilyID[i] = pDelFamilyID[i];
			}
		}
	}

	return 0;
}

