
// http://www.brucesutherland.co.uk/android-ndk/the-runtime-loop-android-ndk-game-programming/


class Task
{
private:
    unsigned int m_priority;
    bool m_canKill;
   
public:
    explicit Task(unsigned int priority) { m_priority = priority; m_canKill = false; }
    virtual ~Task() {}
   
    virtual bool Start()  = 0;
    virtual void OnSuspend() = 0;
    virtual void Update() = 0;
    virtual void OnResume() = 0;
    virtual void Stop()  = 0;

    void SetCanKill(bool canKill) { m_canKill = canKill; }
    bool CanKill() { return m_canKill; }
    unsigned int Priority() { return m_priority; }
};


class Kernel
{
private:
    typedef std::list<std::unique_ptr<Task>> TaskList;
    typedef std::list<std::unique_ptr<Task>>::iterator TaskListIterator;

    TaskList m_tasks;
    TaskList m_pausedTasks;

public:
    Kernel();
    virtual ~Kernel();

    void Execute();

    bool AddTask(std::unique_ptr<Task> pTask);
    void SuspendTask(std::unique_ptr<Task> pTask);
    void ResumeTask(std::unique_ptr<Task> pTask);
    void RemoveTask(std::unique_ptr<Task> pTask);
    void KillAllTasks();
};


