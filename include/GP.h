# include <queue>
# include <stack>
# include "Scheduler.h"
# include <assert.h>

# define Init_deep  2
# define Max_deep   6
# define ELITE_SIZE    2 // POP_SIZE * 10%
# define POP_SIZE    20
# define CR         0.6 // Crossover rate
# define MR         0.1 // Mutation rate

typedef struct node {
    int Operand;
    int depth;
    struct node *lnode;
    struct node *rnode;
} Node; 



/**
 * Nonterminate node: 
 * ADD: a + b
 * SUB: a - b
 * MUL: a * b
 * DIV: protect a / b
 * MAX: max(a, b)
 * MIN: min(a, b)
 * 
 * Terminate node: determined by some parameters of whole 
 * scheduling plan
 * 
 * PT: processing time
 * MS: makespan after choosing
 * RT: robot time after choosing
 * HT: human time after choosing
 * TJ: total job number
 * RJ: remaining job number
 * ID: idle time after choosing
*/
enum Operand {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    MAX = 5,
    MIN = 6,
    PT = 7,
    MS = 8,
    RT = 9,
    HT = 10,
    TJ = 11,
    RJ = 12,
    ID = 13
};


class GP: private Scheduler{
    public:
        static GP *GetGP();
        ~GP();
        void PreProcessing(mJob);
        uint16_t GetSolution();
        double GetCalculateTime();
        void RunAlgorithm();

    private:
        static GP *inst_;
        vector<uint16_t> stack_; // store postorder sequence
        uint16_t terminate[7];

        GP(); 
        Node *Node_init(int);
        Node *Tree_init(int, int, int);
        void AssertNode(Node *);
        void Postorder(Node *);
        double Priority();
        uint16_t GP_Fitness(Node *);
        void CrossOver(Node *, Node *);
        void Mutation(Node *);
        vector<double> Rescale(vector<uint16_t>);
        void FreeTree(Node *);
        Node *CloneTree(Node *);
        bool Validate_Depth(Node *, int);
        int Tree_Height(Node *);
        void Resize_deep(Node *, Node *, Node *);


        // test 
        double fitness_time;
};      