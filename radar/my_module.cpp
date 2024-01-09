#include "my_module.h"
#include "application.h"
#include "http_server.h"
#include "ws_server.h"
#include "resource_servlet.h"
#include "mysqlite3.h"
#include "user_info.h"
#include "user_manager.h"
#include "env.h"
#include "user_create_servlet.h"
#include "user_active_servlet.h"
#include "user_login_servlet.h"
#include "user_logout_servlet.h"
#include "user_modify_servlet.h"
#include "user_forget_passwd_servlet.h"
#include "user_change_passwd_servlet.h"
#include "user_clear_servlet.h"
#include "radar_data_trans_servlet.h"

namespace radar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

static sylar::ConfigVar<std::string>::ptr g_sqlite3_db_name = 
    sylar::Config::Lookup("sqlite3.db_name", std::string("radar.db"), "sqlite3 db name");

MyModule::MyModule() 
    :Module("radar_system", "1.0", "") {
}

bool MyModule::onLoad() {
    SYLAR_LOG_INFO(g_logger) << "onLoad";
    return true;
}

bool MyModule::onUnLoad() {
    SYLAR_LOG_INFO(g_logger) << "onUnLoad";
    return true;
}

bool MyModule::onServerReady() {
    SYLAR_LOG_INFO(g_logger) << "onServerReady";

    // 创建数据库db
    auto work_path = sylar::Config::Lookup<std::string>("server.work_path");
    auto db_path = work_path->getValue() + "/" + g_sqlite3_db_name->getValue();
    sylar::SQLite3::ptr db;
    // 如果db_path路径下数据库已经存在，则以READWRITE形式返回现有的数据库
    db = sylar::SQLite3::Create(db_path, sylar::SQLite3::READWRITE);
    if(!db) {
        SYLAR_LOG_INFO(g_logger) << "init database begin";
        // 创建新的数据库
        db = sylar::SQLite3::Create(db_path);
        if(!db) {
            SYLAR_LOG_ERROR(g_logger) << "open database db=" << db_path << " failed";
            return false;
        }
        // 创建表
#define XX(clazz, t) \
        if(clazz::CreateTableSQLite3(db)) { \
            SYLAR_LOG_ERROR(g_logger) << "create table " t " failed"; \
            return false; \
        }
        XX(user::data::UserInfoDao, "user");
#undef XX
        SYLAR_LOG_INFO(g_logger) << "init database end";
    }

    // 加载数据库所有用户数据，存放到后端类中
#define XX(clazz) \
    if(!clazz::GetInstance()->loadAll()) { \
        SYLAR_LOG_ERROR(g_logger) << #clazz " load all fail"; \
    }
    XX(user::UserMgr);
#undef XX

    // 获取http服务器
    std::vector<sylar::TcpServer::ptr> servers;
    if(!sylar::Application::Getinstance()->getServer("http", servers)) {
        SYLAR_LOG_INFO(g_logger) << "no httpServer alive";
        return false;
    }
    // 放置servlet
    for(auto& i : servers) {
        auto http_server = std::dynamic_pointer_cast<sylar::http::HttpServer>(i);
        if(!http_server) {
            continue;
        }
        auto sltDisp = http_server->getDispatch();

        // 读html界面的servlet
        sylar::http::ResourceServlet::ptr slt = 
            std::make_shared<sylar::http::ResourceServlet>(sylar::EnvMgr::GetInstance()->getCwd());
        // sltDisp->addGlobServlet("/html/*", handle);
        sltDisp->addGlobServlet("/html/*", slt);
        sltDisp->addGlobServlet("/css/*", slt);
        sltDisp->addGlobServlet("/image/*", slt);
        sltDisp->addGlobServlet("/js/*", slt);
        sltDisp->addGlobServlet("/files/*", slt);

#define XX(name) sylar::http::HttpServlet::ptr(new radar::servlet::name)
        sltDisp->addServlet("/user/create", XX(UserCreateServlet));
        sltDisp->addServlet("/user/active", XX(UserActiveServlet));
        sltDisp->addServlet("/user/login", XX(UserLoginServlet));
        sltDisp->addServlet("/user/logout", XX(UserLogoutServlet));
        sltDisp->addServlet("/user/modify", XX(UserModifyServlet));
        sltDisp->addServlet("/user/forgetPasswd", XX(UserForgetPasswdServlet));
        sltDisp->addServlet("/user/changePasswd", XX(UserChangePasswdServlet));

        /// 调试用，清空数据库，删除所有账号，实测要关掉
        sltDisp->addServlet("/user/clear", XX(UserClearServlet));
#undef XX

        SYLAR_LOG_INFO(g_logger) << "addServlet";
    }

    // 获取ws服务器
    servers.clear();
    if(!sylar::Application::Getinstance()->getServer("ws", servers)) {
        SYLAR_LOG_INFO(g_logger) << "no ws alive";
        return false;
    }
    // 放置servlet
    for(auto& i : servers) {
        auto ws_server = std::dynamic_pointer_cast<sylar::http::WSServer>(i);
        if(!ws_server) {
            continue;
        }
        sylar::http::ServletDispatch::ptr sltDisp = ws_server->getDispatch();

        RadarDataWSServlet::ptr slt = std::make_shared<RadarDataWSServlet>();
        sltDisp->addServlet("/radar/data", slt);
    }

    return true;
}

bool MyModule::onServerUp() {
    SYLAR_LOG_INFO(g_logger) << "onServerUp";
    return true;
}

extern "C" {
    sylar::Module* CreateModule() {
        sylar::Module* m = new radar::MyModule();
        SYLAR_LOG_INFO(g_logger) << "CreateModule:" << m;
        return m;
    }

    void DestoryModule(sylar::Module* m) {
        SYLAR_LOG_INFO(g_logger) << "DestoryModule:" << m;
        delete m;
    }
}

}