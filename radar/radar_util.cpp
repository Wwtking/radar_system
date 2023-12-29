#include "radar_util.h"
#include "config.h"
#include <boost/regex.hpp>
#include <string>

namespace radar {

static sylar::ConfigVar<int32_t>::ptr g_db_type =
    sylar::Config::Lookup("db.type", (int32_t)1, "db type 1:sqlite3, 2:mysql");

// 检查字符串是否符合电子邮件地址的格式
bool is_email(const std::string& str) {
    // [0-9A-Za-z\\-_\\.]+ 表示匹配一个或多个数字、字母、连字符-、下划线_或者句点.的字符串
    // \\.[a-z]{2,8} 表示匹配一个句点后面跟着 2 到 8 个小写字母的字符串，
    static const boost::regex s_email_regex("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,8}(\\.[a-z]{2,8})?)");
    return boost::regex_match(str, s_email_regex);
}

// 检查字符串是否符合正常用户名的格式
bool is_valid_account(const std::string& str) {
    // 匹配 4 到 15 个数字、字母、连字符-、下划线_或者句点.的字符串
    static const boost::regex s_account_regex("[0-9A-Za-z\\-_\\.]{4,15}");
    return boost::regex_match(str, s_account_regex);
}

sylar::IDB::ptr GetDB() {
    if(g_db_type->getValue() == 1) {
        return sylar::SQLite3Mgr::GetInstance()->get("radar");
    }
    else {
        // 扩展mysql，暂时未写
        return nullptr;
    }
}

std::string get_max_length_string(const std::string& str, size_t len) {
    auto wstr = sylar::StringUtil::StringToWString(str);
    wstr.resize(len);
    return sylar::StringUtil::WStringToString(wstr);
}
    
} // namespace radar
