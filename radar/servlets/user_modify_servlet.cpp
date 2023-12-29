#include "user_modify_servlet.h"
#include "log.h"
#include "user_info.h"
#include "user_manager.h"
#include "radar_util.h"


namespace radar {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
UserModifyServlet::UserModifyServlet() 
    :BlogServlet("UserModifyServlet") {
}

// 处理请求
int32_t UserModifyServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 获取浏览器界面输入的name、passwd、email
        std::string name = request->getParam("name");
        std::string passwd = request->getParam("passwd");
        std::string email = request->getParam("email");

        // 获取Session，如果Session中没有USER_ID，说明当前没有账号登录
        auto sdata = getSessionData(request, response);
        auto uid = sdata->getData<int64_t>(CookieKey::USER_ID);
        if(!uid) {
            result->setResult(408,  "not login");
            break;
        }

        // 信息全为空
        if(name.empty() && passwd.empty() && email.empty()) {
            result->setResult(400, "no param");
            break;
        }

        // 修改后端存储的UserInfo
        user::data::UserInfo::ptr info = user::UserMgr::GetInstance()->get(uid);
        if(!info) {
            result->setResult(403, "invalid account");
            break;
        }
        if(!name.empty()) {
            info->setName(name);
        }
        if(!passwd.empty()) {
            info->setPasswd(passwd);
        }
        if(!email.empty()) {
            if(user::UserMgr::GetInstance()->getByEmail(email)) {
                result->setResult(401, "email exists");
                break;
            }
            if(!is_email(email)) {
                result->setResult(402, "invalid email format");
                break;
            }
            // 加入原邮箱验证码验证
            // info->setName(email);
        }

        // 更新修改时间，并更新到数据库中
        info->setUpdateTime(time(0));
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(user::data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }

        result->setResult(200, "ok");

        // 更新Cookie参数，因为token与密码passwd有关
        if(!passwd.empty()) {
            int64_t token_time = time(0) + 3600 * 24;
            std::string token = user::UserMgr::GetInstance()->GetToken(info, token_time);
            response->setCookie(CookieKey::USER_ID, std::to_string(info->getId()), token_time, "/");
            response->setCookie(CookieKey::TOKEN, token, token_time, "/");
            response->setCookie(CookieKey::TOKEN_TIME, std::to_string(token_time), token_time, "/");
        }

    } while(0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}