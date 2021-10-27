//
// Created by jiabh on 2021/10/27.
//

#ifndef HTTP_PROXY_SERVER_THREADPOOL_H
#define HTTP_PROXY_SERVER_THREADPOOL_H

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <queue>

class ThreadPool
{
public:
	using Task = std::function<void()>;

	/**
	 * 创建一个线程池
	 *
	 * @param th_cnt 线程个数
	 */
	explicit ThreadPool(size_t th_cnt);

	~ThreadPool();

	/**
	 * 启动线程池
	 */
	void start();

	/**
	 * 停止线程池
	 */
	void stop();

	/**
	 * 向线程池中添加一个任务
	 *
	 * @param task 被调用的任务
	 */
	void add_task(const Task &task);

private:
	void work(); // 包装任务以供线程运行的函数

	size_t _th_cnt; // 线程个数
	std::atomic_bool _is_running; // 线程池是否正在运行，标记线程池的状态
	std::mutex _mtx; // 改变线程池状态时要锁定的互斥量
	std::condition_variable _cond;
	std::thread *_threads; // 所有的线程
	std::queue<Task> _tasks; // 所有的任务
};


#endif //HTTP_PROXY_SERVER_THREADPOOL_H
