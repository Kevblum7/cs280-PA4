//  Kevin Blum
//  Student ID: 31542988
//
//  ProgramAssignment4
//
 /*  Building Interpreter for the simple programming language
  * for the simple programming language based on the recursive-descent parser */
//
//  Main Driver
//
//  Calls Parser/Interpreter
//  Outputs successful or uncessful parsing/interpretation number of errors

#include <iostream>
#include <fstream>
#include <sstream>
#include "parseRun.cpp"


int main(int argc, const char * argv[]) {
    

    ifstream file;
    int linenum = 0;

    // if a file was entered
    if( argc > 1)
    {
        // Making sure only one file entered
        if(argc > 2)
        {
            cout << "ONLY ONE FILENAME ALLOWED" << endl;
        }
        else
        {
            file.open(argv[1]);
            if(file.is_open())
            {
                istream& in = file;
                bool ans = Prog(in, linenum);
    
                if (ans == true)
                {
                    cout << endl <<  "Successful Execution" << endl;
                }
                else
                {
                    cout << endl << "Unsuccessful Interpretation" << endl;
                    cout << "Number of Errors: " << error_count << endl;
                }
            }
            // If file was not recognized or cannot be opened.
            else
            {
                cout << "CANNOT OPEN THE FILE {" << argv[1] << "}" << endl;
                
            }
            file.close();
        }
    }
    else
    {
        cout << "NO FILE ENTERED" << endl;
    }
        
    
    return 0;
}

