//
// Created by huanghao on 19-5-15.
//
#include <iostream>
#include <boost/thread.hpp>
#include "talk_to_server.h"
#include "hb_log4def.h"

//CTalk_to_server::CTalk_to_server() : m_socket(m_service),m_timer(m_service,boost::bind(&CTalk_to_server::start_timer,this)){
CTalk_to_server::CTalk_to_server(boost::asio::io_service &ios) : m_service(ios), m_timer(ios), m_socket(ios) {
    m_bStart = false;
    m_timer.expires_from_now(boost::posix_time::seconds(3));
    m_timer.async_wait(boost::bind(&CTalk_to_server::start_timer, this, boost::asio::placeholders::error));
}

CTalk_to_server::~CTalk_to_server() {

}

void CTalk_to_server::start_timer(const boost::system::error_code &err) {

    if (!err) {
        if (!m_bStart) {
            hbla_log_info("timer reconnect  ...");
            //m_socket.async_connect(m_ep, boost::bind(&CTalk_to_server::handle_connect, shared_from_this(),
                                                    // boost::asio::placeholders::error));
            m_socket.async_connect(m_ep, boost::bind(&CTalk_to_server::handle_connect, this,boost::asio::placeholders::error));
        }
    } else {
        hbla_log_error("start_timer %s", err.message().c_str());
    }
    m_timer.expires_at(m_timer.expires_at() + posix_time::millisec(1000));
    m_timer.async_wait(boost::bind(&CTalk_to_server::start_timer, this, boost::asio::placeholders::error));
}

talk_to_server_ptr CTalk_to_server::create_client(boost::asio::ip::tcp::endpoint ep, boost::asio::io_service &ios) {
    talk_to_server_ptr new_(new CTalk_to_server(ios));
    new_->start(ep);
    return new_;
}

void CTalk_to_server::start(boost::asio::ip::tcp::endpoint ep) {
    m_ep = ep;
    m_socket.async_connect(ep, boost::bind(&CTalk_to_server::handle_connect, shared_from_this(),
                                           boost::asio::placeholders::error));

}

void CTalk_to_server::handle_connect(const boost::system::error_code &err) {
    if (!err) {
        m_bStart = true;
        hbla_log_info("%d connect server sucess", m_socket.native());
        do_read();
    } else {
        hbla_log_error("handle_connect err %s", err.message().c_str())
        if (m_socket.is_open()) {
            m_socket.close();
        }
        m_bStart = false;
    }
}

void CTalk_to_server::stop() {

    if (!m_bStart)
        return;
    m_bStart = false;
    m_socket.close();
}

bool CTalk_to_server::started() const {
    return m_bStart;
}

void CTalk_to_server::do_read() {
    boost::shared_ptr<std::vector<char>> read_ptr(new std::vector<char>(max_msg, 0));
    m_socket.async_read_some(boost::asio::buffer(*read_ptr),
                             boost::bind(&CTalk_to_server::handle_read, shared_from_this(), read_ptr,
                                         boost::asio::placeholders::error,
                                         boost::asio::placeholders::bytes_transferred()));
}

void CTalk_to_server::handle_read(boost::shared_ptr<std::vector<char>> read_ptr, const boost::system::error_code &err,
                                  size_t bytes) {
    if (!err)//没有错误
    {
        string message = "";
        message.assign(read_ptr->begin(), read_ptr->begin() + bytes);
        cout << "handle read" << message << endl;
        m_receive_data(message, bytes, m_socket.native());
    } else {
        hbla_log_error("handle_read %s", err.message().c_str());
        if (m_socket.is_open()) {
            m_socket.close();
            m_bStart = false;
        }
    }
}

void CTalk_to_server::do_write(const std::string &msg) {
    if (!m_bStart) {
        return;
    }
    cout << "write:" << msg << endl;
    m_socket.async_write_some(buffer(msg), boost::bind(&CTalk_to_server::handle_write, shared_from_this(),
                                                       boost::asio::placeholders::error,
                                                       boost::asio::placeholders::bytes_transferred()));
}

void CTalk_to_server::handle_write(const boost::system::error_code &err, size_t bytes) {
    if (err) {
        hbla_log_error("handle_write err %s", err.message().c_str());
        if (m_socket.is_open()) {
            m_socket.close();
        }
        m_bStart = false;
    } else {
        do_read();
    }
}

void CTalk_to_server::set_receive_data(void *receivedata) {
    m_receive_data = (ReceiveData) (receivedata);
}

/*
void CTalk_to_server::start_listen() {
    m_service.run();
    //return;
    //hh 假如用下面的代码,有问题,定时器时而管用，时而不管用,现在想不清为什么,难道给 boost::asio::strand有关
    for (int i = 0; i < THREAD_NUM; ++i) {
        boost::thread(boost::bind(&CTalk_to_server::handle_talk_to_server_thread, this));
    }
}

void CTalk_to_server::handle_talk_to_server_thread() {
    boost::asio::io_service::work work(m_service);
    m_service.run();
}
*/