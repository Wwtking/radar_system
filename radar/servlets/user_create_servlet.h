#ifndef __RADAR_SERVLETS_USER_CREATE_SERVLET_H__
#define __RADAR_SERVLETS_USER_CREATE_SERVLET_H__

#include "http_servlet.h"
#include "struct.h"

namespace radar {
namespace servlet {

/**
 * @brief 用户注册账号
*/
class UserCreateServlet : public radar::BlogServlet {
public:
    typedef std::shared_ptr<UserCreateServlet> ptr;

    /**
     * @brief 构造函数
    */
    UserCreateServlet();

    /**
     * @brief 处理请求
     * @param[in] req HTTP请求
     * @param[in] res HTTP响应
     * @param[in] session HTTP连接
     * @param[in] result 状态码
     * @return 是否处理成功
     */
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) override;

};


}
}

#endif