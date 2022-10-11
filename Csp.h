#pragma once
#ifndef CSP_H
#define CSP_H


#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <chrono>



typedef std::pair<unsigned short int, unsigned short int > pair;

class Solver
{
public :
	//constructor 
	Solver (char blank_): blank(blank_){};
    

    std::vector<std::vector <char>> get_sudoku()
    {
        return sudoko_matrix;
    }

    //to insert a new sudoko
    //we use this tecnique instead of pass the sudoku to the constructor so in this way 
    //we can use the same object to solve many sudokus 
    void insert_sudoku(std::vector<std::vector <char>>& sudoko_matrix_)
    {
        sudoko_matrix = sudoko_matrix_;
        //new sudokus i have to initialize again number_iteartions
        number_iterations = 0;
        backtrack = 0;
    }

    //return the number of recursive call 
    //We use this data member to monitoring the efficient of the algorithm 
    int get_iteration()
    {
        return number_iterations;
    }
    int get_backtrack()
    {
        return backtrack;
    }


    /*
    * Inner class that i use as a conatiner to rappresent the information needde for a sudoko's cell
    */
    class Cell
    {
    public:
        //constrcuctor 
        Cell(Solver& parent_, unsigned short x_, unsigned short int y_) : parent(parent_), cordinate(x_, y_) {};

        
        //rappresent the cordinate of the cell in the sudoku ground
        pair cordinate;

        /*
        *  define the domain of each cell.
        *  I am using an array of occorence. I start with the assumption that each cell's domain is complete
        *  and i do a reduction each time
        */
        std::vector<unsigned short int > domain = { 1,2,3,4,5,6,7,8,9};

        /*
        *  Unlike Java though, there is no correlation between an object of type Outer::Inner
        *  and an object of the parent class. You have to make the parent child relationship manually.
        */
        Solver& parent;
    };

   
	

    /*
    * print the sudoko field in the terminal
    */
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


    /*
    * we use this function to look in the matrix to a possible unsigned cell
    * @return : the cordinate of the unsigned  cell foundeed  
    */
    Cell unsigned_cell(pair cordinate)
    {
        unsigned short int tmp_col = cordinate.second;
        for (unsigned short int i = cordinate.first; i < sudoko_matrix.size(); i++)
        {
            for (unsigned short int j = tmp_col; j < sudoko_matrix[i].size(); j++)
            {
                //free cell founded 
                if (sudoko_matrix[i][j] == blank)
                {
                    Cell c(*this, i, j);
                    return c;
                }
            }
            tmp_col = 0;
        }
        //no free cell founded 
        Cell c(*this, 10, 10);
        return c;
    
    };


    /*
    * it modify the domain of the cell using the sudoko's rules 
    * @param Cell the sudoko's cell that we are triyng to set 
    */
    void check_row(Cell& c) 
    {
        unsigned short int i = c.cordinate.first;
        for (unsigned short int j = 0; j < sudoko_matrix[i].size(); j++)
        {
            if (sudoko_matrix[i][j] != blank)
            {
                //find if present in the domain
                if (std::find(c.domain.begin(), c.domain.end(), sudoko_matrix[i][j] - 48) != c.domain.end())
                {
                    std::remove(c.domain.begin(), c.domain.end(), sudoko_matrix[i][j] - 48);
                    c.domain.resize(c.domain.size() - 1);
                    
                }
                
            }
        }
        
    };


    /*
    * it modify the domain of the cell using the sudoko's rules
    * @param Cell the sudoko's cell that we are triyng to set
    */
    void check_column(Cell& c)
    {
        //the column
        unsigned short int j = c.cordinate.second;
        for (unsigned short int i = 0; i < sudoko_matrix[j].size(); i++)
        {
            if (sudoko_matrix[i][j] != blank)
            {
                //find if present in the domain
                if (std::find(c.domain.begin(), c.domain.end(), sudoko_matrix[i][j] - 48) != c.domain.end())
                {
                    std::remove(c.domain.begin(), c.domain.end(), sudoko_matrix[i][j] - 48);
                    c.domain.resize(c.domain.size() - 1);
                }
            }
        }
    };


    /*
    * it modify the domain of the cell using the sudoko's rules
    * @param Cell the sudoko's cell that we are triyng to set
    */
    void check_square(Cell& c)
    {
        unsigned short start_row = c.cordinate.first - (c.cordinate.first % 3);
        unsigned short start_coloumn = c.cordinate.second - (c.cordinate.second % 3);
        for (unsigned short i = 0; i < 3; i++)
        {
            for (unsigned short j = 0; j < 3; j++)
            {
                if (sudoko_matrix[i + start_row][j + start_coloumn] != blank)
                {
                    //find if present in the domain
                    if (std::find(c.domain.begin(), c.domain.end(), sudoko_matrix[i + start_row][j + start_coloumn] - 48) != c.domain.end())
                    {
                        std::remove(c.domain.begin(), c.domain.end(), sudoko_matrix[i + start_row][j + start_coloumn] - 48);
                        c.domain.resize(c.domain.size() - 1);
                    }

                }
            }
        }
    
    };


    /*
    * it does the upadte the domain of the given cell using the sudoko rules 
    * at the end the given cell has all the possibilities that can try at the current time of the game
    * @param Cell the sudoko's cell that we are triyng to set
    */
    void update_domain(Cell& c)
    {
        check_row(c);
        check_column(c);
        check_square(c);
    };



    //check if it is a valid cell
    inline bool valid_cell(Cell& c)
    {
        return (c.cordinate.first <= 9) ? true : false;
    }



    /*
    * main function the we used to try the possible path that will bring us to the sudoko solutions
    */
    bool solve(pair cordinate)
    {
       number_iterations++;
       Cell c = unsigned_cell(cordinate);
       //no more cells available
       if (!valid_cell(c))
       {
           return true;
       }
       //we have a valid cell
       else
       {
           update_domain(c);
           //we have to try the possible branches for the current cell
           while (c.domain.size() > 0 )
           {
               //try to insert the value of the domain
               sudoko_matrix[c.cordinate.first][c.cordinate.second] = static_cast<char>(c.domain[c.domain.size() - 1] + 48);
               c.domain.pop_back();
               //try the branch with DFS  
               if (solve(pair(c.cordinate.first, c.cordinate.second)))
               {
                   return true;
               }
           }
           //we tried  all the possibilities of the actual cell domain but didn't work
           //we have to restore the value that we try to assign to the current cell
           sudoko_matrix[c.cordinate.first][c.cordinate.second] = blank;
           //start the backtraking
           backtrack++;
           return false;
       }
    };


    std::vector<std::vector <char>> start()
    {
        solve(pair(0,0));
        return get_sudoku();
    };
    

private:
	std::vector<std::vector <char>> sudoko_matrix;
    
    char blank;

    int number_iterations = 0;
    int backtrack = 0;

};


#endif