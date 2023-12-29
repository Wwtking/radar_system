#include "user_login_servlet.h"
#include "log.h"
#include "user_info.h"
#include "user_manager.h"
#include "radar_util.h"


namespace radar {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
UserLoginServlet::UserLoginServlet() 
    :BlogServlet("UserLoginServlet") {
}

// 处理请求
// auth_id | string | 是 | 账号/邮箱
// passwd | string | 是 | 密码（MD5）
int32_t UserLoginServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 宏定义出来的 auth_id、passwd 是用户在浏览器界面输入的，检查其是否为空
        DEFINE_AND_CHECK_STRING(result, auth_id, "auth_id");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");

        // 获取Session，如果Session中有USER_ID，说明已经登录
        auto sdata = getSessionData(request, response);
        if(sdata->getData<int64_t>(CookieKey::USER_ID)) {
            result->setResult(409, "already login");
            break;
        }

        user::data::UserInfo::ptr info;
        // 验证账号和邮箱是否合法
        // 验证完后，获取在create后端存储过的账号/邮箱
        if(is_valid_account(auth_id)) {
            info = user::UserMgr::GetInstance()->getByAccount(auth_id);
        }
        else if(is_email(auth_id)) {
            info = user::UserMgr::GetInstance()->getByEmail(auth_id);
        }
        else {
            result->setResult(402, "invalid auth_id");
            break;
        }

        // 账号/邮件未注册
        if(!info) {
            result->setResult(403, "auth_id not registered");
            break;
        }

        // 获取账号/邮箱对应的状态，只有在注册成功时才能正常登录
        if(info->getState() != 2) {
            result->setResult(410, "account invalid state");
            break;
        }

        // 密码输入错误
        if(passwd != info->getPasswd()) {
            result->setResult(411, "incorrect password");
            break;
        }

        // // 获取Session，如果Session中有USER_ID且等于info对应ID，说明该账号已登录
        // auto sdata = getSessionData(request, response);
        // int64_t userId = sdata->getData<int64_t>(CookieKey::USER_ID);
        // if(userId && userId == info->getId()) {
        //     result->setResult(409, "account already login");
        //     break;
        // }

        // 更新登录时间，并更新到数据库中
        info->setLoginTime(time(0));
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(user::data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }

        // 将前端需要的数据放在http响应body中返回
        result->setResult(200, "ok");
        result->set("username", info->getName());
        result->set("account", info->getAccount());
        result->set("userid", info->getId());

        // 服务端在HTTP响应协议中告诉客户端Cookie参数(USER_ID、TOKEN、TOKEN_TIME)
        // 在以后该客户端每次请求时要把Cookie参数发送到服务器，服务器就可以识别
        // 一个server端的响应：
        //         HTTP/2.0 200 OK
        //         Content-Type: text/html
        //         Set-Cookie: name=hani
        //         Set-Cookie: site=www.hani.com
        // 当浏览器接收到这个响应之后，就会在本地的cookies中设置对应的值
        //         GET /test.html HTTP/2.0
        //         Host: www.hani.com
        //         Cookie: name=hani; site=www.hani.com
        int64_t token_time = time(0) + 3600 * 24;
        std::string token = user::UserMgr::GetInstance()->GetToken(info, token_time);
        response->setCookie(CookieKey::USER_ID, std::to_string(info->getId()), token_time, "/");
        response->setCookie(CookieKey::TOKEN, token, token_time, "/");
        response->setCookie(CookieKey::TOKEN_TIME, std::to_string(token_time), token_time, "/");

        // Session中存放USER_ID
        sdata->setData(CookieKey::USER_ID, info->getId());
    } while(0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}