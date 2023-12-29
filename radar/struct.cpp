#include "struct.h"

namespace radar {

const std::string CookieKey::SESSION_KEY = "SSESSIONID";
const std::string CookieKey::USER_ID = "S_UID";
const std::string CookieKey::TOKEN = "S_TOKEN";
const std::string CookieKey::TOKEN_TIME = "S_TOKEN_TIME";
const std::string CookieKey::IS_AUTH= "IS_AUTH";
const std::string CookieKey::COMMENT_LAST_TIME = "COMMENT_LAST_TIME";
const std::string CookieKey::ARTICLE_LAST_TIME = "ARTICLE_LAST_TIME";
const std::string CookieKey::EMAIL_LAST_TIME = "EMAIL_LAST_TIME";


Result::Result(int32_t c, const std::string& m)
    :code(c)
    ,used(sylar::GetCurrentUS())
    ,msg(m) {
}

std::string Result::toJsonString() const {
    Json::Value v;
    v["code"] = std::to_string(code);
    v["msg"] = msg;
    v["used"] = ((sylar::GetCurrentUS() - used) / 1000.0);
    if(!jsondata.isNull()) {
        v["data"] = jsondata;
    } else {
        //if(!datas.empty()) {
        //    auto& d = v["data"];
        //    for(auto& i : datas) {
        //        d[i.first] = i.second;
        //    }
        //}
    }
    return sylar::JsonUtil::ToString(v);
}

void Result::setResult(int32_t c, const std::string& m) {
    code = c;
    msg = m;
}

/**
 * @brief 构造函数
*/
BlogServlet::BlogServlet(const std::string& name) 
    :HttpServlet("BlogServlet") {
}

/**
 * @brief 处理请求
 * @param[in] req HTTP请求
 * @param[in] res HTTP响应
 * @param[in] session HTTP连接
 * @return 是否处理成功
 */
int32_t BlogServlet::handle(sylar::http::HttpRequest::ptr request
                        ,sylar::http::HttpResponse::ptr response
                        ,sylar::http::HttpSession::ptr session) {
    uint64_t ts = sylar::GetCurrentUS();
    Result::ptr result = std::make_shared<Result>();
    
    // 表示允许来自任何域的请求访问该资源
    response->setHeader("Access-Control-Allow-Origin", "*");
    // 表示允许发送身份验证凭证（如cookies、HTTP认证等）的请求访问该资源
    response->setHeader("Access-Control-Allow-Credentials", "true");

    if(handlePre(request, response, session, result)) {
        handle(request, response, session, result);
    } else {
        response->setBody(result->toJsonString());
    }
    handlePost(request, response, session, result);

    uint64_t used = sylar::GetCurrentUS() - ts;
    response->setHeader("used", std::to_string((used * 1.0 / 1000)) + "ms");
    return 0;
}

bool BlogServlet::handlePre(sylar::http::HttpRequest::ptr request
                        ,sylar::http::HttpResponse::ptr response
                        ,sylar::http::HttpSession::ptr session
                        ,Result::ptr result) {
    return true;
}

bool BlogServlet::handlePost(sylar::http::HttpRequest::ptr request
                        ,sylar::http::HttpResponse::ptr response
                        ,sylar::http::HttpSession::ptr session
                        ,Result::ptr result) {
    return true;
}

// 获取Session
sylar::http::SessionData::ptr BlogServlet::getSessionData(sylar::http::HttpRequest::ptr request
                                                    ,sylar::http::HttpResponse::ptr response) {
    // 客户端需要在 Cookie 里面记录 Session ID 来告诉服务器
    std::string sid = request->getCookie(CookieKey::SESSION_KEY);
    // HTTP请求的Cookie里有Session ID，则直接获取对应Session
    if(!sid.empty()) {
        auto data = sylar::http::SessionDataMgr::GetInstance()->get(sid);
        if(data) {
            return data;
        }
    }
    // request的Cookie里没有Session ID，说明该客户端还没有对应的Session
    // 则第一次创建Session，服务端会在HTTP协议中告诉客户端，需要在Cookie里面记录Session ID
    // 在以后该客户端每次请求时要把Session ID发送到服务器，服务器就可以识别
    sylar::http::SessionData::ptr data(new sylar::http::SessionData(true));
    sylar::http::SessionDataMgr::GetInstance()->add(data);
    response->setCookie(CookieKey::SESSION_KEY, data->getId(), 0, "/");
    request->setCookie(CookieKey::SESSION_KEY, data->getId());
    return data;
}

// 初始化Cookie信息
bool BlogServlet::initLogin(sylar::http::HttpRequest::ptr request
                        ,sylar::http::HttpResponse::ptr response
                        ,sylar::http::HttpSession::ptr session) {
    return true;
}

// 根据HTTP请求获取用户id(指主键id)
int64_t BlogServlet::getUserId(sylar::http::HttpRequest::ptr request) {
    std::string sid = request->getCookie(CookieKey::SESSION_KEY);
    if(!sid.empty()) {
        auto data = sylar::http::SessionDataMgr::GetInstance()->get(sid);
        if(data) {
            return data->getData<int64_t>(CookieKey::USER_ID);
        }
    }
    return 0;
}

// 根据HTTP请求获取Cookie Id
std::string BlogServlet::getCookieId(sylar::http::HttpRequest::ptr request) {
    return request->getCookie(CookieKey::SESSION_KEY);
}

}