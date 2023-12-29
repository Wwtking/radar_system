#include "user_logout_servlet.h"
#include "log.h"
#include "user_info.h"
#include "user_manager.h"
#include "radar_util.h"


namespace radar {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
UserLogoutServlet::UserLogoutServlet() 
    :BlogServlet("UserLogoutServlet") {
}

// 处理请求
int32_t UserLogoutServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 获取Session，如果Session中没有USER_ID，说明未登录
        auto sdata = getSessionData(request, response);
        if(!sdata->getData<int64_t>(CookieKey::USER_ID)) {
            result->setResult(408, "not login");
            break;
        }

        result->setResult(200, "ok");

        // 清空Cookie参数(USER_ID、TOKEN、TOKEN_TIME)
        int64_t token_time = time(0) - 3600 * 24;
        response->setCookie(CookieKey::USER_ID, "", token_time, "/");
        response->setCookie(CookieKey::TOKEN, "", token_time, "/");
        response->setCookie(CookieKey::TOKEN_TIME, "", token_time, "/");

        // 清空Session中存放USER_ID
        sdata->setData(CookieKey::USER_ID, (int64_t)0);
        // 删除Session
        sylar::http::SessionDataMgr::GetInstance()->del(sdata->getId());
    } while(0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}