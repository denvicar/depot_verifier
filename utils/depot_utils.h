#pragma once
#include <boost/program_options.hpp>
#include <string>
#include <unordered_map>

using std::string;
using std::unordered_map;

void ltrim(string&);

void rtrim(string&);

unordered_map<string, string> read_config();

string GetHelpMessage(boost::program_options::options_description opt);

boost::program_options::variables_map InitParse(int ac, char* av[],
                                                string& msg);