#include <iostream>
#include <fstream>
#include <string>

int shape_verilog(std::string input_name, std::string patch_name) {
  std::cout << "begin shaping patch verilog" << std::endl;
  std::ifstream reading;
  reading.open(input_name, std::ios::in);
  std::ofstream patch_writing;
  patch_writing.open(patch_name, std::ios::out);

  std::string str;
  while(getline(reading, str)) {
    int y_address;
    int assign_flag = -1;
    if ((y_address = str.find("Y")) != -1) {
      int address = 0;
      while ((str[address] < '0' || str[address] > '9') && str[address] != 'g') {
	if(str[address] >= 'A' && str[address] <= 'Z') {
	  if(str[address] == 'B') {
	    patch_writing << "buf";
	    address += 3;
	  }
	  else if(str[address] == 'Z') {
	    assign_flag = 0;
	    break;
	  }
	  else if(str[address] == 'C') {
	    assign_flag = 1;
	    break;
	  }	  
	  else {
	    patch_writing << char(str[address] + 'a' - 'A');
	  }
	}
	else {
	  patch_writing << str[address];
	}
	address++;
      }

      if(assign_flag == -1) {
	patch_writing << " (";
	int y_writing_address = y_address+2;
	while(str[y_writing_address] != ')') {
	  patch_writing << str[y_writing_address];
	  y_writing_address++;
	}

	while(str[address] != '.') address++;
	while(1) {
	  while(str[address] != '(') address++;
	  if(address >= y_address) break;
	  patch_writing << ", ";
	  address++;
	  while(str[address] != ')') {
	    patch_writing << str[address];
	    address++;
	  }
	}
	patch_writing << ");" << std::endl;
      }
      else {
	patch_writing << "assign ";
	int y_writing_address = y_address+2;
	while(str[y_writing_address] != ')') {
	  patch_writing << str[y_writing_address];
	  y_writing_address++;
	}
	patch_writing << " = " << assign_flag << ";" << std::endl;
      }
    }
    else {
      patch_writing << str << std::endl;
    }
  }
  std::cout << "patch verilog has been shaped" << std::endl;
  return 0;
}
