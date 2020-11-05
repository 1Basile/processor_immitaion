#pragma once
#include <string>
#include <bitset>
#include <exception>
#include <stdexcept>

using namespace std;

typedef bitset<22> register_;

class Processor
{
	public:
			Processor();
			void parse_file(ifstream&, const string&); // parsing file obj, and file_name

	private:
			register_ AX, BX, PC, TC, RS, CR, PS;
			string commands_segments_table[2] = {"mov ", "xor "};
			string IR, ERR_R; // Text register_, that save command text 
			string __file_parsed_name;

			void _xor();
			void move(string, string);
			void _move_ax(bitset<22>);
			void _move_bx(bitset<22>);
			void read_command(string); 
			void do_command(); // get from register_s

			void __tick();
};
