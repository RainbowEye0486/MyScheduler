
# include "Scheduler.h"


# define T_init      3
# define T_min       0.009
# define Alpha       0.95

class SA: private Scheduler{
    public:
        static SA *GetSA();
        SA();
        ~SA();
        void PreProcessing(mJob);
        uint16_t GetSolution();
        double GetCalculateTime();
        void RunAlgorithm();

    private:
        static SA *inst_;
        vector<vector<uint16_t>> Neighbor(vector<vector<uint16_t>>);       
};