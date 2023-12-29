#include "user_active_servlet.h"
#include "log.h"
#include "user_info.h"
#include "user_manager.h"
#include "radar_util.h"


namespace radar {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
UserActiveServlet::UserActiveServlet() 
    :BlogServlet("UserActiveServlet") {
}

// 处理请求
// auth_id | string | 是 | 账号/邮箱
// auth_code | string | 是 | 验证码
int32_t UserActiveServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 宏定义出来的 auth_id、auth_code 是用户在浏览器界面输入的，检查其是否为空
        DEFINE_AND_CHECK_STRING(result, auth_id, "auth_id");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");

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

        // 没获取到账号/邮箱，说明在/creat界面输入的账号/邮箱和/active的不一样
        if(!info) {
            result->setResult(403, "different auth_id");
            break;
        }

        // 获取账号/邮箱对应的状态，只有在验证状态时才是合理的
        if(info->getState() != 1) {
            result->setResult(410, "account invalid state");
            break;
        }

        // 验证界面输入验证码和邮箱发送的验证码是否一致
        if(auth_code != info->getCode()) {
            result->setResult(411, "invalid verification code");
            break;
        }

        // 清空存储验证码，设置状态、更新时间
        info->setCode("");
        info->setState((int)State::PUBLISH);
        info->setUpdateTime(time(0));

        // 获取数据库，并将数据库中info更新
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        if(user::data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }
        
        // SendWX("blog", "用户激活成功[" + auth_id + "]");
        result->setResult(200, "ok");
    } while(0);

    response->setBody(result->toJsonString());
    return 0;
}

}
}