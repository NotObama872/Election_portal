#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

/*max number of candidates*/
#define MAX 9

/*preferences[i][j] is the number of voters who prefer i over j, i and j being candidate index*/
int preferences[MAX][MAX];

/*bool type 2-d array, if true, locked[i][j] means i is locked in over j*/
bool locked[MAX][MAX];

/*each pair has a winner and a loser index, win_margin is the number of votes won*/
struct pair
{
    int winner;
    int loser;
    int win_margin;
};

/*used for merge-sorting the pairs, based on win_margin*/
struct left
{
    int winner;
    int loser;
    int win_margin;
};

struct right
{
    int winner;
    int loser;
    int win_margin;
};

char* candidate[MAX];
struct pair pairs[MAX * (MAX - 1) / 2];
int pair_count;
int candidate_count;

/*prototype functions*/
bool vote(int rank, char name[], int ranks[]);
void record_preferrences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void merge_sort(struct pair* pairs, int beg_index, int end_index);
void merge(struct pair* pairs, int beg_index, int end_index, int mid);
void lock_pairs(void);
void print_winner(void);

int main(int argc, char* argv[])
{
    char buffer[100];
    int voter_count;
    pair_count = 0;


    /*check the commanline input validty*/
    if (argc < 2 || argc > MAX)
        printf("Need to put in candidates' names, and max number of candidates is 9.\n");

    /*populate the candidate array from command line arguments*/
    candidate_count = argc - 1;

    /*memory allocation*/   
    for (int i = 0; i < MAX; i++)
    {
        if ((candidate[i] = malloc(50)) == NULL)
            printf("Error. memory allocation unseccessful.\n");
    }

    /*populate the candidate array with candidates' names*/
    for (int j = 0; j < candidate_count; j++)
    {
        strcpy(candidate[j], argv[j + 1]);
        printf("Candidate %d is: %s\n", j + 1, candidate[j]);
    }

    /*clear out the graph of locked in pairs*/
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    /*get number of voters*/
    do
    {
        int flag = 0;
        printf("Number of voters: ");
        fgets(buffer, sizeof(buffer), stdin);
        sscanf(buffer, "%d", &voter_count);
        if (voter_count < 2)
        {
            printf("Enter a number that's larger than 2.\n");
            continue;
        }
        
        for (int i = 0; i < strlen(buffer) - 1; i++)
        {
            if (isdigit(buffer[i]) == 0)    /*found non-numeric character with in user input*/      
            {
                if (buffer[i] == '-' || buffer[i] == '+')
                    continue;
                else
                {
                    printf("Enter only whole numbers that's larger than 2.\n");
                    flag = 1;
                    break;
                }
            }
        }

        if (flag == 1)
            continue;
        else
            break;
    } while (true);

    /*gather rank info for each vote; check each vote's validty; record preferences*/
    for (int i = 0; i < voter_count; i++)
    {
        /*ranks[i] represent voter's i th preference*/
        int* ranks;
        if((ranks = malloc(sizeof(int) * candidate_count) == NULL)
            printf("memory allocation for voter[%d]'s ranks failed.\n", i + 1);
        char name[50];
        
        for (int j = 0; j < candidate_count; j++)
        {
            do
            {
                int flag = 0;
                printf("Ballot %d, rank %d: ", i + 1, j + 1);
                fgets(name, sizeof(name), stdin);
                name[strlen(name) - 1] = '\0';
                if (!vote(j, name, ranks))
                {
                    printf("Invalid name entered. Please Re-enter.\n");
                    continue;
                }
                if (j >= 1)
                {
                    for (int k = 0; k < j; k++)
                    {
                        if (strcmp(candidate[ranks[k]], candidate[ranks[j]]) == 0)
                        {
                            flag = 1;
                            printf("Duplicate name entered. Please Reenter.\n");
                            break;
                        }
                    }
                    
                    if (flag == 1)
                        continue;
                    else
                        break;
                }
                else
                    break;
            } while (true);
            free(ranks);
        }
        
        /*after each vote info has been collected, record perferences in preferences[i][j]*/
        record_preferrences(ranks);
        printf("\n");   
    }

    /*for each pair of candidates, record winner and loser.*/
    add_pairs();

    /*sort the pairs, by the winning margin of each pair*/
    sort_pairs();

    /*lock pairs with the largest win margin first, keep locking down the list until a cycle is created*/
    lock_pairs();

    print_winner();
}

/*check each vote's validty; update the ranks[], ranks[0] is the first rank of a vote*/
bool vote(int rank, char name[], int ranks[])
{
    int flag = 0;
    for (int k = 0; k < candidate_count; k++)
    {
        if (strcmp(name, candidate[k]) == 0)
        {
            flag = 1;
            ranks[rank] = k;
            break;
        }   
    }
    if (flag == 0)
        return false;
    else
        return true;
}

/*record preferences*/
void record_preferrences(int ranks[])
{
    /*loop through each rank in ranks[], */
    for (int i = 0; i < candidate_count; i++)
    {
        /*rank[0] is preferred over all other ranks; rank[1] preferred over all ranks after it */
        int winner = ranks[i];
        for (int j = i + 1; j < candidate_count; j++)
        {
            int loser = ranks[j];
            preferences[winner][loser] += 1;
        }
    }
}

/*record the preferences in struct pair pairs*/
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pairs[pair_count].win_margin = preferences[i][j];
                printf("pair[%d], in pair[%d][%d] winner is: %s; loser is : %s, by %d votes.\n", pair_count, i + 1, j + 1, candidate[i], candidate[j], preferences[i][j]);
            }
            if (preferences[j][i] > preferences[i][j])
            {
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pairs[pair_count].win_margin = preferences[j][i];
                printf("pair[%d], in pair[%d][%d] winner is: %s; loser is : %s, by %d votes.\n", pair_count, i + 1, j + 1, candidate[j], candidate[i], preferences[j][i]);
            }
            if(preferences[i][j] == preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pairs[pair_count].win_margin = 0;
                printf("pair[%d], in pair[%d][%d], there is a tie between %s and %s.\n", pair_count, i + 1, j + 1, candidate[i], candidate[j]);
            }
            pair_count++;
        }
    }
    printf("There are %d pairs.", pair_count);
    printf("\n");
}

