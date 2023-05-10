# ifndef SCHEDULER_H

# define SCHEDULER_H 

# include <iostream>
# include <cstring>
# include <vector>
# include <cstdlib>
# include <chrono>
# include <time.h>
# include <unistd.h>
# include <algorithm>
# include <math.h>

# define RAND(min, max)  ((int)((double) rand() / (RAND_MAX) * ((max) - (min))) + (int)(min))
# define NOR_RAND()      ((double)rand() / ((RAND_MAX)))

# define PRINT       0
# define Max_iter    20

using namespace std::chrono;
using namespace std;


typedef struct task{
    uint16_t num;
    char executor;
    uint16_t r_time;
    uint16_t h_time;
    vector<uint16_t> and_prev;
    vector<uint16_t> and_next;
    vector<uint16_t> or_prev;
    vector<uint16_t> or_next;
}mtask;


typedef struct job{
    uint16_t taskNum;
    vector<mtask> task_list;
}mJob;


class Scheduler
{
public:
    uint16_t GetSolution();
    double GetCalculateTime();

    mJob mjob;
    double calculate_time;

    bool *leaf_node;
    bool *finish_node;
    bool *root_node;
    vector<uint16_t> alternative_node;
    vector<uint16_t> avaliable_node;
    vector<vector<uint16_t>> avaliable_node_GP;

    vector<uint16_t> min_fit_list;
    vector<uint16_t> avg_fit_list;
    // Members used in Kizilay's scheduler
    uint16_t taskNum; // How many task in the job
    uint16_t station; // Best value of used station
    void PreProcessing(mJob);
    vector<vector<uint16_t>> Sequence_Generate();
    uint16_t Fitness(vector<vector<uint16_t>>);
    bool Valid(vector<uint16_t>);
    void PRINT_SEQUENCE(vector<vector<uint16_t>>);

    private:
};


# endif