#ifndef __RADAR_UTIL_H__
#define __RADAR_UTIL_H__

#include "mysqlite3.h"

namespace radar {

#define DEFINE_AND_CHECK_STRING(result, var, param) \
    std::string var = request->getParam(param); \
    if(var.empty()) { \
        result->setResult(400, "param " param " is null"); \
        break; \
    }

// 检查字符串是否符合电子邮件地址的格式
bool is_email(const std::string& str);

// 检查字符串是否符合正常用户名的格式
bool is_valid_account(const std::string& str);

// 获取数据库
sylar::IDB::ptr GetDB();

std::string get_max_length_string(const std::string& str, size_t len);
    
} // namespace radar


#endif