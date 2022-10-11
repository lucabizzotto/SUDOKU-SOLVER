

#ifndef RELAXATION_LABELING
#define RELAXATION_LABELING

#include <iostream>
#include <vector>
#include <cstdlib>
#include <map>
#include<set>

typedef std::pair<unsigned short int, unsigned short int > pair;


class Relaxation_labeling
{
public:
	//constructor 
	Relaxation_labeling(char blank_) : blank(blank_) {};

    /*
    * to insert a new sudoko we use this tecnique instead of pass the sudoku to the constructor so in this way 
    * we can use the same object to solve many sudokus 
    */
    void insert_sudoku(std::vector<std::vector <char>>& sudoko_matrix_)
    {
        sudoko_matrix = sudoko_matrix_;
        //restart the data member 
        number_iterations = 0;
       p_0.clear();
        q.clear();
        p_1.clear();

    }

    //return the numbers of iteration that the algorithim compute to solve the sudoku
    int get_iterations()
    {
        return number_iterations;
    }


    //utility function
    void print_matrix()
    {
        std::cout << "sudoku : " << std::endl;
        for (unsigned short int i = 0; i < sudoko_matrix.size(); i++)
        {
            for (unsigned short int j = 0; j < sudoko_matrix[i].size(); j++)
            {
                std::cout << sudoko_matrix[i][j] << "    ";
            }
            std::cout << std::endl;
        }
    }

    //it checks the constraint rule in the row
     void check_row(std::set<unsigned short int>& set, unsigned short int& row)
     {
         for (unsigned short int j = 0; j < sudoko_matrix.size(); j++)
         {
             if (sudoko_matrix[row][j] != blank)
             {
                 set.insert(sudoko_matrix[row][j] - 48);
             }
         }
     }



     //it checks the constraint rule in the column
     void check_column(std::set<unsigned short int>& set, unsigned short int& coloumn)
     {
         for (unsigned short i = 0; i < sudoko_matrix.size(); i++)
         {
             if (sudoko_matrix[i][coloumn] != blank)
             {
                 set.insert(sudoko_matrix[i][coloumn] - 48);
             }
         }
     }



     //it checks the constraint rule in the square
     void check_square(std::set<unsigned short int>& set, unsigned short int& row , unsigned short int & coloumn)
     {
         unsigned short int start_row = row - (row % 3);
         unsigned short int start_coloumn = coloumn - (coloumn % 3);
         for (unsigned short int i = 0; i < 3; i++)
         {
             for (unsigned short j = 0; j < 3; j++)
             {
                 if (sudoko_matrix[i + start_row][j + start_coloumn] != blank)
                 {
                     set.insert(sudoko_matrix[i + start_row][j + start_coloumn] - 48);
                 }
             }
         }

     }


     /*
     * given a cell it checks its possible vlaues and then create its vector of probability p_0
     */
     void constraint_elements(std::set<unsigned short int>& set, pair& cordinate)
     {
         
         check_row(set,cordinate.first);
         check_column(set, cordinate.second);
         check_square(set,cordinate.first,cordinate.second);
         //here the set contain all the element that we can't use
         //we use uniform distrinìbution among all possible labels
         double probability = 1./(9. -set.size());
         std::vector<double> probability_vector(9, 0.0);
        p_0.insert_or_assign(cordinate,probability_vector);
         
         for (unsigned short int i = 0; i < 9; i++)
         {
             auto it = set.find((i + 1));
             if (it == set.end())
             {
                p_0.find(cordinate)->second[i] = probability;
             }
         }
     }

     //utility function
     void print_probability_vector()
     {
         for (auto it : this->p_0)
         {
             std::cout <<"( " << it.first.first << " , " << it.first.second << " )" << std::endl;
             for (auto x : it.second)
             {
                 std::cout << x << "    ";
             }
             std::cout << std::endl;
         }
     
     }

     //used to implement the compatibility function
     bool same_row(pair& i, pair& j)
     {
         return (i.first == j.first) ? true : false;
     }


     //used to implement the compatibility function
     bool same_coloumn(pair& i, pair& j)
     {
         return (i.second == j.second) ? true : false;
     }


     //used to implement the compatibility function
     bool same_square(pair& i, pair& j)
     {
         unsigned short int start_row = i.first - (i.first % 3);
         unsigned short int start_coloumn = i.second - (i.second % 3);
         //the two cycles for below define the square
         for (unsigned short int x = 0; x < 3; x++)
         {
             for (unsigned short y = 0; y < 3; y++)
             {
                 if (x + start_row == j.first && y + start_coloumn == j.second)
                     return true;
             }
         }
         return false;
     }

   




