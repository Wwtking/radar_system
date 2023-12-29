#include "user_forget_passwd_servlet.h"
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
UserForgetPasswdServlet::UserForgetPasswdServlet() 
    :BlogServlet("UserForgetPasswdServlet") {
}

// 处理请求
// email | string | 是 | 邮箱
int32_t UserForgetPasswdServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 宏定义出来的 email 是用户在浏览器界面输入的，检查其是否为空
        DEFINE_AND_CHECK_STRING(result, email, "email");

        // 验证邮箱是否合法
        if(!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }

        // 检查邮箱是否已注册过
        user::data::UserInfo::ptr info = user::UserMgr::GetInstance()->getByEmail(email);
        if(!info) {
            result->setResult(401, "email not register");
            break;
        }

        // 设置验证码
        std::string v = sylar::random_string(16);
        for(auto& i : v) {
            SYLAR_ASSERT(i);
        }
        info->setCode(v);

        // 获取当前数据库
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        // 开启事务
        auto trans = db->openTransaction();
        // 将用户数据保存到数据库中
        if(user::data::UserInfoDao::Update(info, db)) {
            result->setResult(500, "db update error");
            break;
        }

        // 通过邮件发送验证码
        //////////////// 加隐私保护
        auto mail = sylar::EMail::Create("164510644@qq.com", "arkicmeuplxsbhcd"
                                        , "ChatRoom 重置密码 - 验证码"
                                        , "验证码 [" + v + "]", {email}, {}, {});
        // "smtp.qq.com"是QQ电子邮件服务器的地址，用于发送和接收邮件的服务器
        auto client = sylar::SmtpClient::Create("smtp.qq.com", 465, true);
        if(!client) {
            SYLAR_LOG_ERROR(g_logger) << "connect email server fail";
            result->setResult(501, "connect email server fail");
            break;
        }
        // 发送邮件
        auto rt = client->send(mail, 5000);
        if(rt->result != 0) {
            result->setResult(501, "send email fail: " + std::to_string(rt->result) + " " + rt->msg);
            // SendWX("blog", "邮件发送失败: " + std::to_string(rt->result) + " " + rt->msg + " email:" + email);
            break;
        }

        // 提交事务
        trans->commit();
        result->setResult(200, "ok");
    } while(0);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}