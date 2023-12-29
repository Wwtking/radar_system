#include "resource_servlet.h"
#include "log.h"
#include <fstream>

namespace sylar {
namespace http {

static Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
ResourceServlet::ResourceServlet(const std::string& path) 
    :HttpServlet("ResourceServlet")
    ,m_path(path) {
}

// 处理请求
int32_t ResourceServlet::handle(HttpRequest::ptr req, 
                                HttpResponse::ptr res, 
                                HttpSession::ptr session) {
    std::string path = m_path + "/" + req->getPath();
    SYLAR_LOG_INFO(g_logger) << path;
    if(path.find("..") != std::string::npos) {
        res->setBody("invalid path");
        res->setStatus(HttpStatus::NOT_FOUND);
        return -1;
    }

    std::ifstream ifs(path);
    if(!ifs) {
        res->setBody("invalid file");
        res->setStatus(HttpStatus::NOT_FOUND);
        return -1;
    }

    std::stringstream ss;
    std::string line;
    while(getline(ifs, line)) {
        ss << line << std::endl;
    }

    res->setBody(ss.str());
    res->setHeader("content-type", "text/html;charset=utf-8");
    return 0;
}

}
}