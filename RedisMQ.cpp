/*
 * RedisMQ.cpp
 *
 *  Created on: Feb 5, 2015
 *      Author: yaoxing
 */

#include <cassert>
#include <stdio.h>
#include "RedisCon.h"
#include "RedisReply.h"
#include "RedisException.h"
#include "RedisMQ.h"

const uint32_t RedisMQ::s_retry_time = 1;

RedisMQ::RedisMQ(ConCallBack con_callback, const MQMap& MQ_map, uint32_t timeout)
: con_callback_(con_callback)
, running_(true)
{
	assert(con_callback_);

	MQ_map_ = MQ_map;

	MQ_command_ = "BRPOP";
	for (const auto &i: MQ_map_) {
		MQ_command_ += " " + i.first;
	}
	MQ_command_ += " " + std::to_string(timeout);


	if (!MQ_map_.empty()) {
		thread_ = std::thread(std::bind(&RedisMQ::run, this));
	}
}

RedisMQ::~RedisMQ()
{
	running_ = false;
	thread_.join();
}

void RedisMQ::run()
{
	while (running_) {
		process();
	}
}

void RedisMQ::process()
{
	if (con_ == nullptr || !con_->isValid()) {
		connect();
		sleep(s_retry_time);
		return;
	}

	RedisReply reply;

	try {
		reply = con_->command(MQ_command_);
	} catch (const redis_transport_except& e) {
		return;
	}

	if (!reply.noError()) {
		if (reply.readOnly()) {
			con_.release();
		}
		return;
	}

	const auto& elements = reply.elements();
	assert(elements.size() == 2);

	std::string queue_name = elements[0].str();
	std::string message = elements[1].str();

	auto ifind = MQ_map_.find(queue_name);
	assert(ifind != MQ_map_.end());

	ifind->second(queue_name, message);
}

void RedisMQ::connect()
{
	printf("MQ reconnect\n");
	con_ = con_callback_();
}

