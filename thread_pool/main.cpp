#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <functional>
#include <iostream>

class Task
{
public:
    void operator()() {
        m_func();
    }
    void setFunc(std::function<void()> func)
    {
        m_func = std::move(func);
    }
private:
    std::function<void()> m_func;
};

class ThreadPool
{
public:
    explicit ThreadPool(size_t num)
    {
        for (size_t i = 0; i < num; ++i)
        {
            m_threads.emplace_back(&ThreadPool::threadFunc, this);
        }
        m_terminate = false;
    }

    ~ThreadPool() {
        m_terminate = true;
        m_cv_condition.notify_all();
        for (std::thread& th : m_threads) {
            if (th.joinable()) {
                th.join();
            }
        }
    }

    void addTask(const Task& task)
    {
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            m_task_queue.push(task);
        }
        m_cv_condition.notify_one();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_finish_condition.wait(lock, [this]() -> bool { return m_task_queue.empty() && m_active_tasks_num == 0; });
    }

    void threadFunc()
    {
        while (1)
        {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_cv_condition.wait(lock, [this]() { return !m_task_queue.empty() || m_terminate; });
                if (m_task_queue.empty() && m_terminate)
                {
                    break;
                }
                task = m_task_queue.front();
                m_task_queue.pop();
            }
            m_active_tasks_num++;
            task();
            m_active_tasks_num--;
            m_finish_condition.notify_one();
        }
    }

private:
    std::vector<std::thread> m_threads;
    std::mutex m_queue_mutex;
    std::queue<Task> m_task_queue;
    std::condition_variable m_cv_condition;
    std::condition_variable m_finish_condition;
    std::atomic_bool m_terminate;
    std::atomic_uint m_active_tasks_num;
};

int main() {
    Task task;
    task.setFunc([](){ std::cout << "Hello, World!" << std::endl;});
    ThreadPool pool(3);
    for (int i = 0; i < 3; ++i)
    {
        pool.addTask(task);
        pool.wait();
    }
    return 0;
}
