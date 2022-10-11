#pragma once
#ifndef READ_H
#define READ_H


#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include<algorithm>


#include <tuple>
#include <iostream>
#include <sstream>

class Read
{
public:
	Read() {};

	//utility function
	void print_matrix(std::vector<std::vector <char>>& sudoku_matrix)
    {
        std::cout << "sudoku : " << std::endl;
        for (unsigned short int i = 0; i < sudoku_matrix.size(); i++)
        {
            for (unsigned short int j = 0; j < sudoku_matrix[i].size(); j++)
            {
                std::cout << sudoku_matrix[i][j] << "    ";
            }
            std::cout << std::endl;
        }
    }


	//clean the matrix and initialize
	std::vector<std::vector <char>> initialize_matrix()
	{
		std::vector<std::vector <char>> sudoku_matrix;
		//clean the matrix
		sudoku_matrix.clear();
		sudoku_matrix.resize(9);
		for (unsigned int i = 0; i < 9; i++)
		{
			sudoku_matrix[i].resize(9);
		}
		return sudoku_matrix;
		
	}

	//convert sudoku from string to sudoko matrix
	std::vector<std::vector <char>> create_sudoku(std::string sudoku)
	{
		unsigned int row = 0;
		unsigned int coloumn = 0;
		//until the string is not finish
	
		std::vector<std::vector <char>> sudoku_matrix = initialize_matrix();
		for(auto c : sudoku)
		{
			
			sudoku_matrix[row][coloumn] = c;
			if (coloumn == 8)
			{
				coloumn = 0;
				row++;
			}
			else 
			{
				coloumn++;
			}
		}
		return sudoku_matrix;
		
	}

	
	

	//read the input file
	std::vector<std::tuple<std::vector<std::vector <char>>, std::vector<std::vector <char>>, float>> read_file()
	{
		std::fstream my_file;
		my_file.open("db.csv", std::ios::in);
		//we save all the instance of dataset
		std::vector<std::tuple<std::vector<std::vector <char>>, std::vector<std::vector <char>>, float>> vector_tuples;
		if (!my_file) {
			std::cout << "File not created!";
		}
		else {
			std::string line;
			//read line by line
			
			int i = 0;
			while (std::getline(my_file, line)) {
				if (i > 0)
				{
					
					
					std::istringstream input2;
					input2.str(line);
					// use separator to read parts of the line
					int skip = 0;
					std::vector<std::vector<std::vector <char>>> sudoku_vector;
					
					for (std::string line; std::getline(input2, line, ','); ) {
						
						//the subset of line that i need
						if (skip != 0 && skip != 3)
						{
							if (skip == 1 || skip == 2)
							{
								std::vector<std::vector <char>> sudoku = create_sudoku(line);
								sudoku_vector.push_back(sudoku);
							}
							else
							{
								//tupla where we save sudoku to solve sudoku sulution and difficulty 
								float d = std::stof(line);
								std::tuple<std::vector<std::vector <char>>, std::vector<std::vector <char>>, float> tupla(sudoku_vector[0], sudoku_vector[1], std::stof(line));
								vector_tuples.push_back(tupla);
							}
						}
						skip++;
					}
				
					
				}
				i++;
				//if we want to reduce the input to read
				if (i == 100000 )
				{
					std::cout << "File created successfully!";
					my_file.close();
					break;
				}
			}
			std::cout << "File created successfully!";
			my_file.close();
		}
		return vector_tuples;

	}
	

	//check row constraint
	bool check_row(std::vector<std::vector <char>>& sudoku_matrix, unsigned short int row)
	{
		std::vector<int> occoronce(9, 0);
		for (unsigned int j = 0; j < sudoku_matrix.size(); j++)
		{
			//second occurence of that number
			char c = sudoku_matrix[row][j];
			int here = sudoku_matrix[row][j] - 48 - 1;
			if (occoronce[(sudoku_matrix[row][j]  -48) - 1] == 1)
			{
				return false;
			}
			occoronce[(sudoku_matrix[row][j]  -48) - 1] += 1;
		}
		return true;
	}


	//check colomn constraint
	bool check_col(std::vector<std::vector <char>>& sudoku_matrix, unsigned short int col)
	{
		std::vector<int> occoronce(9, 0);
		for (unsigned int i = 0; i < sudoku_matrix.size(); i++)
		{
			//second occurence of that number
			if (occoronce[(sudoku_matrix[i][col]  -48) - 1] == 1)
			{
				return false;
			}
			occoronce[(sudoku_matrix[i][col]  -48) - 1] += 1;
		}
		return true;

	}

	//check sqaure consatrint 
	bool check_square(std::vector<std::vector <char>>& sudoku_matrix,unsigned short int row, unsigned short int coloumn)
	{
		unsigned short int start_row = row - (row % 3);
		unsigned short int start_coloumn = coloumn - (coloumn % 3);
		std::vector<int> occoronce(9, 0);
		for (unsigned short int i = 0; i < 3; i++)
		{
			for (unsigned short int j = 0; j < 3; j++)
			{
				//second occurence of that number
				int x = i + start_row;
				int y = j + start_coloumn;
				int elem = sudoku_matrix[i + start_row][j + start_coloumn] -48 -1 ;//va fuori
				if (occoronce[(sudoku_matrix[i + start_row][j+start_coloumn] -48) - 1] == 1)
				{
					return false;
				}
				occoronce[(sudoku_matrix[i + start_row][j + start_coloumn] -48) - 1] += 1;
				
			}
		}
		return true;
	}

	//utility function used to controll the correctnees of sudoku
	bool is_correct(std::vector<std::vector <char>>& sudoku_matrix)
	{
		bool row;
		bool col;
		bool square;
		for (unsigned int i = 0; i < sudoku_matrix.size(); i++)
		{
			row = check_row(sudoku_matrix, i);
			col = check_col(sudoku_matrix, i);
			if (!row || !col)
				return false;
		}
		for (unsigned int i= 0;  i < sudoku_matrix.size(); i+=3)
		{
			for (unsigned int j = 0; j< sudoku_matrix.size() ; j+=3)
			{
				square = check_square(sudoku_matrix, i, j);
				if (!square)
					return false;

			}
			
		}


		return true;
	}









private:
	std::fstream my_file;
	std::vector<std::vector <char>> sudoku_matrix;



};


//calss used to write the report file 
class write_report_sudoku {
public:
	write_report_sudoku(std::string path, std::vector<std::string> columns) : report(path) {
		// initialize the columns
		for (auto st : columns) {
			report << st << ";";
		}
		report << "\n";
	}

	void write_line(std::vector<std::string> line) {
		for (auto st : line) {
			report << st << ";";
		}
		report << "\n";
	}

	void close() {
		report.close();
	}


private:
	std::ofstream report;
};






#endif