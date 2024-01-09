#ifndef __SYLAR_RADAR_DATA_MESSAGE_H__
#define __SYLAR_RADAR_DATA_MESSAGE_H__

#include <memory>
#include "json_util.h"

namespace radar {

class RadarDataMessage {
public:
    typedef std::shared_ptr<RadarDataMessage> ptr;

    /**
     * @brief 根据json格式字符串创建RadarDataMessage类对象
     * @details 将json格式字符串转化为Json::Value，再放入类对象的m_datas中
     * @param[in] json_str json格式字符串
     * @return 返回创建的类对象
    */
    static RadarDataMessage::ptr Create(const std::string& json_str);

    /**
     * @brief 构造函数
    */
    RadarDataMessage();

    /**
     * @brief 存放数据
    */
    void set(const std::string& key, const std::string& value);

    /**
     * @brief 获取数据
    */
    std::string get(const std::string& key);

    /**
     * @brief 字符串可读性输出
    */
    std::string toString() const;

private:
    /// 存放 key-value 数据对
    std::map<std::string, std::string> m_datas;

};

}

#endif