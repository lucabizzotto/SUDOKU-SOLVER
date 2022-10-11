
#include"Csp.h"
#include"Relaxation_labeling.h"
#include"Read.h"



int main()
{

	//object we use to ride file 
	Read file = Read();

	//here we saved all the instances 
	std::vector<std::tuple<std::vector<std::vector <char>>, std::vector<std::vector <char>>, float>> instances_data_set;

	//to read the file 
	instances_data_set = file.read_file();
	
	

	//creation file csv
	write_report_sudoku write( "report.csv", {"tecnique","iterations","backtracking","correct","difficulty"});

	//first tecnique Csp
	Solver s = Solver('.');

	//Realaxarion labelling
	Relaxation_labeling l = Relaxation_labeling('.');

	//used as a stopper
	int counter = 0;
	
	
	while (instances_data_set.size() > 0 && counter < 1)
	{
	
		std::tuple<std::vector<std::vector <char>>, std::vector<std::vector <char>>, float> tupla = (instances_data_set.back());
		
		instances_data_set.pop_back();
		float difficulty = std::get<2>(tupla);

		std::vector<std::vector <char>> sudoku = std::get<0>(tupla);

		//csp---------------------------------
		
		std::cout << "-" << counter;
		counter++;
			
		s.insert_sudoku(sudoku);
		std::vector<std::vector <char>> solved = s.start();
		bool answer = file.is_correct(solved);
		int iterations = s.get_iteration();
		int backtrack = s.get_backtrack();
		write.write_line({ "1",std::to_string(iterations),std::to_string(backtrack),std::to_string(answer),std::to_string(difficulty) });
			
			
		//relaxation labelling
		l.insert_sudoku(sudoku);
		solved = l.start();
		answer = file.is_correct(solved);
		iterations = l.get_iterations();
		write.write_line({ "2",std::to_string(iterations),std::to_string(answer),std::to_string(difficulty) });

	}


	return 0;
}