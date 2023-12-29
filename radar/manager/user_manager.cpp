#include "user_manager.h"
#include "radar_util.h"
#include "log.h"

namespace user {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

// 将数据库所有用户数据加载到本类中，用类成员变量存放用户数据
bool UserManager::loadAll() {
    // 获取之前创建的数据库
    auto db = radar::GetDB();
    if(!db) {
        SYLAR_LOG_ERROR(g_logger) << "Get SQLite3 connection fail";
        return false;
    }

    // 查询数据库所有用户数据存入results
    std::vector<user::data::UserInfo::ptr> results;
    if(user::data::UserInfoDao::QueryAll(results, db)) {
        SYLAR_LOG_ERROR(g_logger) << "UserManager loadAll fail";
        return false;
    }

    std::unordered_map<int64_t, user::data::UserInfo::ptr> datas; 
    std::unordered_map<std::string, user::data::UserInfo::ptr> accounts;
    std::unordered_map<std::string, user::data::UserInfo::ptr> emails;
    std::unordered_map<std::string, user::data::UserInfo::ptr> names;

    for(auto& i : results) {
        datas[i->getId()] = i;
        accounts[i->getAccount()] = i;
        emails[i->getEmail()] = i;
        names[i->getName()] = i;
    }

    MutexType::WriteLock lock(m_mutex);
    m_datas.swap(datas);
    m_accounts.swap(accounts);
    m_emails.swap(emails);
    m_names.swap(names);
    
    return true;
}

// 添加用户数据
void UserManager::add(user::data::UserInfo::ptr info) {
    MutexType::WriteLock lock(m_mutex);
    m_datas[info->getId()] = info;
    m_accounts[info->getAccount()] = info;
    m_emails[info->getEmail()] = info;
    m_names[info->getName()] = info;
}


#define XX(map, key) \
    MutexType::WriteLock lock(m_mutex); \
    auto it = map.find(key); \
    if(it != map.end()) { \
        map.erase(it); \
    }

// 根据id删除用户数据
void UserManager::del(int64_t id) {
    XX(m_datas, id);
}

// 根据account删除用户数据
void UserManager::delByAccount(std::string v) {
    XX(m_accounts, v);
}

// 根据email删除用户数据
void UserManager::delByEmail(std::string v) {
    XX(m_emails, v);
}

// 根据name删除用户数据
void UserManager::delByName(std::string v) {
    XX(m_names, v);
}

#undef XX


#define XX(map, key) \
    MutexType::ReadLock lock(m_mutex); \
    auto it = map.find(key); \
    return it == map.end() ? nullptr : it->second;

// 根据id获取用户数据
user::data::UserInfo::ptr UserManager::get(int64_t id) {
    XX(m_datas, id);
}

// 根据account获取用户数据
user::data::UserInfo::ptr UserManager::getByAccount(std::string v) {
    XX(m_accounts, v);
}

// 根据email获取用户数据
user::data::UserInfo::ptr UserManager::getByEmail(std::string v) {
    XX(m_emails, v);
}

// 根据name获取用户数据
user::data::UserInfo::ptr UserManager::getByName(std::string v) {
    XX(m_names, v);
}

#undef XX

std::string UserManager::GetToken(data::UserInfo::ptr info, int64_t ts) {
    std::stringstream ss;
    ss << info->getId()
       << "|" << info->getAccount()
       << "|" << info->getEmail()
       << "|" << info->getPasswd()
       << "|" << ts;
    return sylar::md5(ss.str());
}

std::string UserManager::statusString() {
    std::stringstream ss;
    MutexType::ReadLock lock(m_mutex);
    ss << "UserManager total=" << m_datas.size() << std::endl;
    lock.unlock();
    return ss.str();
}

}