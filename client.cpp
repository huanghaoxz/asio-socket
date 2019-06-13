//
// Created by huanghao on 19-5-22.
//
#include "client.h"

boost::asio::io_service m_ios;//这个必须是全局变量，不然定时器和程序会出问题,具体原因没有分析出来

CClient::CClient(std::string ip, short port):m_ep(boost::asio::ip::address::from_string(ip),port) {
    m_talk_to_server = CTalk_to_server::create_client(m_ep,m_ios);
}

CClient::~CClient() {

}

void CClient::start() {

    start_listen();
}

void CClient::stop()
{
    m_talk_to_server->stop();
}

void CClient::send_msg(std::string & msg)
{
    m_talk_to_server->do_write(msg);
}

void CClient::set_receive_data(void *receivedata) {
    m_talk_to_server->set_receive_data(receivedata);
}

bool CClient::get_client_status()
{
    return m_talk_to_server->started();
}

void CClient::start_listen() {

    for (int i = 0; i < THREAD_NUM; ++i) {
        boost::thread(boost::bind(&CClient::handle_talk_to_server_thread, this));
    }
}

void CClient::handle_talk_to_server_thread()
{
    boost::asio::io_service::work work(m_ios);
    m_ios.run();
}