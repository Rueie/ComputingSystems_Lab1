#include "global_include.h"
#include "task.h"
#include "json.hpp"

using json = nlohmann::json;

void printTasks(vector<Task> tasks){
    for (int i = 0; i < tasks.size(); i++)
        cout << tasks[i].getInfo() << endl;
}

void printSequence(vector<vector<Task*>> sequence){
    for (int i = 0; i < sequence.size(); i++){
        cout << "step["<<i<<"]:\n";
        for (int j = 0; j < sequence[i].size(); j++)
            cout<<sequence[i][j]->getName()<<"/"<<sequence[i][j]->getSelfPointer()<<endl;
    }
}

vector<Task> setTasks(vector<Task> tasks){
    for (int i = 0; i < tasks.size(); i++)
        tasks[i].setSelfPointer(&tasks[i]);
    for (int i = 0; i < tasks.size(); i++){
        for (int j = 0; j < tasks[i].getListNamePrevTasks().size(); j++){
            for (int k = 0; k < tasks.size(); k++)
                if (tasks[k].getName() == tasks[i].getNamePrevTask(j))
                    tasks[i].addPrevTask(tasks[k].getSelfPointer());
        }
    }
    return tasks;
}

vector<Task> copyTask(vector<Task> tasks){
    vector<Task> newVector{};
    for (int i = 0; i < tasks.size(); i++){
        newVector.push_back(Task(tasks[i].getName()));
        newVector[i].setDays(tasks[i].getDays());
        newVector[i].setWorkers(tasks[i].getWorkers());
    }
    newVector= setTasks(newVector);
    return newVector;
}

vector<vector<Task*>> copyVectorPonters(vector<vector<Task*>> tasksPointer, vector<Task> &prevTasks, vector<Task> &newTasks){
    vector<vector<Task*>> newPointerVector{};
    vector<vector<Task*>> listOfPointersP{};
    for (int i = 0; i < prevTasks.size(); i++)
        listOfPointersP.push_back(vector<Task*>{&prevTasks[i],&newTasks[i]});
    for (int i = 0; i < tasksPointer.size(); i++){
        vector<Task*> newSubVector {};
        for (int j = 0; j < tasksPointer[i].size(); j++)
            newSubVector.push_back(tasksPointer[i][j]);
        for (int j = 0; j < newSubVector.size(); j++)
            for (int k = 0; k < listOfPointersP.size(); k++)
                if (newSubVector[j] == listOfPointersP[k][0])
                    newSubVector[j] = listOfPointersP[k][1];
        newPointerVector.push_back(newSubVector);
    }
    return newPointerVector;
}

vector<Task*> getAvailableTasks(vector<Task> &tasks, int workersNumber){
    vector<Task*> availableTasks{};
    for (int i = 0; i < tasks.size(); i++){
        if (tasks[i].getDays() > 0 && tasks[i].getWorkers() <= workersNumber){
            int counter = 0;
            while (counter != tasks[i].getListPrevTasks().size()){
                if (tasks[i].getListPrevTasks()[counter] -> getDays() > 0)
                    break;
                counter++;
            }
            if (counter == tasks[i].getListPrevTasks().size())
                availableTasks.push_back(tasks[i].getSelfPointer());
        }
    }
    return availableTasks;
}

vector<vector<Task*>> generateTasksSequnce(vector<Task> &tasks, int maxWorkersNumber){
    srand(rand()%10000);
    vector<vector<Task*>> result;
    int localWorkersNumber = maxWorkersNumber; 
    vector<Task*> availableTasks = getAvailableTasks(tasks, localWorkersNumber);
    while (availableTasks.size() > 0){
        vector<Task*> step {};
        while (availableTasks.size() > 0){
            int numberTask = rand() % availableTasks.size();
            bool flagExist = false;
            for (int i = 0; i < step.size(); i++){
                if (step[i] == availableTasks[numberTask]->getSelfPointer()){
                    flagExist = true;
                    break;
                }
                else{
                    for (int j = 0; j < availableTasks[numberTask]->getListPrevTasks().size(); j++)
                        if (availableTasks[numberTask]->getListPrevTasks()[j] == step[i]){
                            flagExist = true;
                            break;
                        }
                    if (flagExist == true)
                        break;
                }
            }
            if (flagExist == false){
                for (int i = 0; i < availableTasks[numberTask]->getListPrevTasks().size(); i++)
                    for (int j =0; j < step.size(); j++)
                        if (availableTasks[numberTask]->getListPrevTasks()[i] == step[j])
                            flagExist = true;
                if (flagExist == false){
                    step.push_back(availableTasks[numberTask]->getSelfPointer());
                    step[step.size() - 1]->subDay(localWorkersNumber);
                    localWorkersNumber -= step[step.size()-1]->getWorkers();
                    availableTasks = getAvailableTasks(tasks, localWorkersNumber);
                }
            }
            else{
                int counter = 0;
                for (int i = 0; i < availableTasks.size(); i++)
                    for (int j = 0; j < step.size(); j++){
                        if (availableTasks[i] == step[j]){
                            counter++;
                            break;
                        }
                        else{
                            flagExist = false;
                            for (int k = 0; k < availableTasks[i]->getListPrevTasks().size(); k++){
                                if (flagExist == true)
                                    break;
                                for (int l = 0; l < step.size(); l++)
                                    if (availableTasks[i]->getListPrevTasks()[k] == step[l]){
                                        counter++;
                                        flagExist = true;
                                        break;    
                                    }
                            }
                            if (flagExist == true)
                                break;
                        }
                    }
                if (counter == availableTasks.size())
                    break;
            }
        }
        localWorkersNumber = maxWorkersNumber;
        availableTasks = getAvailableTasks(tasks, localWorkersNumber);
        result.push_back(step);
    }
    return result;
}

