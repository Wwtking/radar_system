#ifndef __RADAR_STRUCT_H__
#define __RADAR_STRUCT_H__

#include <string>
#include <map>
#include "util.h"
#include "http_servlet.h"
#include "session_data.h"

namespace radar {

/**
 * @brief 用户账号状态
*/
enum class State {
    VERIFYING = 1,      // 验证状态(未激活)
    PUBLISH   = 2,      // 注册成功(激活)
    NOT_PASS  = 3,
    UNPUBLISH = 4
};

/**
 * @brief cookie键封装
*/
struct CookieKey {
    /// 会话键，用于存储用户的会话标识，Session ID
    static const std::string SESSION_KEY;
    /// 用户ID键，用于存储用户的唯一标识
    static const std::string USER_ID;
    /// 令牌键，用于存储用于身份验证或授权的令牌
    static const std::string TOKEN;
    /// 令牌时间键，用于存储令牌的生成时间或有效期限
    static const std::string TOKEN_TIME;
    /// 身份验证状态键，用于存储用户的身份验证状态
    static const std::string IS_AUTH;

    /// 评论最后时间键，用于存储用户最后一次评论的时间
    static const std::string COMMENT_LAST_TIME;
    /// 文章最后时间键，用于存储用户最后一次发布文章的时间
    static const std::string ARTICLE_LAST_TIME;
    /// 邮件最后时间键，用于存储用户最后一次发送邮件的时间
    static const std::string EMAIL_LAST_TIME;
};

/**
 * @brief 保存用户与服务器的交互结果
*/
struct Result {
    typedef std::shared_ptr<Result> ptr;

    Result(int32_t c = 200, const std::string& m = "ok");

    int32_t code;           // 状态码
    int64_t used;           // 所用时间
    std::string msg;        // 原因
    Json::Value jsondata;

    template<class T>
    void set(const std::string& key, const T& v) {
        //datas[key] = std::to_string(v);
        jsondata[key] = v;
    }

    void set(const std::string& key, const char* v) {
        jsondata[key] = v;
    }

    void set(const std::string& key, const std::string& v) {
        jsondata[key] = v;
    }

    template<class T>
    void append(const std::string& key, const T& v) {
        jsondata[key].append(v);
    }

    // 设置 code 和 msg
    void setResult(int32_t c, const std::string& m);

    std::string toJsonString() const;
};


class BlogServlet : public sylar::http::HttpServlet {
public:
    /**
     * @brief 构造函数
    */
    BlogServlet(const std::string& name);

    /**
     * @brief 处理请求
     * @param[in] req HTTP请求
     * @param[in] res HTTP响应
     * @param[in] session HTTP连接
     * @return 是否处理成功
     */
    int32_t handle(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session) override;

    /**
     * @brief 根据HTTP请求获取用户id(指主键id)
     * @param[in] request HTTP请求
    */
    int64_t getUserId(sylar::http::HttpRequest::ptr request);

    /**
     * @brief 根据HTTP请求获取Session ID
     * @param[in] request HTTP请求
    */
    std::string getCookieId(sylar::http::HttpRequest::ptr request);

protected:
    /**
     * @brief 处理请求前要做的事
     * @param[in] result 保存处理结果
    */
    virtual bool handlePre(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result);

    /**
     * @brief 处理请求
     * @param[in] result 保存处理结果
    */
    virtual int32_t handle(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result) = 0;

    /**
     * @brief 处理请求后要做的事
     * @param[in] result 保存处理结果
    */                 
    virtual bool handlePost(sylar::http::HttpRequest::ptr request
                           ,sylar::http::HttpResponse::ptr response
                           ,sylar::http::HttpSession::ptr session
                           ,Result::ptr result);

protected:
    /**
     * @brief 获取Session
    */
    sylar::http::SessionData::ptr getSessionData(sylar::http::HttpRequest::ptr request
                                                ,sylar::http::HttpResponse::ptr response);
    
    /**
     * @brief 初始化Cookie信息
    */
    bool initLogin(sylar::http::HttpRequest::ptr request
                   ,sylar::http::HttpResponse::ptr response
                   ,sylar::http::HttpSession::ptr session);

};

}

#endif
