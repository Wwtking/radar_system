#include "radar_data_trans_servlet.h"
#include "log.h"
#include "radar_data_message.h"
#include "util.h"
#include "struct.h"
#include <cstdlib> // 包含 rand() 和 srand()
#include <ctime>   // 包含 time() 函数

namespace radar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static int32_t SendMessage(sylar::http::WSSession::ptr session, RadarDataMessage::ptr cmsg) {
    SYLAR_LOG_INFO(g_logger) << cmsg->toString() << " - " << session;
    return session->sendMessage(cmsg->toString()) > 0 ? 0 : -1;
}

/**
 * @brief 构造函数
 */
RadarDataWSServlet::RadarDataWSServlet() 
    :WSServlet("RadarDataWSServlet") {
}

/**
 * @brief 握手建立连接成功的回调函数
 * @param[in] req HTTP请求
 * @param[in] session WebSocket连接
 * @return 是否处理成功
 */
int32_t RadarDataWSServlet::onConnect(sylar::http::HttpRequest::ptr req, 
                                sylar::http::WSSession::ptr session) {
    SYLAR_LOG_INFO(g_logger) << "onConnect " << session;
    return 0;
}

/**
 * @brief 挥手关闭连接成功的回调函数
 * @param[in] req HTTP请求
 * @param[in] session WebSocket连接
 * @return 是否处理成功
 */
int32_t RadarDataWSServlet::onClose(sylar::http::HttpRequest::ptr req, 
                            sylar::http::WSSession::ptr session) {
    SYLAR_LOG_INFO(g_logger) << "onClose " << session;
    return 0;
}

/**
 * id: 
 * x: 
 * y: 
 * v: 
*/

/**
 * @brief 双向数据传输时的处理回调函数
 * @param[in] req HTTP请求
 * @param[in] msg 接收到的WS消息体封装
 * @param[in] session WebSocket连接
 * @return 是否处理成功
 */
int32_t RadarDataWSServlet::handle(sylar::http::HttpRequest::ptr req, 
                            sylar::http::WSFrameMessage::ptr msg,
                            sylar::http::WSSession::ptr session) {
    SYLAR_LOG_INFO(g_logger) << "handle " << session 
                            << " opcode=" << msg->getOpcode()
                            << " data=" << msg->getData();

    RadarDataMessage::ptr rsp(new RadarDataMessage);
    auto cmsg = RadarDataMessage::Create(msg->getData());
    std::string type = cmsg->get("type");
    
    if(type == "start_request") {
        rsp->set("type", "radar_data");
        srand(time(0));
        while(1) {
            /// TODO 接收、解析、存储雷达数据
            int peopleCount = 1;            // 雷达解析后的数据
            for(int i = 1; i <= peopleCount; ++i) {
                rsp->set("id", "1");
                rsp->set("x", std::to_string(rand() % 101));
                rsp->set("y", std::to_string(rand() % 101));
                rsp->set("v", std::to_string(rand() % 101));
            }
            SendMessage(session, rsp);
            sleep(1);
        }
    }

    // if(type == "start_request") {
    //     rsp->set("type", "start_response");
    //     rsp->set("result", "200");
    //     rsp->set("msg", "ok");

    //     return SendMessage(session, rsp);
    // }
    // else if(type == "end_request") {
    //     rsp->set("type", "end_response");
    //     rsp->set("result", "200");
    //     rsp->set("msg", "ok");

    //     return SendMessage(session, rsp);
    // }

    return 0;
}

}

