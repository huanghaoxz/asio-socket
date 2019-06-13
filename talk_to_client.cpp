//
// Created by huanghao on 19-5-13.
//

#include <iostream>
#include <boost/thread/recursive_mutex.hpp>
#include "talk_to_client.h"
#include "hb_log4def.h"

int CTalk_to_client::clientnum = 0;
array_clients m_clients;
boost::recursive_mutex m_cs;
CTalk_to_client::CTalk_to_client(boost::asio::io_service &service):m_socket(service){
    m_bStart = false ;
}

CTalk_to_client::~CTalk_to_client() {

}

client_ptr CTalk_to_client::new_client(boost::asio::io_service &service) {

    client_ptr client(new CTalk_to_client(service));
    return client;
}

void CTalk_to_client::start() {
    boost::recursive_mutex::scoped_lock lk(m_cs);
    m_bStart = true;
    m_clients.push_back(shared_from_this());
    hbla_log_info("client count %d is connected",++CTalk_to_client::clientnum);
    do_read();
}

void CTalk_to_client::stop() {
    if(!m_bStart)
    {
        return;
    }
    m_bStart = false;
    m_socket.close();
    del_client();
}

void CTalk_to_client::del_client() {
    boost::recursive_mutex::scoped_lock lk(m_cs);
    array_clients::iterator it = std::find(m_clients.begin(),m_clients.end(),shared_from_this());
    if(it != m_clients.end())
    {
        m_clients.erase(it);
        hbla_log_info("client count %d is connected",--CTalk_to_client::clientnum);
    }
}

void CTalk_to_client::do_read() {
    if (!m_bStart) {
        return;
    }
    //每个客户端读取自己的
    boost::shared_ptr<std::vector<char>> read_ptr(new std::vector<char>(max_msg, 0));
    m_socket.async_read_some(boost::asio::buffer(*read_ptr),
                             boost::bind(&CTalk_to_client::handle_read, shared_from_this(), read_ptr, _1, _2));
}

void CTalk_to_client::handle_read(boost::shared_ptr<std::vector<char>> read_ptr, const boost::system::error_code &err,
                             size_t bytes) {
    if (!err)//没有错误
    {
        string message = "" ;
        message.assign(read_ptr->begin(),read_ptr->begin()+bytes);
        m_receive_data(message,bytes,m_socket.native());
        do_read();
    } else {
        //可以将保存的数组减少1
        hbla_log_error("%d  %s",err.value(),err.message().c_str());
        m_bStart = false;
        m_socket.close();
        del_client();
    }
}

void CTalk_to_client::set_receive_data(void* receivedata)
{
    m_receive_data = (ReceiveData)(receivedata);
}

boost::asio::ip::tcp::socket & CTalk_to_client::get_socket(){
    return m_socket;
}

void CTalk_to_client::do_write(std::string &messsage)
{
    m_socket.async_write_some(buffer(messsage),boost::bind(&CTalk_to_client::handle_write,shared_from_this(),_1,_2));
}

void CTalk_to_client::handle_write(const boost::system::error_code &err, size_t bytes) {
    if(!err)
    {
        //do_read();
    } else
    {
        hbla_log_error("write_handler err:%s",err.message().c_str());
        m_bStart = false;
        m_socket.close();
        del_client();
    }
}


void CTalk_to_client::set_client_changed() {
    m_client_changed = true;
}
