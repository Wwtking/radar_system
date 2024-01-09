#include "radar_data_message.h"

namespace radar {
    
RadarDataMessage::ptr RadarDataMessage::Create(const std::string& json_str) {
    Json::Value json;
    if(!sylar::JsonUtil::FromString(json, json_str)) {
        return nullptr;
    }

    RadarDataMessage::ptr cmsg = std::make_shared<RadarDataMessage>();
    // 返回std::vector<std::string>，存放着所有key的字符串
    Json::Value::Members keys = json.getMemberNames();
    for(auto& i : keys) {
        cmsg->m_datas[i] = json[i].asString();  // 转化为字符串形式
    }
    return cmsg;
}

RadarDataMessage::RadarDataMessage() {
}

void RadarDataMessage::set(const std::string& key, const std::string& value) {
    m_datas[key] = value;
}

std::string RadarDataMessage::get(const std::string& key) {
    auto it = m_datas.find(key);
    if(it == m_datas.end()) {
        return "";
    }
    return it->second;
}

std::string RadarDataMessage::toString() const {
    Json::Value json;
    for(auto& i : m_datas) {
        json[i.first] = i.second;
    }
    return sylar::JsonUtil::ToString(json);
}

} // namespace radar
