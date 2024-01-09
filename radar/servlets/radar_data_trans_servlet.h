#ifndef __SYLAR_RADAR_DATA_TRANS_SERVLET_H__
#define __SYLAR_RADAR_DATA_TRANS_SERVLET_H__

#include "ws_servlet.h"
#include "radar_data_message.h"
#include "singleton.h"

namespace radar {


class RadarDataWSServlet : public sylar::http::WSServlet {
public:
    typedef std::shared_ptr<RadarDataWSServlet> ptr;

    /**
     * @brief 构造函数
     */
    RadarDataWSServlet();

    /**
     * @brief 握手建立连接成功的回调函数
     * @param[in] req HTTP请求
     * @param[in] session WebSocket连接
     * @return 是否处理成功
     */
    virtual int32_t onConnect(sylar::http::HttpRequest::ptr req, 
                              sylar::http::WSSession::ptr session) override;

    /**
     * @brief 双向数据传输时的处理回调函数
     * @param[in] req HTTP请求
     * @param[in] msg 接收到的WS消息体封装
     * @param[in] session WebSocket连接
     * @return 是否处理成功
     */
    virtual int32_t handle(sylar::http::HttpRequest::ptr req, 
                           sylar::http::WSFrameMessage::ptr msg,
                           sylar::http::WSSession::ptr session) override;

    /**
     * @brief 挥手关闭连接成功的回调函数
     * @param[in] req HTTP请求
     * @param[in] session WebSocket连接
     * @return 是否处理成功
     */
    virtual int32_t onClose(sylar::http::HttpRequest::ptr req, 
                            sylar::http::WSSession::ptr session) override;

};

} // namespace radar

#endif