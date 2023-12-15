//
// Created by byte_flow on 2020/3/10.
//

#ifndef NDK_OPENGLES_3_0_BYTEFLOWLOCK_H
#define NDK_OPENGLES_3_0_BYTEFLOWLOCK_H

#include <pthread.h>

/**
 * MySyncLock 类：封装 pthread 互斥锁，提供递归锁功能。
 */
class MySyncLock {
public:
    /**
     * 构造函数：初始化互斥锁属性，并创建递归互斥锁。
     */
    MySyncLock() {
        pthread_mutexattr_init(&m_attr); // 初始化互斥锁属性
        pthread_mutexattr_settype(&m_attr, PTHREAD_MUTEX_RECURSIVE); // 设置为递归锁
        pthread_mutex_init(&m_mutex, &m_attr); // 初始化互斥锁
    }

    /**
     * 析构函数：销毁互斥锁及其属性。
     */
    ~MySyncLock(void) {
        pthread_mutex_destroy(&m_mutex); // 销毁互斥锁
        pthread_mutexattr_destroy(&m_attr); // 销毁互斥锁属性
    }

    /**
     * 加锁操作
     * @return 返回 pthread_mutex_lock 的执行结果
     */
    int Lock() {
        return pthread_mutex_lock(&m_mutex);
    }

    /**
     * 解锁操作
     * @return 返回 pthread_mutex_unlock 的执行结果
     */
    int UnLock() {
        return pthread_mutex_unlock(&m_mutex);
    }

    /**
     * 尝试加锁操作
     * @return 返回 pthread_mutex_trylock 的执行结果
     */
    int TryLock() {
        return pthread_mutex_trylock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex;           // 互斥锁
    pthread_mutexattr_t m_attr;        // 互斥锁属性
};

/**
 * ScopedSyncLock 类：作用域锁，自动管理锁的加锁和解锁。
 */
class ScopedSyncLock {
public:
    /**
     * 构造函数：在指定锁上执行加锁操作。
     * @param pLock 指向 MySyncLock 的指针
     */
    ScopedSyncLock(MySyncLock *pLock) :
            m_pLock(pLock) {
        if (pLock != NULL)
            m_pLock->Lock();
    }

    /**
     * 析构函数：在指定锁上执行解锁操作。
     */
    ~ScopedSyncLock() {
        if (m_pLock != NULL)
            m_pLock->UnLock();
    }

private:
    MySyncLock *m_pLock; // 关联的锁对象
};

#endif //NDK_OPENGLES_3_0_BYTEFLOWLOCK_H