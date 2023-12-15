//
// Created by ByteFlow on 2019/7/15.
//

#include "Looper.h"

#include <jni.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include "LogUtil.h"

// Looper 消息结构体
struct LooperMessage;
typedef struct LooperMessage LooperMessage;

/**
 * 静态函数，用于启动 Looper 的事件循环。
 * @param p Looper 实例指针
 * @return 返回值
 */
void* Looper::trampoline(void* p) {
    ((Looper*)p)->loop();
    return NULL;
}

/**
 * Looper 构造函数
 * 初始化信号量、创建线程并启动事件循环。
 */
Looper::Looper() {
    head = NULL;

    // 初始化信号量
    sem_init(&headDataAvailable, 0, 0);
    sem_init(&headWriteProtect, 0, 1);
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    // 创建线程并启动事件循环
    pthread_create(&worker, &attr, trampoline, this);
    running = true;
}

/**
 * Looper 析构函数
 * 如果 Looper 正在运行，则调用 quit() 停止。
 */
Looper::~Looper() {
    if (running) {
        LOGCATE("Looper deleted while still running. Some messages will not be processed");
        quit();
    }
}

/**
 * 发送简单消息
 * @param what 消息类型
 * @param flush 是否清空队列
 */
void Looper::postMessage(int what, bool flush) {
    postMessage(what, 0, 0, NULL, flush);
}

/**
 * 发送带对象的消息
 * @param what 消息类型
 * @param obj 附带对象
 * @param flush 是否清空队列
 */
void Looper::postMessage(int what, void *obj, bool flush) {
    postMessage(what, 0, 0, obj, flush);
}

/**
 * 发送带参数的消息
 * @param what 消息类型
 * @param arg1 参数1
 * @param arg2 参数2
 * @param flush 是否清空队列
 */
void Looper::postMessage(int what, int arg1, int arg2, bool flush) {
    postMessage(what, arg1, arg2, NULL, flush);
}

/**
 * 发送完整消息
 * @param what 消息类型
 * @param arg1 参数1
 * @param arg2 参数2
 * @param obj 附带对象
 * @param flush 是否清空队列
 */
void Looper::postMessage(int what, int arg1, int arg2, void *obj, bool flush) {
    LooperMessage *msg = new LooperMessage();
    msg->what = what;
    msg->obj = obj;
    msg->arg1 = arg1;
    msg->arg2 = arg2;
    msg->next = NULL;
    msg->quit = false;
    addMessage(msg, flush);
}

/**
 * 添加消息到队列
 * @param msg 消息指针
 * @param flush 是否清空队列
 */
void Looper::addMessage(LooperMessage *msg, bool flush) {
    sem_wait(&headWriteProtect); // 获取写保护信号量
    LooperMessage *h = head;

    if (flush) {
        // 清空现有队列
        while (h) {
            LooperMessage *next = h->next;
            delete h;
            h = next;
        }
        h = NULL;
    }

    if (h) {
        // 将消息添加到队列末尾
        while (h->next) {
            h = h->next;
        }
        h->next = msg;
    } else {
        // 如果队列为空，设置头节点
        head = msg;
    }

    LOGCATE("Looper::addMessage msg->what=%d", msg->what);
    sem_post(&headWriteProtect); // 释放写保护信号量
    sem_post(&headDataAvailable); // 通知有新消息可用
}

/**
 * 事件循环
 */
void Looper::loop() {
    while (true) {
        // 等待消息可用
        sem_wait(&headDataAvailable);

        // 获取消息
        sem_wait(&headWriteProtect);
        LooperMessage *msg = head;
        if (msg == NULL) {
            LOGCATE("Looper::loop() no msg");
            sem_post(&headWriteProtect);
            continue;
        }
        head = msg->next;
        sem_post(&headWriteProtect);

        if (msg->quit) {
            // 处理退出消息
            LOGCATE("Looper::loop() quitting");
            delete msg;
            return;
        }

        LOGCATE("Looper::loop() processing msg.what=%d", msg->what);
        handleMessage(msg); // 处理消息
        delete msg;
    }
}

/**
 * 停止事件循环并销毁 Looper
 */
void Looper::quit() {
    LOGCATE("Looper::quit()");
    LooperMessage *msg = new LooperMessage();
    msg->what = 0;
    msg->obj = NULL;
    msg->next = NULL;
    msg->quit = true;
    addMessage(msg, false); // 添加退出消息
    void *retval;
    pthread_join(worker, &retval); // 等待线程退出
    sem_destroy(&headDataAvailable); // 销毁信号量
    sem_destroy(&headWriteProtect);
    running = false;
}

/**
 * 消息处理函数
 * @param msg 消息指针
 */
void Looper::handleMessage(LooperMessage *msg) {
    LOGCATE("Looper::handleMessage [what, obj]=[%d, %p]", msg->what, msg->obj);
}
