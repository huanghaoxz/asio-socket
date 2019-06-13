//
// Created by huanghao on 19-5-13.
//

#ifndef HBAUDITFLOW_CTALK_TO_CLIENT_H
#define HBAUDITFLOW_CTALK_TO_CLIENT_H

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "common.h"


using namespace std;
using namespace boost;
using namespace boost::asio;


class CTalk_to_client:public boost::enable_shared_from_this<CTalk_to_client>,boost::noncopyable
 {
public:
    CTalk_to_client(boost::asio::io_service &service);

    ~CTalk_to_client();

    static client_ptr new_client(boost::asio::io_service &service);

    void start();

    void stop();

    void do_read();
    void handle_read(boost::shared_ptr<std::vector<char>> read_ptr,const boost::system::error_code& err,size_t bytes);


    void do_write(std::string &messsage);
    void handle_write(const boost::system::error_code& err,size_t bytes);

    boost::asio::ip::tcp::socket &get_socket();

    void set_client_changed();

    void set_receive_data(void* receivedata);
    static int clientnum;
    void del_client();
private:
    boost::asio::ip::tcp::socket m_socket;
    bool m_bStart;
    enum {max_msg = MAX_MSG_NUM};
    ReceiveData m_receive_data;
    boost::asio::ip::tcp::endpoint m_ep;
    bool m_client_changed;
};


#endif //HBAUDITFLOW_CTALK_TO_CLIENT_H