/*sort pairs based on the amount of winning margin in each pair*/
void sort_pairs(void)
{
    printf("unsorted array is:");
    for (int i = 0; i < pair_count; i++)
    {
        printf("%d ", pairs[i].win_margin);
    }

    /*use merge sort to sort the array*/
    int beg_index = 0;
    int end_index = pair_count - 1;
    merge_sort(pairs, beg_index, pair_count - 1);
    printf("\n");
    
    printf("sorted array is:");
    for (int i = 0; i < pair_count; i++)
    {
        printf("%d ", pairs[i].win_margin);
    }
    printf("\n");
}

/*produce the sorted array*/
void merge_sort(struct pair *pairs, int beg_index, int end_index)
{   
    if (beg_index < end_index)
    {
        int mid = (beg_index + end_index) / 2;
        merge_sort(pairs, beg_index, mid);
        merge_sort(pairs, mid + 1, end_index);
        merge(pairs, beg_index, end_index, mid);
    }   
}

/*merge the sorted 2 half arrays*/
void merge(struct pair* pairs, int beg_index, int end_index, int mid)
{
    int n1 = mid - beg_index + 1;
    int n2 = end_index - (mid + 1) + 1;
    
    struct left* left;
    if((left = malloc(sizeof(struct left) * (n1 + 1))) == NULL)
        printf("memory allocation failed for struct left array.\n");    
    struct right *right;
    if((right = malloc(sizeof(struct right) * (n2 + 1))) == NULL)
        printf("memory allocation failed for struct right array.\n");

    for (int i = 0; i < n1; i++)
    {
        left[i].win_margin = pairs[beg_index + i].win_margin;
        left[i].winner = pairs[beg_index + i].winner;
        left[i].loser = pairs[beg_index + i].loser;
    }
    for (int j = 0; j < n2; j++)
    {
        right[j].win_margin = pairs[mid + 1 + j].win_margin;
        right[j].winner = pairs[mid + 1 + j].winner;
        right[j].loser = pairs[mid + 1 + j].loser;
    }
        

    int i = 0;
    int j = 0;
    int k = beg_index;

    while (i < n1 && j < n2)
    {
        if (left[i].win_margin <= right[j].win_margin)
        {
            pairs[k].win_margin = left[i].win_margin;
            pairs[k].winner = left[i].winner;
            pairs[k].loser = left[i].loser;
            i++;
        }
        else
        {
            pairs[k].win_margin = right[j].win_margin;
            pairs[k].winner = right[j].winner;
            pairs[k].loser = right[j].loser;
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        pairs[k].win_margin = left[i].win_margin;
        pairs[k].winner = left[i].winner;
        pairs[k].loser = left[i].loser;
        i++;
        k++;
    }

    while (j < n2)
    {
        pairs[k].win_margin = right[j].win_margin;
        pairs[k].winner = right[j].winner;
        pairs[k].loser = right[j].loser;
        j++;
        k++;
    }
    free(left);
    free(right);
}

void lock_pairs(void)
{
    /*the pair with the largest win_margin always got locked first*/
    locked[pairs[pair_count - 1].winner][pairs[pair_count - 1].loser] = true;

    /*unique_ounter counts the number of unique candidate index on the loser side; if less than
candidate_count, keep locking since locking won't create a cycle*/
    int unique_counter = 1;
    int flag;

    for (int i = pair_count - 2; i >= 0; i--)
    {   
        for (int j = pair_count - 1; j >= i; j--)
        {
            flag = 0;
            /*check duplicate loser index for locked pairs so far*/
            if (pairs[i].loser == pairs[j].loser)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
            unique_counter += 1;
        if (unique_counter < candidate_count && pairs[i].win_margin != 0)
            locked[pairs[i].winner][pairs[i].loser] = true;
        if(unique_counter == candidate_count)
            break;
    }

    for (int i = pair_count - 1; i >= 0; i--)
    {
        if (locked[pairs[i].winner][pairs[i].loser])
        {
            printf("pair[%d] locked.\n", i);
        }
        else
        {
            printf("pair[%d] remains unlocked.\n", i);
        }
    }
}

void print_winner(void)
{
    int* lost_pool;
    if((lost_pool = malloc(sizeof(int) * candidate_count)) == NULL)
        printf("memory allocation failed for lost_pool array.\n");
    int* win_pool;
    if((win_pool = malloc(sizeof(int) * candidate_count)) == NULL)
        printf("memory allocation failed for win_pool array.\n");
    int flag;
    lost_pool[0] = pairs[pair_count - 1].loser;
    int k = 1;
    int l = 0;

    /*for all the locked pairs, find the candidates on the losing side and add to lost_pool array; 
for duplicates, only add once.*/
    for (int i = pair_count - 2; i >= 0; i--)
    {
        flag = 0;
        for (int j = pair_count - 1; j > i; j--)
        {
            if (pairs[i].loser == pairs[j].loser)
            {
                flag = 1;
                break;
            }
        }

        if (locked[pairs[i].winner][pairs[i].loser] && flag == 0)
        {
            lost_pool[k] = pairs[i].loser;
            k++;
        }
    }

    printf("Candidats who lost in locked pairs: ");
    for (int i = 0; i < k; i++)
    {
        printf("%s ", candidate[lost_pool[i]]);
    }
    printf("\n");
    
    /*the candidate(s) that is not in the lost_pool within the locked pairs, will be the winner*/
    
    for (int i = 0; i < candidate_count; i++)
    {
        int flag = 0;
        for (int j = 0; j < k; j++)
        {
            if (i == lost_pool[j])
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            win_pool[l] = i;
            l++;
        }
    }

    if (l == 1)
        printf("The Winner is: %s!\n", candidate[win_pool[0]]);
    if (l > 1)
    {
        printf("The following are in the win_pool: ");
        for (int i = 0; i < l; i++)
            printf("%s ", candidate[win_pool[i]]);
    }
    free(lost_pool);
    free(win_pool);
    
}
