#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

void recursion(int k);
int u;

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        if (! strcmp(name, candidates[i]))
        {
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // TODO
    for (int p = 0; p < candidate_count; p++)
    {
        for (int q = p + 1; q < candidate_count; q++)
        {
            preferences[ranks[p]][ranks[q]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // TODO
    for (int p = 0; p < candidate_count; p++)
    {
        for (int q = p + 1; q < candidate_count; q++)
        {
            if (preferences[p][q] > preferences[q][p])
            {
                pairs[pair_count].winner = p;
                pairs[pair_count].loser = q;
                pair_count++;
            }
            else if (preferences[p][q] < preferences[q][p])
            {
                pairs[pair_count].winner = q;
                pairs[pair_count].loser = p;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // TODO
    int largest, temp1, temp2;
    for (int p = 0; p < pair_count; p++)
    {
        largest = preferences[pairs[p].winner][pairs[p].loser];
        for (int q = p + 1; q < pair_count; q++)
        {
            if (preferences[pairs[q].winner][pairs[q].loser] > largest)
            {
                largest = preferences[pairs[q].winner][pairs[q].loser];
                temp1 = pairs[q].winner;
                temp2 = pairs[q].loser;
                pairs[q].winner = pairs[p].winner;
                pairs[q].loser = pairs[p].loser;
                pairs[p].winner = temp1;
                pairs[p].loser = temp2;
            }
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // TODO
    for (u = 0; u < pair_count; u++)
    {
        if ((u == 0) || (u == 1))
        {
            locked[pairs[u].winner][pairs[u].loser] = true;
        }
        else
        {
            int k = pairs[u].loser;
            locked[pairs[u].winner][pairs[u].loser] = true;
            recursion(k);
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    int fcount, array[candidate_count], r = 0;
    for (int m = 0; m < candidate_count; m++)
    {
        for (int n = m + 1; n < candidate_count; n++)
        {
            fcount = 0;
            if (true)
            {
                for (int o = 0; o < candidate_count; o++)
                {
                    if (locked[o][m] == true)
                    {
                        fcount = 1;
                        break;
                    }
                }
                if (fcount == 0)
                {
                    array[r] = m;
                    r++;
                }
            }
        }
    }
    for (int k = 0; k < r; k++)
    {
        printf("%s\n", candidates[array[k]]);
    }
    return;
}

void recursion(int k)
{
    for (int j = 0; j < candidate_count; j++)
    {
        if (locked[k][j] == true)
        {
            if (j == pairs[u].winner)
            {
                locked[pairs[u].winner][pairs[u].loser] = false;
                return;
            }
            else
            {
                recursion(j);
            }
        }
    }
    return;
}