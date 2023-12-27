#include "my_module.h"

namespace radar {

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("system");

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