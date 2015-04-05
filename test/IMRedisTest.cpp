/*
 * IMRedisTest.cpp
 *
 *  Created on: Jan 16, 2015
 *      Author: yaoxing
 */


#include <boost/test/unit_test.hpp>
#include <iostream>
#include <chrono>
#include <time.h>
#include <thread>
#include "RedisConPool.h"
#include "IMRedis.h"

using namespace std;
using namespace std::chrono;

struct im_redis_fixture
{

};

BOOST_FIXTURE_TEST_SUITE(s_im_redis, im_redis_fixture);

#if 0
BOOST_AUTO_TEST_CASE(t_set_user_info)
{
	tagGSRV_ADDMODIFY_FULLUSERINFO_QUERY::UserInfo ui[50];
	for (uint32_t i = 0; i < 50; ++i) {
		ui[i].id = i;
		ui[i].status = i % 3;
		ui[i].rule = 0;
		ui[i].sexy = i % 2;
		ui[i].serverip = i * 6;
		ui[i].clientip = i * 5;
		ui[i].clientport = i * 4;
		ui[i].lanip = i * 3;
		strcpy(ui[i].nickname, "wahaha");
		strcpy(ui[i].p2psessionkey, "nima");
		strcpy(ui[i].sessionkey, "wocao");
		strcpy(ui[i].username, "ye");
	}

	system_clock::time_point old_time = system_clock::now();
	//conn_pool.test();
	std::vector<thread> threads;
	for (uint32_t i = 0; i < 1; ++i) {
		thread t([&ui] () {
			for (uint32_t i = 0; i < 1; ++i) {

				uint8_t cnt = 50;
				IM_REDIS->setUserInfo(ui, cnt);
				//BOOST_CHECK_EQUAL(cnt, 50);
			}
		});

		threads.emplace_back(std::move(t));
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
BOOST_AUTO_TEST_CASE(t_del_user_info)
{
	uint32_t uid[40];
	for (uint32_t i = 0; i < 40; ++i) {
		uid[i] = i;
	}
	uint8_t cnt = 40;
	uint32_t a;
	IM_REDIS->delUserInfo(uid, cnt, &a);
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_del_user_info)
{
	uint32_t uid[100];
	for (uint32_t i = 0; i < 100; ++i) {
		uid[i] = i;
	}

	uint8_t cnt = 100;
	cout << "del user info " << IM_REDIS->delUserInfo(uid, cnt) << endl;
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_get_user_info)
{
	uint32_t uid[50];
	for (uint32_t i = 0; i < 50; ++i) {
		uid[i] = i;
	}

	tagGSRV_GET_ONLINE_FULLINFO_ACK::UserInfo ui[50];
	uint8_t count = 50;

	IM_REDIS->getUserInfo(uid, count, ui);

	for (auto& i: ui) {
		cout << "-------------------------------------------------" << i.id  << "----------------------" << endl;
		cout << i.clientip << endl;
		cout << i.clientport << endl;
		cout << i.nickname << endl;
		cout << i.p2psessionkey << endl;
		cout << i.serverip << endl;
		cout << i.sessionkey << endl;
		cout << i.sexy << endl;
		cout << i.status << endl;
		cout << i.username << endl;
		cout << i.version << endl;
	}
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_get_user_status)
{
	tagGSRV_GET_USER_STATUS_ACK::tagUserStatus us[50];
	uint32_t uid[50];
	for (uint32_t i = 0; i < 50; ++i) {
		uid[i] = i;
	}

	system_clock::time_point old_time = system_clock::now();
	//conn_pool.test();
	std::vector<thread> threads;
	for (uint32_t i = 0; i < 10; ++i) {
		thread t([&uid, &us] () {
			for (uint32_t i = 0; i < 10000; ++i) {
				uint8_t cnt = 1;
				IM_REDIS->getUserStatus(uid, us, cnt);
			}
		});

		threads.emplace_back(std::move(t));
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
BOOST_AUTO_TEST_CASE(t_get_user_status)
{
	tagGSRV_MODIFY_ONLINE_STATUS_QUERY::tagUser u[100];
	for (uint32_t i = 0; i < 100; ++i) {
		u[i].id = i;
		u[i].status = i % 5;
	}

	uint8_t cnt = 100;
	IM_REDIS->setUserStatus(u, cnt);
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_set_last_contact)
{
	IM_REDIS->setLastContacts(5, 55);
	IM_REDIS->setLastContacts(5, 67);
	IM_REDIS->setLastContacts(5, 7667);
	IM_REDIS->setLastContacts(5, 5423);
	IM_REDIS->setLastContacts(5, 205);

	system_clock::time_point old_time = system_clock::now();
	std::vector<thread> threads;
	for (uint32_t i = 0; i < 1; ++i) {
		thread t([] () {
			for (uint32_t j = 1; j < 10000; ++j) {
				IM_REDIS->setLastContacts(5, j);
			}
		});
		threads.emplace_back(std::move(t));
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
BOOST_AUTO_TEST_CASE(t_get_last_contact)
{
	vector<uint32_t> ids;
	IM_REDIS->getLastContacts(5, ids);
	for (auto& i: ids) {
		cout << i << " ";
	}
	cout << endl;
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_get_control_info)
{
	tagGSRV_USERCONTROLINFO_QUERY_ACK::tagUserControlInfo uci[50];
	uint32_t uid[50];
	for (uint32_t i = 0; i < 50; ++i) {
		uid[i] = i;
	}
	IM_REDIS->getUserControlInfo(uid, uci, 50);
	for (auto& i: uci) {
		cout << i.uid << " " << (int)i.flag << " "
				<< i.p2pSessionKey << " " << i.sessionKey << " " << i.srvip << endl;
	}
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_get_user_online)
{
	tagGSRV_SEARCH_ONLINE_USER_ACK_HEAD::tagUserInfo uci[50];
	uint8_t cnt = 50;
	uint32_t total;
	IM_REDIS->getUserOnline(uci, cnt, total);
	cout << (int)cnt << " " << total << endl;
	for (uint32_t i = 0; i < cnt; ++i) {
		cout << uci[i].clientid << " " << (int)uci[i].rule << " " <<
				(int)uci[i].sexy << " " << (int)uci[i].status << " " << uci[i].username <<
				" " << uci[i].nickname << endl;
	}

	system_clock::time_point old_time = system_clock::now();
	//conn_pool.test();
	std::vector<thread> threads;
	for (uint32_t i = 0; i < 10; ++i) {
		thread t([&uci] () {
			for (uint32_t i = 0; i < 10000; ++i) {
				uint8_t cnt = 50;
				uint32_t total;
				IM_REDIS->getUserOnline(uci, cnt, total);
			}
		});

		threads.emplace_back(std::move(t));
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
BOOST_AUTO_TEST_CASE(t_set_family_info)
{
	tagGSRV_FAMILYINFO_ADD_QUERY::tagFamilyInfo pFamilyInfo[50];
	uint8_t cnt = 50;
	uint32_t ids[50];
	for (uint32_t i = 0; i < 50; ++i) {
		pFamilyInfo[i].FamilyID = i;
		pFamilyInfo[i].ServerIp = i * 5;
	}

	IM_REDIS->setFamilyInfo(pFamilyInfo, cnt, ids, FAMILYINFO_QUERY_TYPE_TEMP);

}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_get_family_info)
{
	tagGSRV_FAMILYINFO_QUERY_ACK::tagFamilyInfo pFamilyInfo[50];
	uint8_t cnt = 50;
	uint32_t ids[50];
	for (uint32_t i = 0; i < 50; ++i) {
		ids[i] = i;
	}

	IM_REDIS->getFamilyInfo(ids, cnt, pFamilyInfo, FAMILYINFO_QUERY_TYPE_TEMP);

	for (uint32_t i = 0; i < cnt; ++i) {
		cout << pFamilyInfo[i].fid << "\t" << pFamilyInfo[i].srvip << endl;
	}
}
#endif

#if 0
BOOST_AUTO_TEST_CASE(t_del_family_info)
{
	uint8_t cnt = 50;
	uint32_t ids[50];
	for (uint32_t i = 0; i < 50; ++i) {
		ids[i] = i;
	}

	IM_REDIS->delFamilyInfo(ids, cnt);
}
#endif

BOOST_AUTO_TEST_SUITE_END()
