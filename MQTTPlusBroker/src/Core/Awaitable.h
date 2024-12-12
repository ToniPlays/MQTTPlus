#pragma once

#include <coroutine>
#include <condition_variable>

#include "Core/Coroutine.h"
#include "Core/Callback.h"

template<typename T>
class Awaitable
{
public:
    
    virtual void OnSuspend() {};
    virtual T GetResults() { return T(); }
    
    T await_resume() noexcept { return GetResults(); }
    bool await_ready() { return false; }
    void await_suspend(Coroutine::handle_type handle)
    {
        m_OnResolved = [handle]() {
            if(--handle.promise().m_Dependencies)
                handle.resume();
        };
        
        OnSuspend();
    }
    
    void Resolve()
    {
        m_OnResolved();
    }
    
private:
    std::function<void()> m_OnResolved;
};