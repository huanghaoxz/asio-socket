//
// Created by huanghao on 19-5-15.
//

#ifndef HBAUDITFLOW_CTALK_TO_SERVER_H
#define HBAUDITFLOW_CTALK_TO_SERVER_H

#include <string>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "common.h"
#include "a_timer.h"


using namespace std;
using namespace boost;
using namespace boost::asio;


class CTalk_to_server :public boost::enable_shared_from_this<CTalk_to_server>,boost::noncopyable
{
public:
    CTalk_to_server(boost::asio::io_service &ios);
    ~CTalk_to_server();
    static talk_to_server_ptr create_client(boost::asio::ip::tcp::endpoint ep,boost::asio::io_service &ios);
    void start(boost::asio::ip::tcp::endpoint ep);
    void handle_connect(const boost::system::error_code& err);
    void stop();
    bool started() const;
    void do_read();
    void do_write(const std::string &msg);
    void handle_write(const boost::system::error_code &err,size_t bytes);
    void handle_read(boost::shared_ptr<std::vector<char>> read_ptr,const boost::system::error_code& err,size_t bytes);
    void set_receive_data(void* receivedata);
    //void start_listen();
    //void handle_talk_to_server_thread();
    void start_timer(const boost::system::error_code& err);
private:
    boost::asio::io_service& m_service;
    deadline_timer m_timer;
    boost::asio::ip::tcp::socket m_socket;
    boost::asio::ip::tcp::endpoint m_ep;
    enum {max_msg =  MAX_MSG_NUM};
    bool m_bStart;
    char m_read_buffer[MAX_MSG];
    char m_write_buffer[MAX_MSG];
    ReceiveData m_receive_data;
    //string m_ip;
    //short m_port;

   // boost::asio::strand m_strand;
};

#endif //HBAUDITFLOW_CTALK_TO_SERVER_H
