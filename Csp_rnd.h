#pragma once
#pragma once
#include <iostream>
#include <vector>
#include <cstdlib>
#include <algorithm>

#include <chrono>

typedef std::pair<unsigned short int, unsigned short int > pair;

class Solver
{
public:
    //constructor 
    Solver(char blank_) : blank(blank_) {};






    /*
    * Inner class that i use as a conatiner to rappresent the information needde for a sudoko's cell
    */
    class Cell
    {
    public:
        Cell(Solver& parent_, unsigned short int x_, unsigned short  int y_) : parent(parent_), cordinate(x_, y_) {};
        
        //rappresent the cordinate of the cell in the sudoku ground
        pair cordinate;

        /*
        *  define the domain of each cell.
        *  I am using an array of occorence. I start with the assumption that each cell's domain is complete
        *  and i do a reduction each time
        */
        std::vector<unsigned short int > domain = { 1,2,3,4,5,6,7,8,9 };

        /*
        *  Unlike Java though, there is no correlation between an object of type Outer::Inner
        *  and an object of the parent class. You have to make the parent child relationship manually.
        */
        Solver& parent;
    };


    //to insert a new sudoko
   //we use this tecnique instead of pass the sudoku to the constructor so in this way 
   //we can use the same object to solve many sudokus 
    void insert_sudoku(std::vector<std::vector <char>>& sudoko_matrix_)
    {
        sudoko_matrix = sudoko_matrix_;
        //new sudokus i have to initialize again number_iteartions
        number_iterations = 0;
        backtrack = 0;
        occorence.clear();
    }


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
    * it has the responsability to initialize the occorence vector.
    * occorence vector store the cordinate of the unsigned cell of the sudoko
    */
    void initiliaze_occorence()
    {
        
        for (unsigned short int i= 0; i < sudoko_matrix.size(); i++)
        {
            for (unsigned short int j=0; j < sudoko_matrix[i].size(); j++)
            {
                //unsigned cell
                if (sudoko_matrix[i][j] == blank )
                {
                    occorence.push_back( pair(i, j));
                }
                
            }
        }
       
    }


    /*
    * we use this function to look in the matrix to a possible unsigned cell
    * @return : the cordinate of the unsigned  cell foundeed
    */
    Cell unsigned_cell()
    {
        //still have unsigned cell
        if (occorence.size() > 0)
        {
            //choose random key 
            unsigned short int rnd = std::rand() % occorence.size();
            auto x = std::find(occorence.begin(), occorence.end(), occorence[rnd]);
            Cell c(*this, x->first, x->second);
            occorence.erase(x);
            return c;
        }
        //no free cell founded 
        Cell c(*this, 10, 10);
        return c;
    }

    /*
   * it modify the domain of the cell using the sudoko's rules
   * @param Cell the sudoko's cell that we are triyng to set
   */
    void check_row(Cell& c)
    {
        unsigned short int i = c.cordinate.first;
        for (unsigned short int j = 0; j < sudoko_matrix[i].size(); j++)
        {
            if (sudoko_matrix[i][j] != blank )
            {
                //find if present in the domain
                if (std::find(c.domain.begin(), c.domain.end(), sudoko_matrix[i][j] - 48) != c.domain.end())
                {
                    std::remove(c.domain.begin(), c.domain.end(), sudoko_matrix[i][j] - 48);
                    c.domain.resize(c.domain.size() - 1);
                }
            }
        }
    }



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
            if (sudoko_matrix[i][j] != blank )
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
        unsigned short int start_row = c.cordinate.first - (c.cordinate.first % 3);
        unsigned short int start_coloumn = c.cordinate.second - (c.cordinate.second % 3);
        for (unsigned short int i = 0; i < 3; i++)
        {
            for (unsigned short int j = 0; j < 3; j++)
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
    bool solve()
    {
        number_iterations++;
        Cell c = unsigned_cell();
        //no more cells available
        if (!valid_cell(c))
        {
            return true;
        }
        //we have a valid cell
        else
        {
            update_domain(c);
            //we have to try the possible branches
            //if domain is equal to 0 ?
            //return false but i removed the cell i didnt put back 
            while (c.domain.size() > 0)
            {
                //try to insert the value of the domain
                sudoko_matrix[c.cordinate.first][c.cordinate.second] = static_cast<char>(c.domain[c.domain.size() - 1] + 48);
                c.domain.pop_back();
                //try the branch with DFS return true if all the below nodes are correct 
                if (solve() )
                {
                    return true;
                }
                //we explored the bellow nodes with the value that we assign but didn't wokout 
                //we have to restore the value that we try to assign to the current cell
                //occorence.push_back(pair(c.cordinate.first, c.cordinate.second));
                
                
            }
            occorence.push_back(pair(c.cordinate.first, c.cordinate.second));
            sudoko_matrix[c.cordinate.first][c.cordinate.second] = blank ;
            //we try all the possibilities of the actual cell domain but didn't work
            backtrack++;
            return false;
        }
    };



    std::vector<std::vector <char>> start()
    {
        initiliaze_occorence();
        solve();
        std::cout << occorence.size() << std::endl;
        return sudoko_matrix;
    };


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


private:
    //sudoko matrix 
    std::vector<std::vector <char>> sudoko_matrix;
    //array of occoronce that contain all the possible available cell of the sudoko
    std::vector<pair> occorence;
    char blank;

    int number_iterations = 0;
    int backtrack = 0;
};