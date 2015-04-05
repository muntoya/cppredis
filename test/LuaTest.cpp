

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <chrono>
#include <time.h>
#include <thread>
#include "RedisLua.h"

using namespace std;

struct lua_fixture
{
	lua_fixture() {}
	~lua_fixture() {}
};

BOOST_FIXTURE_TEST_SUITE(s_con_pool, lua_fixture);

#if 0
BOOST_AUTO_TEST_CASE(t_lua_string)
{
	string script(R"("wocao!\n hahahaha")");
	RedisLua rl(script);
	vector<string> keys = {"wo", "cao", "ri", "nima"};
	vector<string> args = {"zhong", "hua", "ren", "min"};

//	BOOST_MESSAGE(rl.commands(keys, args, false));
//	BOOST_MESSAGE(rl.commands(keys, args));
}
#endif

BOOST_AUTO_TEST_SUITE_END()
