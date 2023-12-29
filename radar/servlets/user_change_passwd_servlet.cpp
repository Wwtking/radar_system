#include "user_change_passwd_servlet.h"
#include "log.h"
#include "macro.h"
#include "user_manager.h"
#include "user_info.h"
#include "radar_util.h"
#include "smtp.h"


namespace radar {
namespace servlet {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 构造函数
UserChangePasswdServlet::UserChangePasswdServlet() 
    :BlogServlet("UserChangePasswdServlet") {
}

// 处理请求
// email | string | 是 | 邮箱
// auth_code | string | 是 | 验证码
// passwd | string | 是 | 密码（MD5）
int32_t UserChangePasswdServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 宏定义出来的 email、auth_code、passwd 是用户在浏览器界面输入的，检查其是否为空
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, auth_code, "auth_code");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");

        // 验证邮箱是否合法
        if(!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }

        // 没获取到账号/邮箱，说明在/creat界面输入的账号/邮箱和/active的不一样
        user::data::UserInfo::ptr info = user::UserMgr::GetInstance()->getByEmail(email);
        if(!info) {
            result->setResult(403, "different auth_id");
            break;
        }

        // 验证界面输入验证码和邮箱发送的验证码是否一致
        if(auth_code != info->getCode()) {
            result->setResult(411, "invalid verification code");
            break;
        }

        // 重置后的密码与之前密码相同
        if(info->getPasswd() == passwd) {
            result->setResult(407, "same passwd");
            break;
        }

        // 清空存储验证码，更改密码、更新时间
        info->setCode("");
        info->setPasswd(passwd);
        info->setUpdateTime(time(0));

        // 获取当前数据库
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        // 将用户数据保存到数据库中
        if(user::data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }

        result->setResult(200, "ok");
    } while(0);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}