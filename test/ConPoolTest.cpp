/*
 * ConPoolTest.cpp
 *
 *  Created on: Jan 9, 2015
 *      Author: yaoxing
 */


#include <boost/test/unit_test.hpp>
#include <iostream>
#include <chrono>
#include <time.h>
#include <thread>
#include "RedisConPool.h"

using namespace std;
using namespace std::chrono;

struct con_pool_fixture
{

};

BOOST_FIXTURE_TEST_SUITE(s_con_pool, con_pool_fixture);

#if 0
BOOST_AUTO_TEST_CASE(t_muti_thread)
{
	RedisConPool::Attr attr;
	attr.sentinel_hosts["10.211.240.55"] = 26379;
	attr.master_name = "mymaster";
	attr.master_cnt = 2;
	attr.slave_cnt = 1;
	RedisConPool conn_pool(attr);
	conn_pool.connect();

	conn_pool.execMaster(
			[] (RedisCon* con)
			{
				con->command("set str haha");
			});

	system_clock::time_point old_time = system_clock::now();
	//conn_pool.test();
	std::vector<thread> threads;
	for (uint32_t i = 0; i < 10; ++i) {
		thread t([&conn_pool] ()
				{
					for (uint32_t cnt = 0; cnt < 1000; ++cnt) {
						try {
						conn_pool.execSlave(
								[] (RedisCon* con)
								{
									RedisReply reply = con->command("get str");
									cout << reply.str() << endl;
									sleep(10);
								});
						} catch (const std::exception& e) {
							cout << e.what() << endl;
						}
					}
				});
		threads.push_back(std::move(t));
	}

	for (auto& t: threads) {
		t.join();
	}
	system_clock::time_point new_time = system_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(new_time - old_time);
	cout << time_span.count() << endl;
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_read)
{
	RedisConPool::Attr attr;
	attr.sentinel_hosts["10.211.240.55"] = 26379;
	attr.master_name = "mymaster";
	attr.slave_cnt = 3;
	RedisConPool conn_pool(attr);
	conn_pool.connect();

	conn_pool.execMaster(
			[] (RedisCon* con)
			{
				con->command("set str 2");
			});

	RedisReply reply;

	system_clock::time_point old_time = system_clock::now();

	for (int i = 0; i < 10000; ++i) {
		conn_pool.execSlave(
				[&reply] (RedisCon* con)
				{
					reply = con->command("get str");
				});
	}

	system_clock::time_point new_time = system_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(new_time - old_time);
	cout << time_span.count() << endl;
}
#endif

BOOST_AUTO_TEST_SUITE_END()

