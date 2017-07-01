#include <vector>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <thread>
#include <mutex>


//!
//! The task manager allows to easily create a pool thread and send jobs to
//! it. Each job will be executed on an independent thread, in a FIFO fashion.
//!
class TaskManager
{

public:

	//!
	//! Defines a simple task
	//!
	typedef std::function< void(void) > Task;

	//!
	//! Constructor.
	//!
	//! @param threadCount
	//!		Optional number of thread used by the task manager. If omitted, the number
	//!		of thread created will be the one supported by the platform.
	//!
	TaskManager(int threadCount = -1)
		: m_Stop(false)
	{
		// get the number of threads supported by the platform if no threadCount
		// was provided.
		if (threadCount == -1)
		{
			threadCount = std::thread::hardware_concurrency();
		}

		// create the threads
		for (int i = 0; i < threadCount; ++i)
		{
			m_Threads.push_back(new std::thread([&](void) {
				// the loop should be going on until the task manager wants to stop, and we
				// no longer have tasks to process.
				while (m_Stop == false || this->HasTask() == true)
				{
					// prepare a task
					Task task;

					// lock the queue's mutex to check if there is a job
					m_QueueMutex.lock();

					// check if we have jobs
					if (m_Queue.empty())
					{
						// No job. Allow new ones to be queued
						m_QueueMutex.unlock();

						// and wait for something to do
						std::unique_lock< std::mutex > uniqueLock(m_ConditionVariableMutex);
						m_ConditionVariable.wait(uniqueLock);

						// once activated, we want to go back to the beginning of the loop
						// to lock again the queue's mutex and check the queue, etc.
						continue;
					}
					else
					{
						// we've got a job ! Get it, pop it, and release the queue's lock.
						task = m_Queue.front();
						m_Queue.pop();
						m_QueueMutex.unlock();
					}

					// execute the task
					task();
				}
			}));
		}
	}

	//!
	//! Destructor. Cleanup the threads. This will stall the current
	//! thread until all jobs are done processing and all threads have
	//! been correctly stopped and cleaned up.
	//!
	~TaskManager(void)
	{
		// notify the threads that they should stop and wake the sleeping ones.
		m_Stop = true;
		m_ConditionVariable.notify_all();

		// join and delete
		for (std::thread * thread : m_Threads)
		{
			thread->join();
			delete thread;
		}
	}

	//!
	//! Checks if there is a task queued
	//!
	inline bool HasTask(void)
	{
		std::lock_guard< std::mutex > lock(m_QueueMutex);
		return m_Queue.empty() == false;
	}

	//!
	//! Push a new task
	//!
	inline void PushTask(const Task & task)
	{
		if (m_Threads.empty() == true)
		{
			// no jobs, just execute the task
			task();
		}
		else
		{
			// push the job
			{
				std::lock_guard< std::mutex > lock(m_QueueMutex);
				m_Queue.push(task);
			}

			// and notify one thread that we have a job to do
			m_ConditionVariable.notify_one();
		}
	}

private:

	//! The list of threads
	std::vector< std::thread * > m_Threads;

	//! The jobs
	std::queue< Task > m_Queue;

	//! The mutex used to protect the job queue
	std::mutex m_QueueMutex;

	//! Condition variable used to awake the threads when a job is available
	std::condition_variable m_ConditionVariable;

	//! Mutex used with the condition variable
	std::mutex m_ConditionVariableMutex;

	//! Used to stop the threads' loop before joining them
	std::atomic_bool m_Stop;

};
