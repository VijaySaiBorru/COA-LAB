#include<bits/stdc++.h>
using namespace std;

map<string,string> converter={
    //R-Type
    {"ADD","R0000000rs2rs1000rd0110011"},
    {"SUB","R0100000rs2rs1000rd0110011"},
    {"XOR","R0000000rs2rs1100rd0110011"},
    {"OR","R0000000rs2rs1110rd0110011"},
    {"AND","R0000000rs2rs1111rd0110011"},
    {"SLL","R0000000rs2rs1001rd0110011"},
    {"SRL","R0000000rs2rs1101rd0110011"},
    {"SRA","R0100000rs2rs1101rd0110011"},
    {"SLT","R0000000rs2rs1010rd0110011"},
    {"SLTU","R0000000rs2rs1011rd0110011"},
    //I-Type
    {"ADDI","Iimmrs1000rd0010011"},
    {"XORI","Iimmrs1100rd0010011"},
    {"ORI","Iimmrs1110rd0010011"},
    {"ANDI","Iimmrs1111rd0010011"},
    {"SLTI","Iimmrs1010rd0010011"},
    {"SLTIU","Iimmrs1011rd0010011"},
    {"SLLI","Himmrs1001rd0010011"},
    {"SRLI","Himmrs1101rd0010011"},
    {"SRAI","Himmrs1101rd0010011"},
    //LOAD
    {"LB","Limmrs1000rd0000011"},
    {"LH","Limmrs1001rd0000011"},
    {"LW","Limmrs1010rd0000011"},
    {"LBU","Limmrs1100rd0000011"},
    {"LHU","Limmrs1011rd0010011"},
    //STORE
    {"SB","Simm2rs2rs1000imm10100011"},
    {"SH","Simm2rs2rs1001imm10100011"},
    {"SW","Simm2rs2rs1010imm10100011"},
    //BRANCH
    {"BEQ","Bimm2rs2rs1000imm11100011"},
    {"BNE","Bimm2rs2rs1001imm11100011"},
    {"BLT","Bimm2rs2rs1100imm11100011"},
    {"BGE","Bimm2rs2rs1101imm11100011"},
    {"BLTU","Bimm2rs2rs1110imm11100011"},
    {"BGEU","Bimm2rs2rs1111imm11100011"},
    //JUMP
    {"JAL","Jimmrd1101111"},
    {"JALR","Jimmrs1000rd1100111"},
     //U-TYPE
    {"LUI","Uimm1rd0110111"},
    {"AUIPC","Uimm1rd0010111"},
    // ecall i type system calls
    {"ECALL","K00000000000000000000000001110011"},
    {"EBREAK","E00000000000100000000000001110011"},
    // extra
    {"MUL","R0000001rs2rs1000rd0110011"},
    {"MULH","R0000001rs2rs1001rd0110011"},
    {"MULSU","R0000001rs2rs1010rd0110011"},
    {"MULU","R0000001rs2rs1011rd0110011"},
    {"DIV","R0000001rs2rs1100rd0110011"},
    {"DIVU","R0000001rs2rs1101rd0110011"},
    {"REM","R0000001rs2rs1110rd0110011"},
    {"REMU","R0000001rs2rs1111rd0110011"}
};
string toUpperCase(string str){
    transform(str.begin(),str.end(),str.begin(),::toupper);
    return str;
}
string assembler(string line,int linenumber,map<string,pair<int,int>>labels){
    string temp="";
    int len=line.length();
    vector<string> format(3);
    string word="";
    int followup=0;
    for(char i : line){
        if(i==' '){
            followup++;
            break;
        }
        else{
            word.push_back(i);
            followup++;
        }
    }
    word=toUpperCase(word);
    temp=converter[word];
    if(converter.find(word)==converter.end()){
        cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
        return "";
    }
    else{
    if(word=="JALR"){
         int i=0;
        while(followup<len){
            if(line[followup]==')'){
            break;
            }
            else if(line[followup]==',' || line[followup]=='('){
                i++;
                followup++;
            }
           
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[2]=format[2].substr(1,format[2].length()-1);
        if(format[1][0]=='x'){
              cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
             return "";
        }
        string rs1 = bitset<5>(stoi(format[2])).to_string();
        string imm = bitset<12>(stoi(format[1])).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        string ans="";
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7)+"\n";
        return ans;
    }
    if(temp[0]=='R'){
        int i=0;
        while(followup<len){
            if(line[followup]=='\n'){
            break;
            }
            else if(line[followup]==','){
                i++;
                followup++;
            }
        
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[1]=format[1].substr(1,format[1].length()-1);
        format[2]=format[2].substr(1,format[2].length()-1);
        string rs1 = bitset<5>(stoi(format[1])).to_string();
        string rs2 = bitset<5>(stoi(format[2])).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        string ans="";
        ans += temp.substr(1,7)+rs2+rs1+temp.substr(14,3)+rd+temp.substr(19,7)+"\n";
        return ans;
    }
    else if(temp[0]=='I'){
        int i=0;
        while(followup<len){
            if(line[followup]=='\n'){
            break;
            }
            else if(line[followup]==','){
                i++;
                followup++;
            }
        
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[1]=format[1].substr(1,format[1].length()-1);
        if(format[2][0]=='x'){
            cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
            return "";
        }
        string rs1 = bitset<5>(stoi(format[1])).to_string();
        string imm = bitset<12>(stoi(format[2])).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        string ans="";
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7)+"\n";
        return ans;
    }
    else if(temp[0]=='H'){
        int i=0;
        while(followup<len){
            if(line[followup]=='\n'){
            break;
            }
            else if(line[followup]==','){
                i++;
                followup++;
            }
        
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[1]=format[1].substr(1,format[1].length()-1);
        if(format[2][0]=='x'){
              cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
             return "";
        }
        string rs1 = bitset<5>(stoi(format[1])).to_string();
        string imm = bitset<5>(stoi(format[2])).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        if(word == "SRAI"){
          imm = "0100000" + imm;
        }
        else{
          imm = "0000000" + imm;
        }
        string ans="";
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7)+"\n";
        return ans;
    }
    else if(temp[0]=='L'){
        int i=0;
        while(followup<len){
            if(line[followup]==')'){
            break;
            }
            else if(line[followup]==',' || line[followup]=='('){
                i++;
                followup++;
            }
           
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[2]=format[2].substr(1,format[2].length()-1);
        if(format[1][0]=='x'){
              cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
             return "";
        }
        string rs1 = bitset<5>(stoi(format[2])).to_string();
        string imm = bitset<12>(stoi(format[1])).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        string ans="";
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7)+"\n";
        return ans;
    }
    else if(temp[0]=='S'){
        int i=0;
        while(followup<len){
            if(line[followup]==')'){
            break;
            }
            else if(line[followup]==',' || line[followup]=='('){
                i++;
                followup++;
            }
           
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[2]=format[2].substr(1,format[2].length()-1);
        if(format[1][0]=='x'){
              cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
             return "";
        }
        string rs1 = bitset<5>(stoi(format[2])).to_string();
        string rs2 = bitset<5>(stoi(format[0])).to_string();
        string imm = bitset<12>(stoi(format[1])).to_string();
        string ans="";
        ans += imm.substr(0,7)+rs2+rs1+temp.substr(11,3)+imm.substr(7,5)+temp.substr(18,7)+"\n";
        return ans;
    }
    else if(temp[0]=='B'){
        int i=0;
        while(followup<len){
            if(line[followup]=='\n'){
            break;
            }
            else if(line[followup]==','){
                i++;
                followup++;
            }
           
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        format[1]=format[1].substr(1,format[1].length()-1);
        int offset;
        if(format[2][0]<'0' || format[2][0]>'9'){
            int instruction_diff = labels[format[2]].first-labels[format[2]].second - linenumber;
            if(instruction_diff<0){
                instruction_diff+=1;
            }
            offset = instruction_diff * 4;
        }
        else{
            offset=stoi(format[2]);
        }
        // bool isLabel = false, isNum = false;
        // int check = 0;
        
        // // Check if the third part is a label or an immediate number
        // if(labels.find(format[2]) != labels.end()) {
        //     isLabel = true;
        //     check = labels[format[2]];  // Address of the label
        // } else {
        //     try {
        //         check = stoi(format[2]);
        //         isNum = true;
        //     } catch (...) {
        //         isNum = false;
        //     }
        // }

        // // Error handling for undefined label or immediate
        // if(!isLabel && !isNum) {
        //     cout << "Error: Undefined label or immediate '" << format[2] << "' at line " << linenumber << endl;
        //     return "";
        // }
        // int instruction_diff = check - linenumber;
        // int offset = instruction_diff * 4;
        string rs1 = bitset<5>(stoi(format[0])).to_string();
        string rs2 = bitset<5>(stoi(format[1])).to_string();
        string imm = bitset<13>(offset).to_string();
        string ans="";
        ans += imm[0]+imm.substr(2,6)+rs2+rs1+temp.substr(11,3)+imm.substr(8,4)+imm[1]+temp.substr(18,7)+"\n";
        return ans;
    }
    else if(temp[0]=='U'){
        int i=0;
        while(followup<len){
            if(line[followup]=='\n'){
            break;
            }
            else if(line[followup]==','){
                i++;
                followup++;
            }
           
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
         if(format[1][0]=='x'){
              cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
             return "";
        }
        string imm = bitset<20>(stoi(format[1])).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        string ans="";
        ans += imm+rd+temp.substr(7,7)+"\n";
        return ans;
    }
    else if(temp[0]=='J'){
        int i=0;
        while(followup<len){
            if(line[followup]=='\n'){
            break;
            }
            else if(line[followup]==','){
                i++;
                followup++;
            }
           
               format[i]+=line[followup];
            followup++;
        }
        format[0]=format[0].substr(1,format[0].length()-1);
        int offset;
        if(format[1][0]<'0' || format[1][0]>'9'){
            int instruction_diff = labels[format[1]].first-labels[format[2]].second - linenumber;
            if(instruction_diff<0){
                instruction_diff+=1;
            }
            offset = instruction_diff * 4;
        }
        else if(format[1][0]=='x' || stoi(format[1])%2 != 0){
              cout << "Error: No Such type of instruction found in the given instruction set." << " At line number " << linenumber << endl;
             return "";
        }
        else{
            offset=stoi(format[1]);
        }
        string imm = bitset<21>(offset).to_string();
        string rd = bitset<5>(stoi(format[0])).to_string();
        string ans="";
        ans += imm.substr(0,1)+imm.substr(10,10)+imm.substr(9,1)+imm.substr(1,8)+rd+temp.substr(6,7)+"\n";
        return ans;
    }
     else if(temp[0]=='K' || temp[0]=='E'){
        string ans="";
        ans=temp.substr(1,temp.length()-1);
        return ans;
    }
    }
}


int main(){
    /*ifstream inputFile("input.txt");
    ofstream outfile("output.txt");
    if(!inputFile.is_open()){
        cout<<"Error in opening file!"<<endl;
        return 1;
    }
    if(!outfile.is_open()){
        cout<<"Error in opening file for writing!"<<endl;
        return 1;
    }
    string line;
    while(getline(inputFile,line)){
        string change=line;
        outfile<<change<<endl;
    }
    inputFile.close();
    outfile.close();*/
    int n;
    cout << "Input the number of lines of assembly code: ";
    cin >> n;
    cin.ignore(); 
    string s; 
    vector<string> instruction_set;
    cout << "Give the instruction set  (use labels with a colon, e.g., .loop: ; ADD x1,x2,x3 ; addi x1,x1,5 ):\n";
    for (int i = 0; i < n; i++) {
        getline(cin, s);
        instruction_set.push_back(s);
    }
    map<string, pair<int,int>> labels;
    for (int i = 0; i < n; i++) {
        if (!instruction_set[i].empty() && instruction_set[i].back() == ':') {
            string func_name = instruction_set[i].substr(1, instruction_set[i].size() - 2);
            int use=labels.size();
            labels[func_name].first = i+1;
            labels[func_name].second = use;
        }
    }
    for (const auto& label : labels) {
        cout << "Label: " << label.first 
             << ", Line: " << label.second.first 
             << ", Previous labels count: " << label.second.second 
             << endl;
    }
    cout << "The converted assembly instructions to machine language code:\n";
    for (int i = 0; i < n; i++) {
        string ans;
        ans = assembler(instruction_set[i],i+1,labels);
        if(!ans.empty()){
            cout << ans;
        }
    }
    return 0;
}