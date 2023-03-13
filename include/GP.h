# include <queue>
# include <stack>
# include "Scheduler.h"
# include <assert.h>

# define NEW_ALG    true

# define Init_deep  2
# define Max_deep   6
# define POP_SIZE    20
# define ELITE_SIZE  0
# define CR         0.8 // Crossover rate
# define MR         0.1 // Mutation rate

typedef struct node {
    int Operand;
    int depth;
    int height;
    int sticky;
    struct node *lnode;
    struct node *rnode;
    struct node *parent;
} Node;



/**
 * Nonterminate node: 
 * ADD: a + b
 * SUB: a - b
 * MUL: a * b
 * DIV: protect a / b
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
    PT = 5,
    MS = 6,
    RT = 7,
    HT = 8,
    TJ = 9,
    RJ = 10,
    ID = 11
};

enum Executor {
    ROBOT = 0,
    HUMAN = 1,
    BOTH = 2
};


class GP: private Scheduler{
    public:
        static GP *GetGP();
        ~GP();
        void PreProcessing(mJob);
        uint16_t GetSolution();
        double GetCalculateTime();
        void RunAlgorithm();
        double GetAvgFitness(int i);

    private:
        static GP *inst_;
        vector<uint16_t> stack_; // store postorder sequence
        uint16_t terminate[7];
        int feature_usage[11];

        GP(); 
        Node *Node_init(int);
        Node *Tree_init(int min_deep, int max_deep, int deep_init, string);
        void AssertNode(Node *);
        int Postorder(Node *);
        double Priority();
        uint16_t GP_Fitness(Node *);
        void CrossOver(Node *, Node *);
        void Mutation(Node *);
        Node *Trim(Node *, uint16_t);
        vector<double> Rescale(vector<uint16_t>);
        void FreeTree(Node *);
        Node *CloneTree(Node *);
        bool Validate_Depth(Node *, int);
        void Resize_height(Node *);
        void Resize_deep(Node *, Node *, Node *);
        int AssertHeight(Node *);
        Node *Random_node(Node *root, bool weighted);
        // test 
        double fitness_time;
        int node_num_accum;
        bool feature_flag;
        float trim_ratio[6];

        float trim_success_rate;
        vector<Node *> history_tree; 
        vector<uint16_t> history_fitness;
};      