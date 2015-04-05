/*
 * RedisTools.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: yaoxing
 */


#include <sstream>
#include <fstream>
#include <cstdio>
#include <boost/filesystem.hpp>
#include <boost/uuid/sha1.hpp>
#include "RedisException.h"
#include "RedisTools.h"


const std::string RedisTools::lua_path = "Lua";

std::string RedisTools::to_sha1(const std::string& lua)
{
	using namespace boost::uuids::detail;
	sha1 sha;
	sha.process_bytes(lua.c_str(), lua.length());

	uint32_t dt[5];
	sha.get_digest(dt);

	char str[41];
	snprintf(str, 41, "%08x%08x%08x%08x%08x", dt[0], dt[1], dt[2], dt[3], dt[4]);

	return std::string(str);
}

std::string RedisTools::load_script(const std::string& file)
{
	using namespace boost::filesystem;
	path bin_path = read_symlink(path("/proc/self/exe"));
	path script_path = bin_path.parent_path() / path(lua_path) / path(file);
	if (!exists(script_path)) {
		throw redis_lua_except("can't fild file " + script_path.string());
	}

	std::ifstream ifs(script_path.string());
	std::stringstream buffer;
	buffer << ifs.rdbuf();

	return buffer.str();
}

