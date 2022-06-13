#include "triglav.h"
#include <sstream>
#include <algorithm>
#include <fstream>
#include <limits>
#include <sys/wait.h>
#include <unistd.h>

using namespace triglav;
//#define TRIGLAV_DEBUG

#ifdef TRIGLAV_DEBUG
#define DEBUG(X) std::cout << __FILE__ << " " << __LINE__ << ":" << X << "\n"
#else
#define DEBUG(X)
#endif


agent::my_output_multifile::output_file::output_file(const std::string & path, const std::string & prefix, unsigned n, const std::string & extension, int proc_id): number{n}, processor_id{proc_id}
{
    std::stringstream filename_stream;
    
    filename_stream << path << "/" << prefix << processor_id << "_" << number << "." << extension;
        
    std::cout << "opening file " << filename_stream.str() << "\n";
    
    output_file_stream.open(filename_stream.str(), std::ofstream::out);
    
    if (!output_file_stream.good())
    {
        throw std::runtime_error("unable to open file cases_<...>.txt");
    }
}
            
agent::my_output_multifile::output_file::~output_file()
{
    output_file_stream.close();
}

std::ofstream & agent::my_output_multifile::get_random_output_file_stream()
{
    unsigned i = dist(rng)-1;
    
    return vector_of_output_files[i]->get_stream();
}


agent::my_output_multifile::my_output_multifile(const std::string & path, const std::string & prefix, unsigned amount_of_files, const std::string & extension, int proc_id): 
    rng{dev()}, dist(1, amount_of_files), processor_id{proc_id}
{
    if (amount_of_files == 0)
    {
        throw std::runtime_error("amount of output files must not be 0");
    }
    for (unsigned i{0}; i<amount_of_files; i++)
    {
        vector_of_output_files.push_back(std::make_shared<output_file>(path, prefix, i, extension, processor_id));
    }
}

