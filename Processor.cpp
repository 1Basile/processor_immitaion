#include "Processor.h"
#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <bitset>
#include <regex>
#include <cctype>

// #define DEBUG_MODE // Never use together ^$^
#define TABLE_MODE

class Syntax_error : public std::exception {  };

void erase_substr(string & main_str, const string & to_erase) {
    size_t pos = main_str.find(to_erase);
    if (pos != string::npos) { main_str.erase(pos, to_erase.length()); }
}

void clear_screen() {
    // CSI[2J clears screen, CSI[H moves the cursor to top-left corner
    cout << "\x1B[2J\x1B[H";
}

string ltrim(const string& s) {
	return regex_replace(s, regex("^\\s+"), string(""));
}

string rtrim(const string& s) {
	return regex_replace(s, regex("\\s+$"), string(""));
}

string trim(const string& s) {
	return ltrim(rtrim(s));
}

Processor::Processor() {
  this->PS = register_(0); 
}

void Processor::parse_file(ifstream& file, const string& file_name="") {
    string file_line;
    this->__file_parsed_name = file_name;
    #ifdef DEBUG_MODE
    cout << "Start Execution" << endl;
    cout << "---------------\n" << endl;
    #endif /* DEBUG_MODE */

    while (getline(file, file_line)) {
        try {
            if (file_line == "") { continue; }
            this->read_command(file_line);
            this->do_command();

        } catch (std::out_of_range) {
          #ifndef DEBUG_MODE
          this->ERR_R = "out of range";
          this->TC = register_(0); // Reset ticks
          this->__tick();
          #endif

          #ifdef DEBUG_MODE
          cout << "\tError!!!!!!\n\t";
          cout << "Number is `Out Of Range`" << endl;
          cout << endl;
          #endif

        } catch (Syntax_error) {
          #ifndef DEBUG_MODE
          this->ERR_R = "syntax error";
          this->TC = register_(0); // Reset ticks
          this->__tick();
          #endif

          #ifdef DEBUG_MODE
          cout << "\tError!!!!!!\n\t";
          cout << "Syntax is Wrong" << endl;
          cout << endl;
          #endif
          }
    }

    #ifdef TABLE_MODE
    // Print & Update table
    cout << "// Made by Vasia Chomko, K-21." << endl;
    #endif

    #ifdef DEBUG_MODE
    cout << "------------------" << endl;
    cout << "Execution Finished" << endl;
    #endif
}

void Processor::_xor() {

    this->__tick();

    #ifdef DEBUG_MODE
    cout << "Do XOR opperator" << endl;
    cout << "   AX = " << this->AX << endl;
    cout << "   BX = " << this->BX << endl;
    #endif

    this->AX = (this->AX & ~this->BX) | (~this->AX & this->BX);

    #ifdef DEBUG_MODE
    cout << "   New AX = " << this->AX << endl;
    #endif
}

void Processor::_move_ax(const bitset<22> num) {
    this->AX = num;
}

void Processor::_move_bx(const bitset<22> num) {
    this->BX = num;
}

void Processor::read_command(const string line) {
    // cout << "Reading command:" << endl;
    this->IR = trim(line);

    #ifdef TABLE_MODE
    this->TC = register_(0); // Reset ticks
    this->PC = register_(this->PC.to_ulong() + 1);  // Add 1 to commands executing counter
    #endif

    #ifdef DEBUG_MODE
    cout << "   //"<< this->IR << "//" << endl;
    #endif
}

