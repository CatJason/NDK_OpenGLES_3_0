#ifndef _BYTEFLOW_NATIVE_LOOPER_H_
#define _BYTEFLOW_NATIVE_LOOPER_H_

#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>

// Looper 消息结构体定义
struct LooperMessage {
    int what;              // 消息类型
    int arg1;              // 参数1
    int arg2;              // 参数2
    void *obj;             // 附带的对象指针
    LooperMessage *next;   // 指向下一个消息
    bool quit;             // 是否为退出消息
};

// Looper 类，用于管理消息队列和事件循环
class Looper {

public:
    /**
     * 构造函数，初始化 Looper
     */
    Looper();

    // 禁用拷贝构造函数和赋值运算符
    Looper&operator=(const Looper&) = delete;
    Looper(Looper&) = delete;

    /**
     * 析构函数，清理资源
     */
    virtual ~Looper();

    /**
     * 发送简单消息
     * @param what 消息类型
     * @param flush 是否清空队列
     */
    void postMessage(int what, bool flush = false);

    /**
     * 发送带对象的消息
     * @param what 消息类型
     * @param obj 附带对象
     * @param flush 是否清空队列
     */
    void postMessage(int what, void *obj, bool flush = false);

    /**
     * 发送带参数的消息
     * @param what 消息类型
     * @param arg1 参数1
     * @param arg2 参数2
     * @param flush 是否清空队列
     */
    void postMessage(int what, int arg1, int arg2, bool flush = false);

    /**
     * 发送完整消息
     * @param what 消息类型
     * @param arg1 参数1
     * @param arg2 参数2
     * @param obj 附带对象
     * @param flush 是否清空队列
     */
    void postMessage(int what, int arg1, int arg2, void *obj, bool flush = false);

    /**
     * 停止事件循环并退出
     */
    void quit();

    /**
     * 消息处理函数，子类可重写此函数实现自定义逻辑
     * @param msg 消息指针
     */
    virtual void handleMessage(LooperMessage *msg);

private:
    /**
     * 添加消息到队列
     * @param msg 消息指针
     * @param flush 是否清空队列
     */
    void addMessage(LooperMessage *msg, bool flush);

    /**
     * 线程入口函数，启动事件循环
     * @param p Looper 实例指针
     */
    static void *trampoline(void *p);

    /**
     * 事件循环
     */
    void loop();

    LooperMessage *head;       // 消息队列头指针
    pthread_t worker;          // 工作线程
    sem_t headWriteProtect;    // 保护队列写入的信号量
    sem_t headDataAvailable;   // 消息可用的信号量
    bool running;              // 标志是否在运行
};

#endif //_BYTEFLOW_NATIVE_LOOPER_H_
