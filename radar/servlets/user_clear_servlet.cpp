#include "user_clear_servlet.h"
#include "log.h"
#include "user_info.h"
#include "user_manager.h"
#include "radar_util.h"


namespace radar {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
UserClearServlet::UserClearServlet() 
    :BlogServlet("UserClearServlet") {
}

// 处理请求
// 清空数据库(调试用)
int32_t UserClearServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    result->setResult(200, "ok");
    do {
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }

        auto datas = user::UserMgr::GetInstance()->getDatas();
        for(auto& i : datas) {
            if(user::data::UserInfoDao::Delete(i.second, db)) {
                result->setResult(500, "db delete error");
                break;
            }
        }
    } while(0);
    
    SYLAR_LOG_INFO(g_logger) << result->toJsonString();
    response->setBody(result->toJsonString());
    return 0;
}

}
}