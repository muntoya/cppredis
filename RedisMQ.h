/*
 * RedisMQ.h
 * 消息队列
 *
 *  Created on: Feb 5, 2015
 *      Author: yaoxing
 */

#ifndef REDISMQ_H_
#define REDISMQ_H_

#include <string>
#include <functional>
#include <map>
#include <thread>
#include <memory>

class RedisCon;

class RedisMQ
{
public:
	using Con = std::unique_ptr<RedisCon>;

	//收到消息后执行的回调函数
	using MQCallBack = std::function<void(const std::string&, const std::string&)>;

	//消息队列名字与回调函数映射
	using MQMap = std::map<std::string, MQCallBack>;

	//获取redis连接的回调函数，必须返回master连接
	using ConCallBack = std::function<Con()>;


	RedisMQ(ConCallBack con_callback, const MQMap& MQ_map, uint32_t timeout = 2);
	~RedisMQ();

private:
	void run();
	void process();
	void connect();

private:
	ConCallBack con_callback_;
	Con con_;										//redis连接，必须是master

	MQMap MQ_map_;
	std::string MQ_command_;

	std::thread thread_;

	bool running_;

	static const uint32_t s_retry_time;				//故障后重连的间隔时间
};

#endif /* REDISMQ_H_ */