     //used to compute the compatibility coefficient
     int compatibility(pair i, pair j, unsigned short int lambda, unsigned short int mu)
     {
         if (i.first == j.first && i.second == j.second)
             return 0;
         if (lambda != mu)
             return 1;
         if (same_row(i, j) || same_coloumn(i, j) || same_square(i, j))
             return 0;
         return 1;

     }


     
     /*
     * initialize matrix p a time 0
     * our matrix contains only the cell that are unsigned.
     * we dont take into consideration cell that in the given sudoko are all ready set up
     * in this way we can obtain a smaller matrix saving some space
     */
    void initialize_p0()
    {
        for (unsigned short int i = 0; i < sudoko_matrix.size(); i++)
        {
            for (unsigned short int j = 0; j < sudoko_matrix[i].size(); j++)
            {
                if (sudoko_matrix[i][j] == blank )
                {
                    //set containing the element that we can't use
                    std::set<unsigned short int> s;
                    pair cordinate = pair(i, j);
                    //to build the right vector of probability 
                    constraint_elements(s,cordinate);
                }
            }
        }
    }

  

    //initialize q 
    void initialize_q()
    {
        for (auto it :p_0)
        {
            std::vector<double> vet(9, 0.0);
            q.insert_or_assign(it.first, vet);
            
        }
        //passed by value
        p_1 =p_0;
        populate_q();
    }

   
    /*
    * we populate q 
    * we are using the formula of relaxation labelling operator (1976 Rule)
    */
   void populate_q()
    {
       //for each object i
        for (auto iter_i :p_0)
        {
            //fix the label
            for (unsigned short int lambda = 1; lambda <= iter_i.second.size(); lambda++)
            {
                //for each possible object j
                for (auto iter_j :p_0)
                {
                    //for each label mu
                    for (unsigned short int mu = 1; mu <= iter_j.second.size(); mu++)
                    {
                       
                        q[iter_i.first][lambda - 1] += compatibility(iter_i.first, iter_j.first, lambda, mu) *p_0[iter_j.first][mu - 1];
                    }
                }
            }
        }
    }

 
   /*
   * we update q in respect of the formula 
   */
    void update_q()
    {
        for (auto iter_i :p_0)
        {
            for (unsigned short int lambda = 1; lambda <= iter_i.second.size(); lambda++)
            {
                q[iter_i.first][lambda - 1] = 0.;
                for (auto iter_j :p_0)
                {
                    for (unsigned short int mu = 1; mu <= iter_j.second.size(); mu++)
                    {
                        //sto creadno q al tempo al tempo t per questo ho bisogno di p_1
                        q[iter_i.first][lambda - 1] += compatibility(iter_i.first, iter_j.first, lambda, mu) * p_1[iter_j.first][mu - 1];
                    }
                }
            }
        }
    }


    //p a time t+1
    void update_p()
    {
        for (auto iter_i :p_0)
        {
            for (unsigned short int lambda = 1; lambda <= 9; lambda++)
            {
                double count = 0.;
                for (unsigned short int mu = 1; mu <= 9; mu++)
                {

                    count +=p_0[iter_i.first][mu - 1] * q[iter_i.first][mu - 1];
                }
                p_1[iter_i.first][lambda - 1] = (p_0[iter_i.first][lambda - 1] * q[iter_i.first][lambda - 1])/ count ;
            }
        }

    }


    /*
    * we are computing the average local consistency
    */
    double avarage_consistency()
    {
        double count = 0.;
        for (auto iter_i :p_0)
        {
            for (unsigned short int lambda = 1; lambda <= 9; lambda++)
            {
                count += p_1[iter_i.first][lambda - 1] * q[iter_i.first][lambda - 1];
            }
        }
        return count;
    }


    /*
    * we calculate the average consistency 
    * we keep going to itearte until the difference from the average consistency at time p id close to 
    * the avarage consistency a time p + 1
    */
    void solve()
    {
        double avarage = avarage_consistency();
        while (avarage > 0.01)
        {
            number_iterations++;
            avarage = avarage_consistency();
            update_p();
            update_q();
            avarage = std:: abs(avarage_consistency() - avarage);
            //sovarscrivo il tempo t con t+1
           p_0 = p_1;

        }
    }


    /*
    * at the end we have to write in the sudoku matrix for each empty cell  the value that has the hihgest probability 
    */
    void write_on_sudoko_matrix()
    {
        for (auto it :p_0)
        {
            auto max = std::max_element(it.second.begin(), it.second.end());
            int pos = std::distance(it.second.begin(), max);
            
            sudoko_matrix[it.first.first][it.first.second] = static_cast<char> (pos + 1 +48);
           
        }
    
    }
    

    //start function
    std::vector<std::vector <char>> start()
    {
        initialize_p0();
        initialize_q();
        solve();
        write_on_sudoko_matrix();
        return sudoko_matrix;
    }



private:
    //sudoku matrix
	std::vector<std::vector <char>> sudoko_matrix;
	
    //containing the unsigned cell with its corripondign vector of probabilities
    //p_0
    std::map<pair, std::vector<double>>p_0; 

    //q matrix
    std::map<pair, std::vector<double>> q;

    //matrix a time p + 1
    std::map<pair, std::vector<double>> p_1;

    //to give indipendent from the sudoko rappresentation given 
    char blank;

    //to calculate numeber of iteration of 
    //it given us an idea of efficinty
    int number_iterations = 0;


};












#endif