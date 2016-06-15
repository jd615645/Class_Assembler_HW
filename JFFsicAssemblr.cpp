// 引入標準程式庫中相關的輸入、輸出程式
#include <iostream>
// 引入標準程式庫中相關的字串程式
#include <iomanip>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <stdio.h> 
#include <stdlib.h>

// std 為標準程式庫的名稱空間
using namespace std;
map<string, string> optable;
map<string, int> address;

// del white space
std::string& trim(std::string &s) {
  if (s.empty()) {
    return s;
  }

  s.erase(0,s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
  return s;
}
// have ,X?
int check_x(string str) {
  if (str.find(",X") != -1) {
    return 1;
  }
  else {
    return 0;
  }
}
// have +?
int check_plus(string str) {
  if (str.find("+") != -1) {  
    return 1;
  }
  else {
    return 0;
  }
}
// have @?
int check_at(string str) {
  if (str.find("@") != -1) {
    return 1;
  }
  else {
    return 0;
  }
}
// have #?
int check_sharp(string str) {
  if (str.find("#") != -1) {
    return 1;
  }
  else {
    return 0;
  }
}
// 確定是否為Base-relative
int in_base(int num, int b) {
  int range = num - b;
  if(0 <= range && range <= 4095) {
    return 1;
  }
  else {
    return 0;
  }
}
// 確定是否為PC-relative
int in_pc(int num, int pc) {
  int range = num - pc;
  if(-2048 <= range && range <= 2047)
    return 1;
  else
    return 0;
}
int dec2bin(int num) {
  int a, b=1, c=0;
  while(num != 0){ 
    a= num % 2;
    num = num / 2; 
    c = c + a * b; 
    b = b * 10; 
  }
  return c;
}
int hex2dec(string num) {
  unsigned int x;   
  std::stringstream ss;
  ss << std::hex << num;
  ss >> x;
  return static_cast<int>(x);
}
string hex2bin(char hex_num) {
  if (hex_num == '0')
    return "0000";
  else if (hex_num == '1') 
    return "0001";
  else if (hex_num == '2')
    return "0010";
  else if (hex_num == '3')
    return "0011";
  else if (hex_num == '4')
    return "0100";
  else if (hex_num == '5')
    return "0101";
  else if (hex_num == '6')
    return "0110";
  else if (hex_num == '7')
    return "0111";
  else if (hex_num == '8')
    return "1000";
  else if (hex_num == '9')
    return "1001";
  else if (hex_num == 'A')
    return "1010";
  else if (hex_num == 'B')
    return "1011";
  else if (hex_num == 'C')
    return "1100";
  else if (hex_num == 'D')
    return "1101";
  else if (hex_num == 'E')
    return "1110";
  else if (hex_num == 'F')
    return "1111";
}

// flag bits
struct nixbpe {
  int n, i, x, b, p, e;
};

int main(void) {
  optable["ADD"] = "18";
  optable["ADDF"] = "58";
  optable["ADDR"] = "90";
  optable["AND"] = "40";
  optable["CLEAR"] = "B4";
  optable["COMP"] = "28";
  optable["COMPF"] = "88";
  optable["COMPR"] = "A0";
  optable["DIV"] = "24";
  optable["DIVF"] = "64";
  optable["DIVR"] = "9C";
  optable["FIX"] = "C4";
  optable["FLOAT"] = "C0";
  optable["HIO"] = "F4";
  optable["J"] = "3C";
  optable["JEQ"] = "30";
  optable["JGT"] = "34";
  optable["JLT"] = "38";
  optable["JSUB"] = "48";
  optable["LDA"] = "00";
  optable["LDB"] = "68";
  optable["LDCH"] = "50";
  optable["LDF"] = "70";
  optable["LDL"] = "08";
  optable["LDS"] = "6C";
  optable["LDT"] = "74";
  optable["LDX"] = "04";
  optable["LPS"] = "D0";
  optable["MUL"] = "20";
  optable["MULF"] = "60";
  optable["MULR"] = "98";
  optable["NORM"] = "C8";
  optable["OR"] = "44";
  optable["RD"] = "D8";
  optable["RMO"] = "AC";
  optable["RSUB"] = "4C";
  optable["SHIFTL"] = "A4";
  optable["SHIFTR"] = "A8";
  optable["SIO"] = "F0";
  optable["SSK"] = "EC";
  optable["STA"] = "0C";
  optable["STB"] = "78";
  optable["STCH"] = "54";
  optable["STF"] = "80";
  optable["STI"] = "D4";
  optable["STL"] = "14";
  optable["STS"] = "7C";
  optable["STSW"] = "E8";
  optable["STT"] = "84";
  optable["STX"] = "10";
  optable["SUB"] = "1C";
  optable["SUBF"] = "5C";
  optable["SUBR"] = "94";
  optable["SVC"] = "B0";
  optable["TD"] = "E0";
  optable["TIO"] = "F8";
  optable["TIX"] = "2C";
  optable["TIXR"] = "B8";
  optable["WD"] = "DC";

  /*
    ln:　input data total line
    src_code: input data
    base: BASE num
    flag: nixbpe flag
    loc: 經計算後的local
    object_code: 經計算後的object_code
  */
  char line[128];
  int ln = 0, loc[10000], base=0;
  struct nixbpe flag[200];
  string src_code[10000][3];
  string object_code[10000]; 

  // init filesystem
  fstream file;
  file.open("input.txt", ios::in);

  if (!file)
    cout << "Open source file failed!" << endl;
 
  while(file.getline(line, sizeof(line), '\n')) {
    // get source statement
    stringstream s(line);
    string sub_str;
    int src_num = 0;

    // flag e
    if(check_plus(line)) {
      flag[ln].e = 1;
    } 
    // flage n i 
    if(check_sharp(line)) {
      flag[ln].n = 0;
      flag[ln].i = 1;
    }
    if(check_at(line)) {
      flag[ln].n = 1;
      flag[ln].i = 0;
    }
    // flag x
    if(check_x(line)) {
      flag[ln].x = 1;
    }

    while(getline(s,sub_str,'|')) {
      src_code[ln][src_num++] = trim(sub_str);
    }
    ln++;
  }
  ln--;
  // start local space
  loc[0] = loc[1] = hex2dec(src_code[0][2]);

  for (int i= 1; i <= ln; i++) {
    int format = 3;
    string label = src_code[i][0];
    string code1 = src_code[i][1];
    string code1_op = optable[code1];
    string code2 = src_code[i][2];
    string opcode = hex2bin(code1_op[0]) + hex2bin(code1_op[1]).substr(0,1);
    
    // if flag e is 1 => format 4
    if(flag[i].e == 1) {
      format = 4;
    }
    // 處理RESB的位址計算
    if (code1 == "RESB") {
      loc[i+1] = loc[i] + std::stoi(code2);
    }
    // 處理BYTE的位址計算
    else if (code1 == "BYTE") {
      if(code2 == "C'EOF'") {
        loc[i+1] = loc[i] + 3;
      }
      else {
        loc[i+1] = loc[i] + 1;
      }
    }
    // 計算位址（以input.text的內容來看基本上都是format3
    else {
      loc[i+1] = loc[i] + format;
    }

    if (label != "") {
      address[label] = loc[i];
    }
    
  }

  // pass 2 get object_code
  for (int i = 0; i<= ln; i++) {
    string label = src_code[i][0];
    string code1 = src_code[i][1];
    string code1_op = optable[code1];
    string code2 = src_code[i][2];

    if (code1 == "BYTE") {
      if(code2 == "C'EOF'") {
        object_code[i] = "454F46";
      }
      else {
        object_code[i] = "0000" + code2.substr(2,2); 
      }
    }
    else if (code1 == "WORD") {
      std::string addzero(6 - code2.size(), '0');
      object_code[i] = addzero + code2;
    }
    else if (code1 == "RSUB") {
      std::string addzero(6 - code1_op.size(), '0');
      object_code[i] = code1_op + addzero;
    }
    else if (code1 == "RESW" || code1 == "RESB" || code1 == "STAR" || code1 == "END") {
      object_code[i] = "";
    }
    else {
      std::stringstream stream;
      stream << std::hex << address[code2];
      std::string result(stream.str());
      object_code[i] = code1_op + result;
    }
    // 確定是PC-relative定址 or Base-relative定址
    if(in_pc(hex2dec(object_code[i]), loc[i+1])) {
      flag[i].p = 1;
    }
    else {
      if(in_base(hex2dec(object_code[i]), base)) {
        flag[i].b = 1;
      }
    }
    // print sol
    std::cout << std::hex << loc[i] << setw(10) << label << setw(10) << code1 << setw(10) << code2 << setw(10) << object_code[i] << std::endl;
  }

  // close file
  file.close();

  return 0;
}