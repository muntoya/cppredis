/*
 * RedisConPool.cpp
 *
 *  Created on: Jan 8, 2015
 *      Author: yaoxing
 */

#include <cassert>
#include <iostream>
#include "RedisException.h"
#include "RedisConPool.h"

using std::unique_lock;
using std::mutex;

const std::chrono::duration<int> RedisConPool::timeout(2);

RedisConPool::RedisConPool(const Attr& attr)
: attr_(attr)
, t(std::chrono::system_clock::now())
{

}

RedisConPool::~RedisConPool()
{

}

void RedisConPool::connect()
{
    ++pool_epoch_;

	Con sentinel_con = createSentinelCon();


    if (sentinel_con == nullptr) {
    	throw sentinel_connect_except("no available sentinel");
    }

    createMasterCons(sentinel_con);
    createSlaveCons(sentinel_con);

    connected = true;
}

void RedisConPool::reconnect()
{
	using namespace std::chrono;

	if (connected) {
		return;
	}

	unique_lock<mutex> lock_m(mutex_[Role::master]);
	unique_lock<mutex> lock_s(mutex_[Role::slave]);

	duration<int> interval = duration_cast<duration<int>>(system_clock::now() - t);
	if (static_cast<uint32_t>(interval.count()) > attr_.reconnect_interval) {
		std::cout << "reconnect" << std::endl;
		connect();
		t = std::chrono::system_clock::now();
	} else {
		std::cout << "can't reconnect" << std::endl;
	}

}

void RedisConPool::execSlave(const std::function<void(RedisCon*)>& f)
{
	exec(f, Role::slave);
}

void RedisConPool::execMaster(const std::function<void(RedisCon*)>& f)
{
	exec(f, Role::master);
}

void RedisConPool::exec(const std::function<void(RedisCon*)>& f, Role role)
{
	reconnect();

	Con con = getCon(role);
	auto releaser = [&con, role, this] (void*) {releaseCon(std::move(con), role);};
	std::unique_ptr<Con, decltype(releaser)> guard(&con, releaser);

	try {
		f(con.get());
	} catch (const redis_transport_except& e) {
		connected = false;
		throw;
	}
}

void RedisConPool::createMasterCons(const Con& sentinel_con)
{
	cons_[Role::master].clear();

	RedisReply reply = sentinel_con->command("SENTINEL master %s", attr_.master_name.c_str());
	const std::vector<RedisReply>& master = reply.elements();
	assert(!master.empty());
	assert(master.at(1).str() == attr_.master_name);

	if (master.at(9).str() == std::string("master")) {
		for (uint32_t i = 0; i < attr_.master_cnt; ++i) {
			Con master_con(new RedisCon(
					pool_epoch_, master.at(3).str(), std::stoul(master.at(5).str())));
			cons_[Role::master].emplace_back(std::move(master_con));
		}
	} else {
		throw redis_master_except("can't connet to master");
	}
}

void RedisConPool::createSlaveCons(Con& sentinel_con)
{
	cons_[Role::slave].clear();

	RedisReply reply = sentinel_con->command("SENTINEL slaves %s", attr_.master_name.c_str());
    const std::vector<RedisReply>& slaves = reply.elements();
	assert(!slaves.empty());

	for (uint32_t i = 0; i < attr_.slave_cnt; ++i) {
		for (auto& slave: slaves) {
			const std::vector<RedisReply>& slave_attr = slave.elements();
			if (slave_attr.at(9).str() == std::string("slave")) {
				Con slave_con(new RedisCon(
						pool_epoch_, slave_attr.at(3).str(), std::stoul(slave_attr.at(5).str())));
				cons_[Role::slave].emplace_back(std::move(slave_con));
			}
		}
	}

	if (cons_[Role::slave].empty()) {
		throw redis_slave_except("can't connect to slave");
	}
}

RedisConPool::Con RedisConPool::createSentinelCon()
{
	Con sentinel_con;
    for(const auto& host: attr_.sentinel_hosts)
    {
		try {
			sentinel_con.reset(new RedisCon(pool_epoch_, host.first, host.second));
		} catch (const redis_connect_except& e) {
			continue;
		}
	}

    return std::move(sentinel_con);
}

RedisConPool::Con RedisConPool::createMasterCon()
{
	Con sentinel_con = createSentinelCon();

	RedisReply reply = sentinel_con->command("SENTINEL master %s", attr_.master_name.c_str());
	const std::vector<RedisReply>& master = reply.elements();
	assert(!master.empty());
	assert(master.at(1).str() == attr_.master_name);

	Con master_con;
	if (master.at(9).str() == std::string("master")) {
		for (uint32_t i = 0; i < attr_.master_cnt; ++i) {
			try {
				master_con.reset(new RedisCon(
						pool_epoch_, master.at(3).str(), std::stoul(master.at(5).str())));
			} catch (const redis_connect_except& e) {
				continue;
			}

			if (master_con != nullptr) {
				break;
			}
		}
	}

	return std::move(master_con);
}

RedisConPool::Con RedisConPool::createSlaveCon()
{

}

RedisConPool::Con RedisConPool::getCon(Role role)
{
	unique_lock<mutex> lock(mutex_[role]);
	bool ret = cond[role].wait_for(lock, timeout,
			[this, role] ()->bool {return !cons_[role].empty();});
	if (!ret) {
		throw redis_transport_except("can't get redis connnection");
	}

	assert(!cons_[role].empty());
	Con conn = std::move(cons_[role].front());
	cons_[role].pop_front();
	return conn;
}

void RedisConPool::releaseCon(Con&& con, Role role)
{
	unique_lock<mutex> lock(mutex_[role]);
	if (con->epoch() == pool_epoch_) {
		cons_[role].emplace_back(std::move(con));
		cond[role].notify_one();
	}
}