int calculateUsability(vector<vector<Task*>> &tasks, int maxNumberWorkers){
    int usability = 0;
    usability += tasks.size()*100*maxNumberWorkers;
    for (int i = 0; i < tasks.size() ; i++){
        int unusedWorkers = maxNumberWorkers;
        for (int j = 0; j < tasks[i].size(); j++)
            unusedWorkers -= tasks[i][j]->getDays();
        usability += unusedWorkers;
    }
    return usability;
}

void findOptimalSequnce(string threadName, int numberSeuences, int numberOfWorkers, vector<Task> tasks, promise <vector<vector<Task*>>> &&result){
    cout << threadName << ": began work with number of workers equal <" << numberOfWorkers << ">\n";
    int minrResult = 1000000000;
    vector<Task> ts{};
    vector<Task> tasksCopy;
    vector<vector<Task*>> optimalSequence {};
    vector<vector<Task*>> sequence;
    for (int i = 0; i < numberSeuences; i++){
        if (i%1000 == 0)
            cout << threadName << ": uwent by iteration number <" << i << ">\n";
        tasksCopy = tasks;
        tasksCopy = setTasks(tasksCopy);
        sequence = generateTasksSequnce(tasksCopy,numberOfWorkers);
        int result = calculateUsability(sequence, numberOfWorkers);
        if (result < minrResult){
            cout << threadName << ": find new optimal sequence (" << minrResult << "/" << result << ") in <" << i << "> iteration!\n";
            minrResult = result;
            ts = copyTask(tasksCopy);
            optimalSequence = copyVectorPonters(sequence,tasksCopy,ts);
        }
    }
    cout << threadName << ": end his work on iteration <" << numberSeuences << "> with value of sequence equal <" << minrResult << ">\n";
    result.set_value(optimalSequence);
}

int main(int argc,char* argv[]){
    srand(time(NULL));
    string fileNameWithTasks = "data.json";
    ifstream jsonFile(fileNameWithTasks);
    json fileData = json::parse(jsonFile);
    jsonFile.close();
    vector<Task> tasks;
    for (json::iterator i = fileData.begin(); i != fileData.end(); i++){
        Task newTask(i.key());
        json jsonObjectNewTask = i.value();
        for (json::iterator j = jsonObjectNewTask.begin(); j != jsonObjectNewTask.end(); j++){
            if (j.key() == "workers")
                newTask.setWorkers(j.value());
            else if (j.key() == "days")
                newTask.setDays(j.value());
            else if (j.key() == "previous tasks"){
                json namesPrevTasks = j.value();
                for (json::iterator k = namesPrevTasks.begin(); k != namesPrevTasks.end(); k++)
                    newTask.addNamePrevTask(k.value());
            }
        }
        tasks.push_back(newTask);
    }
    vector<Task> firstElem(tasks);
    firstElem = setTasks(firstElem);
    int minnumberOfWorkers = 0;
    for (int i = 0; i < tasks.size(); i++)
        if (tasks[i].getWorkers() > minnumberOfWorkers)
            minnumberOfWorkers = tasks[i].getWorkers();
    int numberOfWorkers = 0;
    if (argc == 1){
        numberOfWorkers = minnumberOfWorkers;
        cout << "Was not input number of workers!\n Number of workers was set as minimum required workers for biggest task: (" << numberOfWorkers << ")\n";
    }
    else if (argc >1 ){
        numberOfWorkers = atoi(argv[1]);
        if (numberOfWorkers < minnumberOfWorkers){
            numberOfWorkers = minnumberOfWorkers;
            cout << "Input number of workers less than required for biggest task!\n Number of workers was set equal this required biggest task: (" << numberOfWorkers << ")\n";
        }
        else
            cout << "Number of workers was set equal: (" << numberOfWorkers << ")\n";
        }
    // printSequence(generateTasksSequnce(firstElem,20));
    // return 0;

    vector<vector<vector<Task*>>> sequences{};
    vector<Task> tasksCopy;
    vector<Task> ts;
    vector<vector<Task*>> sequence;
    for (int i = 0; i < 80; i++){
        tasksCopy = tasks;
        tasksCopy = setTasks(tasksCopy);
        sequence = generateTasksSequnce(tasksCopy,numberOfWorkers);
        ts = copyTask(tasksCopy);
        sequences.push_back(vector<vector<Task*>>(copyVectorPonters(sequence,tasksCopy,ts)));
    }
    printSequence(sequences[10]);
    return 0;
    // promise<vector<vector<vector<Task*>>>> seuqenseQueue{};
    // auto f = seuqenseQueue.get_future();
    vector<promise <vector<vector<Task*>>>> listOfThreadResults{};
    vector<future <vector<vector<Task*>>>> F{};
    vector<thread> threads{};
    for (int i = 0; i < 8; i++){
        listOfThreadResults.push_back(promise<vector<vector<Task*>>>{});
        F.push_back(listOfThreadResults[i].get_future());
        string threadName = "THREAD_";
        threads.push_back(thread(&findOptimalSequnce, threadName + to_string(i), 10000, numberOfWorkers, tasks, move(listOfThreadResults[i])));
    }
    for (int i = 0; i < threads.size(); i++)
        threads[i].join();
    vector<vector<Task*>> result(F[0].get());
    for (int i = 1; i < listOfThreadResults.size(); i++){
        vector<vector<Task*>> oneOfResults(F[i].get());
        if (calculateUsability(result,numberOfWorkers) > calculateUsability(oneOfResults,numberOfWorkers))
            result = oneOfResults;
    }
    printSequence(result);
    cout << calculateUsability(result,numberOfWorkers) << endl;
    return 0;
}