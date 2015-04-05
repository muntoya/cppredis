/*
 * MQTest.cpp
 *
 *  Created on: Feb 5, 2015
 *      Author: yaoxing
 */

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <chrono>
#include <time.h>
#include <thread>
#include <functional>
#include "RedisCon.h"
#include "RedisMQ.h"

using namespace std;

struct mq_fixture
{
	mq_fixture() {}
	~mq_fixture() {}
};

void print(const string& name, const string& msg)
{
	cout << name << " " << msg << endl;
}

unique_ptr<RedisCon> con_func()
{

	unique_ptr<RedisCon> con;
	try {
		con.reset(new RedisCon(3, "10.211.64.80"));
	} catch (...) {

	}
	return std::move(con);
}

BOOST_FIXTURE_TEST_SUITE(s_con_pool, mq_fixture);

#if 1
BOOST_AUTO_TEST_CASE(t_mq_fail)
{

	RedisMQ::MQMap mq_map;
	mq_map["mq1"] = print;
	mq_map["mq2"] = print;

	RedisMQ mq(con_func, mq_map, 2);

	RedisCon con2(3, "10.211.64.80");
	try {
		for (uint32_t i = 0; i < 20; ++i) {

			RedisReply reply = con2.command("LPUSH mq1 " + to_string(i * 2));
			con2.command("LPUSH mq2 " + to_string(i * 3));
			sleep(1);
		}
	} catch (...) {
		cout << "stop" << endl;
	}


	cout << "shutdown" << endl;
	sleep(100);
}
#endif

BOOST_AUTO_TEST_SUITE_END()



