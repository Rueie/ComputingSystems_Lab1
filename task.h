#include "global_include.h"

class Task{
public:
    int workers,days;
    string name;
    vector<string> namePrevTasks;
    Task* selfPointer = nullptr;
    vector<Task*> prevTasks {};

    Task(string _name);
    ~Task();
    void setWorkers(int newWorkers);
    int getWorkers();
    void setDays(int newDays);
    int getDays();
    string getName();
    void addNamePrevTask(string nameNewPrevTask);
    vector<string> getListNamePrevTasks();
    string getNamePrevTask(int taskNumnber);
    string getInfo();
    void addPrevTask(Task *newPrevTask);
    vector<Task*> getListPrevTasks();
    Task* getPrevTask(int taskNumber);
    bool subDay(int workers);
    void setSelfPointer(Task* self);
    Task* getSelfPointer();
};