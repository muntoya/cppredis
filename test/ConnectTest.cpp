/*
 * ConnectTest.cpp
 *
 *  Created on: Jan 7, 2015
 *      Author: yaoxing
 */

#define BOOST_TEST_MODULE redis_interface_test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <chrono>
#include "RedisCon.h"

using namespace std;
using namespace std::chrono;

struct connect_fixture
{
	connect_fixture(): conn_(3, "10.211.240.56")
	{
	};
	~connect_fixture() {};

	RedisCon conn_;
};

BOOST_FIXTURE_TEST_SUITE(s_connect, connect_fixture);

#if 0
BOOST_AUTO_TEST_CASE(t_set)
{

	RedisReply reply = conn_.command("SET int 2");
	vector<string> cmds;
	cmds.push_back("get int");
	cmds.push_back("get int1");
	cmds.push_back("HMSET userinfo:3 status 0 rule 0 username ye nickname wahaha clientip 5 clientport 4235 sexy 0 serverip 4 lanip 4325 sessionkey wocao p2psessionkey nima");
	vector<RedisReply> replys = conn_.pipelining(cmds);
	for (auto& i: replys) {
		cout << i.str() << endl;
	}
	return;

	system_clock::time_point old_time = system_clock::now();
	for (int i = 0; i < 100000; ++i) {
		reply = conn_.command("GET int");
		BOOST_CHECK_EQUAL(reply.str(), string("2"));
	}
	system_clock::time_point new_time = system_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(new_time - old_time);

	cout << time_span.count() << endl;
}
#endif

BOOST_AUTO_TEST_SUITE_END()
