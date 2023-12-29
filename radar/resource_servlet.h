#ifndef __SYLAR_RESOURCE_SERVLET_H__
#define __SYLAR_RESOURCE_SERVLET_H__

#include "http_servlet.h"

namespace sylar {
namespace http {

class ResourceServlet : public HttpServlet {
public:
    typedef std::shared_ptr<ResourceServlet> ptr;

    /**
     * @brief 构造函数
    */
    ResourceServlet(const std::string& path);

    /**
     * @brief 处理请求
     * @param[in] req HTTP请求
     * @param[in] res HTTP响应
     * @param[in] session HTTP连接
     * @return 是否处理成功
     */
    virtual int32_t handle(HttpRequest::ptr req, 
                           HttpResponse::ptr res, 
                           HttpSession::ptr session) override;

private:
    std::string m_path;
};

}
}


#endif