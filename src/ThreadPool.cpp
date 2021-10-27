//
// Created by jiabh on 2021/10/27.
//

/**
 * 参考链接：
 * [C++11实现线程池](https://zhuanlan.zhihu.com/p/64739638)
 * [C++11多线程 unique_lock详解](https://blog.csdn.net/u012507022/article/details/85909567)
 * [有了互斥锁，为什么还要条件变量？](https://www.jianshu.com/p/01ad36b91d39)
 * [C++11（六） 条件变量（condition_variable）](https://zhuanlan.zhihu.com/p/224054283)
 * [C++11 中的std::function和std::bind](https://www.jianshu.com/p/f191e88dcc80)
 */

#include <thread>
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t th_cnt) : _th_cnt(th_cnt), _is_running(false)
{
	_threads = new std::thread[th_cnt];
}

ThreadPool::~ThreadPool()
{
	if (_is_running)
		stop(); // 先终止所有进程

	delete[] _threads;
}

void ThreadPool::start()
{
	_is_running = true;
	for (size_t i = 0; i < _th_cnt; i++) // 创建所有进程
		_threads[i] = std::thread(&ThreadPool::work, this);
}

void ThreadPool::stop()
{
	_is_running = false;
	_cond.notify_all(); // 唤醒所有挂起进程，否则在回收时可能卡住

	for (size_t i = 0; i < _th_cnt; i++) // 回收所有线程
	{
		std::thread &t = _threads[i];
		if (t.joinable())
			t.join();
	}
}

void ThreadPool::add_task(const ThreadPool::Task &task)
{
	{
		std::lock_guard<std::mutex> lg(_mtx);
//		printf("Task left: %lu\n", _tasks.size()); // DEBUG
		_tasks.push(task);

		if (_is_running)
			_cond.notify_one(); // wake a thread to do the task
	}
}

void ThreadPool::work()
{
	/**
	 * 每个work不断地从任务队列中取任务，然后执行
	 * 无任务时将挂起，等待条件变量_cond的唤醒
	 */
	while (_is_running)
	{
		Task task;
		{
			std::unique_lock<std::mutex> lk(_mtx); // 操作任务队列，要先加锁
			if (!_tasks.empty())
			{
				// if tasks not empty,
				// must finish the task whether thread pool is running or not
				task = _tasks.front();
				_tasks.pop();
//				std::this_thread::sleep_for(std::chrono::seconds(5)); // DEBUG
			} else if (_is_running) // 无任务，挂起
				_cond.wait(lk);
		}
		if (task)
			task(); // do the task
	}
}
