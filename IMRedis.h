/*
 * IMRedis.h
 *
 *  Created on: Jan 15, 2015
 *      Author: yaoxing
 */

#ifndef IMREDIS_H_
#define IMREDIS_H_

#include <vector>
#include <mutex>
#include <memory>
#include <boost/noncopyable.hpp>
#include "SrvFrameProtocol.h"
#include "src/MiniIM/User.h"
#include "RedisConPool.h"
#include "RedisLua.h"

#define IM_REDIS (IMRedis::getInstance())

class IMRedis: boost::noncopyable
{
	IMRedis();
	~IMRedis();
public:

	static IMRedis* getInstance();

	struct IMInfo
	{
		uint32_t server_ip = 0;
	};

	void setIMInfo(const IMInfo& info) {im_info_ = info;}

	//获取用户信息
	int getUserInfo(const uint32_t * uid, uint8_t count,
			tagGSRV_GET_ONLINE_FULLINFO_ACK::UserInfo * pUserInfo);

	//删除用户信息
	int delUserInfo(const uint32 * uid,uint8 & count,uint32 * aOKUserID = nullptr);

	//增加或修改用户信息
	int setUserInfo(const tagGSRV_ADDMODIFY_FULLUSERINFO_QUERY::UserInfo * pUserInfo,
			uint8 flag = XT_HEAD::FLAG_CNTREQUEST, uint8_t count = 1);
	int setUserInfo(CUser* pUser, uint8 flag = XT_HEAD::FLAG_CNTREQUEST);

	//获取用户在线状态
	int getUserStatus(const uint32_t* uid,
			tagGSRV_GET_USER_STATUS_ACK::tagUserStatus * pUStatus,uint8_t& count);

	//修改用户在线状态
	int setUserStatus(const tagGSRV_MODIFY_ONLINE_STATUS_QUERY::tagUser * pUserStatus,
			uint8 & count,tagGSRV_MODIFY_ONLINE_STATUS_ACK::tagUser * pUStatusNow = nullptr);

	//获取最近联系人
	int getLastContacts(uint32_t uid, std::vector<uint32_t>& contacts, uint32_t count = 1000);

	//增加最近联系人
	int setLastContacts(uint32_t uid, uint32_t cid);

	//查询在线用户
	int getUserOnline(tagGSRV_SEARCH_ONLINE_USER_ACK_HEAD::tagUserInfo * pUserInfo,
			uint8 & count,uint32& nOnlineNum);

	//查询在线用户控制信息
	int getUserControlInfo(const uint32 * uid,
			tagGSRV_USERCONTROLINFO_QUERY_ACK::tagUserControlInfo * pUserControlInfo,uint8 count);

/////////////////////////////////////////////////////////////////////////////////////////////////////
	//查询全局家族资料
	int getFamilyInfo(const uint32 * pFamilyID,uint8 & count,
			tagGSRV_FAMILYINFO_QUERY_ACK::tagFamilyInfo * pFamilyInfo,
			uint8 type = FAMILYINFO_QUERY_TYPE_PERM);

	//添加全局家族资料
	int setFamilyInfo(const tagGSRV_FAMILYINFO_ADD_QUERY::tagFamilyInfo * pFamilyInfo,
			uint8 & count,uint32 * pFamilyID = nullptr,uint8 type = FAMILYINFO_QUERY_TYPE_PERM);

	//删除全局家族资料
	int delFamilyInfo(const uint32 * pDelFamilyID,uint8 & count,
			uint32 * pOKFamilyID = nullptr,uint8 type = FAMILYINFO_QUERY_TYPE_PERM);

private:
	void initLua();

	inline std::string keyUserInfo(uint32_t id);
	inline std::string keyUserContacts(uint32_t id);
	inline std::string keyUserOnline();
	inline uint32_t idFromUserinfo(const std::string& key);

	inline std::string keyTempFamilyIP(uint32_t id);
	inline std::string keyPermFamilyIP(uint32_t id);

private:
	static std::once_flag s_instance_flag;
	static IMRedis* s_im_redis;

	static const std::string s_lua_get_last_contacts;
	static const std::string s_lua_set_last_contacts;
	static const std::string s_lua_set_user_info;
	static const std::string s_lua_del_user_info;
	static const std::string s_lua_set_user_status;
	static const std::string s_lua_get_user_online;

private:
	enum LuaType: uint32_t
	{
		get_last_contacts = 0,
		set_last_contacts,

		set_user_info,
		del_user_info,

		set_user_status,

		get_user_online,

		lua_end,
	};

	std::unique_ptr<RedisLua> luas_[lua_end];

	std::unique_ptr<RedisConPool> redis_con_pool_;

	IMInfo im_info_;
};

#endif /* IMREDIS_H_ */