void Processor::move(string __str_first_operand, string __str_second_operand) {

    this->__tick();

    transform(__str_second_operand.begin(), __str_second_operand.end(), __str_second_operand.begin(), ::toupper);
    transform(__str_first_operand.begin(), __str_first_operand.end(), __str_first_operand.begin(), ::toupper);

    if ((__str_first_operand) == "BX" && (__str_second_operand) == "AX") { 
        this->_move_bx(this->AX);
        #ifdef DEBUG_MODE
        cout << "Moving AX to BX"<< endl;
        cout << "   BX = " << this->BX << endl;
        cout << "   AX = " << this->AX << endl;
        #endif
    }

     else if ((__str_first_operand) == "AX" && (__str_second_operand) == "BX") {
        this->_move_ax(this->BX);
        #ifdef DEBUG_MODE
        cout << "Moving BX to AX"<< endl;
        cout << "   AX = " << this->AX << endl;
        cout << "   BX = " << this->BX << endl;
        #endif
    }

    else if ((__str_first_operand) == "AX" && (__str_second_operand) == "AX") { 
      #ifdef DEBUG_MODE
      cout << "Moving AX to AX (doing nothing)" << endl; 
      #endif
    }

    else if ((__str_first_operand) == "BX" && (__str_second_operand) == "BX") {
      #ifdef DEBUG_MODE
      cout << "Moving BX to BX (doing nothing)" << endl;
      #endif
    }

    else if ((__str_first_operand) == "AX") {
          #ifdef DEBUG_MODE
          cout << "Moving " << __str_second_operand << " to AX"<< endl;
          #endif
    

          if ( __str_second_operand.compare(0, 2, "0X") == 0 && __str_second_operand.size() > 2 && __str_second_operand.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos) { // Is hex num
              this->PS = register_(0);
              this->_move_ax(register_(stoi(__str_second_operand, 0, 16)));
          }
          else if (!__str_second_operand.empty() && all_of(__str_second_operand.begin()+1, __str_second_operand.end(), ::isdigit) && (__str_second_operand.at(0) == '-' | isdigit(__str_second_operand[0]))) {
              this->PS = register_((__str_second_operand.at(0) == '-') ? 1: 0);
              this->_move_ax(register_(stoi(__str_second_operand, 0, 10) - ((__str_second_operand.at(0) == '-') ? 1: 0))); // Is decimal num
          }
          else { throw Syntax_error(); } 

    }

    else if ((__str_first_operand) == "BX") {
         #ifdef DEBUG_MODE
         cout << "Moving " << __str_second_operand << " to BX"<< endl;
         #endif
    

          if ( __str_second_operand.compare(0, 2, "0X") == 0 && __str_second_operand.size() > 2 && __str_second_operand.find_first_not_of("0123456789abcdefABCDEF", 2) == std::string::npos) { // Is hex num
              this->PS = register_(0);
              this->_move_bx(register_(stoi(__str_second_operand, 0, 16)));
          }

          else if (!__str_second_operand.empty() && all_of(__str_second_operand.begin()+1, __str_second_operand.end(), ::isdigit) && (__str_second_operand.at(0) == '-' | isdigit(__str_second_operand[0]))) {
              this->PS = register_((__str_second_operand.at(0) == '-') ? 1: 0);
              this->_move_bx(register_(stoi(__str_second_operand, 0, 10) - ((__str_second_operand.at(0) == '-') ? 1: 0))); // Is decimal num
          }
          else { throw Syntax_error(); } 

      }

    else { throw Syntax_error(); }

    #ifdef DEBUG_MODE
    cout << endl;
    #endif

}

void Processor::do_command() {
    string command = this->IR;
    string __str_first_operand;
    string __str_second_operand;
    this->CR = 0xffffff;

    for (int i=0; i < sizeof(this->commands_segments_table)/sizeof(this->commands_segments_table[0]); i++) {

          if (command.rfind(this->commands_segments_table[i], 0) == 0) {
              this->CR = i;
              erase_substr(command, (this->commands_segments_table[i]));
              break; }

    }

    switch(this->CR.to_ulong()) {

      case 0: // move in ax or in bx;
          __str_first_operand = command.substr(0, command.find(string(",")));
          command.erase(0, command.find(string(",")) + 1);
          __str_second_operand = command;

          this->move(__str_first_operand, __str_second_operand);

          break;

      case 1: // xor ax, bx
          __str_first_operand = command.substr(0, command.find(string(",")));
          command.erase(0, command.find(string(",")) + 1);
          __str_second_operand = command;

          this->move("ax", __str_first_operand);
          this->move("bx", __str_second_operand);
          this->_xor();

          break;

      default:
          throw Syntax_error();

          break;

    }
}

void Processor::__tick() {
    #ifdef TABLE_MODE

    this->TC = register_(this->TC.to_ulong() + 1);  //  Add tick to command ticks
    // clear_screen();
    // Draw header
    // if (this->__file_parsed_name != "") {
    //     cout << "File parsed: " << this->__file_parsed_name << "\n" << endl; }
    cout << "<-------------->" << endl;
    // Print & Update table
    cout << "IR:  " << this->IR << endl;
    cout << "AX:  " << this->AX << endl;
    cout << "BX:  " << this->BX << endl;
 
    cout << endl;

    if (this->ERR_R != "") {
      cout << "ERR_R:  " << this->ERR_R << "\n" << endl;
      this->ERR_R = "";
    }

    cout << "PS:  " << this->PS.to_ulong() << endl;
    cout << "PC:  " << this->PC.to_ulong() << endl;
    cout << "TC:  " << this->TC.to_ulong() << endl;

    cout << "<-------------->" << endl;
    #endif

    #ifndef DEBUG_MODE
    getchar();
    #endif
}

int main(int argc, char* argv[]) {
    Processor proc = Processor();
    ifstream file_to_parce;

    file_to_parce.open(argv[1], ifstream::in);

    #ifdef DEBUG_MODE
    cout << "Parsing file: " << argv[1] << "\n" << endl;
    #endif

    proc.parse_file(file_to_parce, argv[1]);

    file_to_parce.close();

    #ifdef DEBUG_MODE
    cout << "\nFile closed" << endl;
    #endif
}

