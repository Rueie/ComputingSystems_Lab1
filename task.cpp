#include "task.h"

Task::Task(string _name){
    name = _name;
    namePrevTasks = {};
    prevTasks = {};
}

Task::~Task(){};

void Task::setWorkers(int newWorkers){
    this->workers = newWorkers;
}

int Task::getWorkers(){
    return workers;
}
   
void Task::setDays(int newDays){
    this->days = newDays;
}

int Task::getDays(){
    return days;
}

string Task::getName(){
    return name;
}

void Task::addNamePrevTask(string nameNewPrevTask){
    namePrevTasks.push_back(nameNewPrevTask);
}

string Task::getInfo(){
    string info = "Name: " + name + "\n\tDays: " + to_string(days) + "\n\tWorkers: " + to_string(workers) + "Previous tasks: ";
    for (int i = 0; i < namePrevTasks.size(); i++)
        info += "\n\t\t" + namePrevTasks[i];
    return info;
}

vector<string> Task::getListNamePrevTasks(){
    return namePrevTasks;
}

string Task::getNamePrevTask(int taskNumnber){
    if (taskNumnber > -1 && taskNumnber < namePrevTasks.size())
        return namePrevTasks[taskNumnber];
    return "";  
}

void Task::addPrevTask(Task *newPrevTask){
    prevTasks.push_back(newPrevTask);
}

vector<Task*> Task::getListPrevTasks(){
    return prevTasks;
}

Task* Task::getPrevTask(int taskNumber){
    if (taskNumber > -1 && taskNumber < prevTasks.size())
        return prevTasks[taskNumber];
    return nullptr;
}

bool Task::subDay(int workers){
    if (days > 0 && this -> workers <= workers){
        days--;
        return true;
    }
    return false;
}

void Task::setSelfPointer(Task* self){
    this->selfPointer = self;
}

Task* Task::getSelfPointer(){
    return this->selfPointer;
}