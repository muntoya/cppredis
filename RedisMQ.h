/*
 * RedisMQ.h
 * ��Ϣ����
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

	//�յ���Ϣ��ִ�еĻص�����
	using MQCallBack = std::function<void(const std::string&, const std::string&)>;

	//��Ϣ����������ص�����ӳ��
	using MQMap = std::map<std::string, MQCallBack>;

	//��ȡredis���ӵĻص����������뷵��master����
	using ConCallBack = std::function<Con()>;


	RedisMQ(ConCallBack con_callback, const MQMap& MQ_map, uint32_t timeout = 2);
	~RedisMQ();

private:
	void run();
	void process();
	void connect();

private:
	ConCallBack con_callback_;
	Con con_;										//redis���ӣ�������master

	MQMap MQ_map_;
	std::string MQ_command_;

	std::thread thread_;

	bool running_;

	static const uint32_t s_retry_time;				//���Ϻ������ļ��ʱ��
};

#endif /* REDISMQ_H_ */
