#include "user_create_servlet.h"
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
UserCreateServlet::UserCreateServlet() 
    :BlogServlet("UserCreateServlet") {
}

// 处理请求
// account | string | 是 | 账号
// email | string | 是 | 邮箱
// passwd | string | 是 | 密码（MD5）
int32_t UserCreateServlet::handle(sylar::http::HttpRequest::ptr request
                                ,sylar::http::HttpResponse::ptr response
                                ,sylar::http::HttpSession::ptr session
                                ,Result::ptr result) {
    do {
        // 宏定义出来的 account、email、passwd 是用户在浏览器界面输入的，检查其是否为空
        DEFINE_AND_CHECK_STRING(result, account, "account");
        DEFINE_AND_CHECK_STRING(result, email, "email");
        DEFINE_AND_CHECK_STRING(result, passwd, "passwd");

        // 检查账号和邮箱是否已注册过
        if(user::UserMgr::GetInstance()->getByAccount(account)) {
            result->setResult(401, "account exists");
            break;
        }
        if(user::UserMgr::GetInstance()->getByEmail(email)) {
            result->setResult(401, "email exists");
            break;
        }

        // 验证账号和邮箱是否合法
        if(!is_valid_account(account)) {
            result->setResult(402, "invalid account");
            break;
        }
        if(!is_email(email)) {
            result->setResult(402, "invalid email format");
            break;
        }

        // 获取当前数据库
        sylar::IDB::ptr db = GetDB();
        if(!db) {
            result->setResult(500, "get db error");
            break;
        }
        // 开启事务
        sylar::ITransaction::ptr trans = db->openTransaction();

        // 设置验证码
        std::string v = sylar::random_string(16);
        SYLAR_LOG_INFO(g_logger) << "[" << v << "]";
        for(auto& i : v) {
            SYLAR_ASSERT(i);
        }

        // 将用户数据保存到后端
        user::data::UserInfo::ptr info(new user::data::UserInfo);
        info->setAccount(account);
        info->setEmail(email);
        info->setPasswd(passwd);
        info->setName(account);
        info->setCode(v);
        info->setState((int)State::VERIFYING);
        info->setCreateTime(time(0));
        info->setUpdateTime(time(0));

        // 将用户数据保存到数据库中
        if(user::data::UserInfoDao::Insert(info, db)) {
            result->setResult(500, "db insert user info fail");
            break;
        }

        // 通过邮件发送验证码
        //////////////// 加隐私保护
        auto mail = sylar::EMail::Create("164510644@qq.com", "arkicmeuplxsbhcd"
                                        , "Chat Create Account Auth - 验证码"
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
        user::UserMgr::GetInstance()->add(info);
        result->setResult(200, "ok");
        SYLAR_LOG_INFO(g_logger) << info->toJsonString();
    } while(0);
    
    response->setBody(result->toJsonString());
    return 0;
}

}
}