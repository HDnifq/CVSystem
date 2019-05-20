#include "ThreadPool.h"

namespace dxlib {

ThreadPool* ThreadPool::m_pInstance = new ThreadPool();

ThreadPool::ThreadPool()
{
}

ThreadPool::~ThreadPool()
{
}

} // namespace dxlib