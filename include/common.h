//
// Created by huanghao on 19-5-21.
//

#ifndef HBAUDITFLOW_COMMON_H
#define HBAUDITFLOW_COMMON_H

#define MAX_MSG_NUM 1024
#define THREAD_NUM 1
#define MAX_MSG 1024

class CTalk_to_client;
typedef boost::shared_ptr<CTalk_to_client> client_ptr;
//定义一个回调函数将数据传输到应用层
typedef void(*ReceiveData)(std::string & message,int size,int fd);

//typedef void(*ReceiveData_server)(std::string & message,int size,client_ptr &ptr);

class CTalk_to_server;
typedef boost::shared_ptr<CTalk_to_server>  talk_to_server_ptr;


typedef std::vector<client_ptr> array_clients;

#endif //HBAUDITFLOW_COMMON_H
