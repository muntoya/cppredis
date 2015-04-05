/*
 * RedisConPool.h
 *
 *  Created on: Jan 8, 2015
 *      Author: yaoxing
 */

#ifndef REDISCONPOOL_H_
#define REDISCONPOOL_H_

#include <boost/noncopyable.hpp>
#include <memory>
#include <deque>
#include <string>
#include <map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "RedisCon.h"
#include "RedisMQ.h"


class RedisConPool: boost::noncopyable
{
public:
	using MQCallBack = std::function<void(const std::string&, const std::string&)>;
	using MQMap = std::map<std::string, MQCallBack>;

	struct Attr
	{
		std::map<std::string, int32_t> sentinel_hosts;		//所有sentinel的IP和端口
		std::string master_name;							//sentinel配置中的master name
		uint32_t master_cnt = 3;							//master连接数
		uint32_t slave_cnt = 3;								//slave的连接数
		uint32_t reconnect_interval = 1;					//主从切换时的连接重试间隔（秒）
		MQMap MQ_map;										//消息队列的名字与回调函数
	};

	enum Role: size_t
	{
		master = 0,
		slave = 1,

		role_cnt,
	};

	RedisConPool(const Attr& attr);
	~RedisConPool();

	//连接，可以重复调用
	void connect();

	//连接slave执行redis命令
	void execSlave(const std::function<void(RedisCon*)>& f);

	//连接master执行redis命令
	void execMaster(const std::function<void(RedisCon*)>& f);

private:
	using Con = std::unique_ptr<RedisCon>;
	using Cons = std::deque<Con>;

	Con getCon(Role role);
	void releaseCon(Con&& con, Role role);

	//生成一个可用的sentinel连接
	Con createSentinelCon();

	//创建连接池中全部的连接
	void createMasterCons(const Con&);
	void createSlaveCons(Con&);

	Con createMasterCon();
	Con createSlaveCon();

	//任意一个连接出错时调用
	void reconnect();

	void exec(const std::function<void(RedisCon*)>& f, Role role = Role::slave);

private:
	bool connected{false};

	Cons cons_[role_cnt];
	std::mutex mutex_[role_cnt];
	std::condition_variable cond[role_cnt];

	Attr attr_;

	uint32_t pool_epoch_{0};
	std::chrono::system_clock::time_point t;

	std::unique_ptr<RedisMQ> redis_MQ_;

	static const std::chrono::duration<int> timeout;
};

#endif /* REDISCONPOOL_H_ */
