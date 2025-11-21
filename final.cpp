#include <bits/stdc++.h>
using namespace std;
int check=0;
int stalls=0;
int flushed=0;
int instructions_executed=0;
vector<string> instructionMemory;

//         5-Stage Pipeline CPU      //
struct ControlWord{
    int RegWrite;
    int MemWrite;
    int MemRead;
    int Branch;
    int Jump;
    int Mem2Reg;
    int AluSrc;
    int AluOp;
    int RegRead;
    int JumpBack;
    ControlWord() : RegWrite(0), MemWrite(0), MemRead(0), Branch(0), Jump(0), Mem2Reg(0), AluSrc(0), AluOp(0), RegRead(0), JumpBack(0) {}
};
class PC{
    public:
        int value;
        bool valid;
        PC(){
            value=0;
            valid=false;
        }
};
class IFID{
    public:
        int NPC;
        int DPC;
        string IR;
        bool stall;
        bool valid;

        IFID(){
           NPC=0;
           DPC=0;
           IR="00000000000000000000000000000000";
           stall=false;
           valid=false;
        }
};
class IDEX{
    public:
        int JPC;
        int DPC;
        string imm_alu;
        string func;
        int imm;
        ControlWord CW;
        int rs1;
        int rs2;
        int rs2_val;
        int rdl;
        bool stall;
        bool valid;

        IDEX(){
            JPC=0;
            DPC=0;
            imm_alu="0000000";
            func="000";
            imm=0;
            rs1=0;
            rs2=0;
            rs2_val=0;
            rdl=0;
            stall=false;
             valid=false;
        }
};
class EXMO{
    public:
        int DPC;
        ControlWord CW;
        int ALU_result;
        int rs2;
        int rdl;
        int tpc;
        bool stall;
        bool valid;
        EXMO(){
            DPC=0;
            ALU_result=0;
            rs2=0;
            rdl=0;
            tpc=0;
            stall=false;
            valid=false;
        }
};
class MOWB{
    public:
        int DPC;
        ControlWord CW;
        int ALU_result;
        int LD_out;
        int rdl;
        bool stall;
        bool valid;
        MOWB(){
            DPC=0;
            ALU_result=0;
            LD_out=0;
            rdl=0;
            stall=false;
            valid=false;
        }
};
class RegisterFile {
    private:
    vector<int> GPR = vector<int>(32, 0);
    public:
    vector<int> GPR_valid = vector<int>(32, 0);
    int read(int reg) {
        if (reg < 0 || reg >= 32) {
            cerr << "Error: Invalid register access!" << endl;
            return 0;
        }
        return GPR[reg];
    }
    void write(int reg, int value) {
        if (reg < 0 || reg >= 32) {
            cerr << "Error: Invalid register access!" << endl;
            return;
        }
        if (reg != 0) { 
            GPR[reg] = value;
        }
    }
    void printRegisterFile() {
        cout << "Register File:\n";
        cout << setw(14) << "Register" << setw(10) << "Value" << "\n";
        cout << setw(5)<<" " << setfill('-') << setw(20) << "" << setfill(' ') << "\n";

        for (int i = 0; i < 32; i++) {
            if(i<10){
                cout << setw(10) << "R" << i << setw(11) << GPR[i] << "\n";
            }
            else
                cout << setw(10) << "R" << i << setw(10) << GPR[i] << "\n";
        }
        cout << setw(5)<<" " << setfill('-') << setw(20) << "" << setfill(' ') << "\n"; 
    }
};
class DataMemory {
private:
    vector<int> memory = vector<int>(1024, 0);
    vector<bool> accessed = vector<bool>(1024, false);  

public:
    int read(int address) {
        if (address < 0 || address >= memory.size()) {
            cout << "Error: Address out of bounds\n";
            return -1; 
        }
        accessed[address] = true;
        return memory[address];
    }

    void write(int address, int value) {
        if (address < 0 || address >= memory.size()) {
            cout << "Error: Address out of bounds\n";
            return; 
        }
        memory[address] = value;
        accessed[address] = true;
    }
    void printAccessedMemory() {
        cout << "Accessed Data Memory:\n";
        cout << setw(14) << "Address" << setw(10) << "Value" << "\n";
        cout << setw(5) << " " << setfill('-') << setw(20) << "" << setfill(' ') << "\n";
        bool anyAccessed = false;
        for (int i = 0; i < accessed.size(); i++) {
            if (accessed[i]) {
                cout << setw(10) << i << setw(10) << memory[i] << "\n";
                anyAccessed = true;
            }
        }
        if (!anyAccessed) {
            cout << setw(4) << "" << setw(10) << "No data memory accessed.\n"; 
        }
        cout << setw(5) << " " << setfill('-') << setw(20) << "" << setfill(' ') << "\n";
    }
};

ControlWord controller(string instruction) {
    ControlWord cw; 
    string opcode = instruction.substr(25, 7);
    if (opcode == "0110011" && instruction.substr(0,7)=="0000001") { 
        cw.RegWrite = 1;
        cw.AluOp = 4;
        cw.RegRead = 1;
    } 
    else if (opcode == "0110011") { 
        cw.RegWrite = 1;
        cw.AluOp = 2;
        cw.RegRead = 1;
    } 
    else if (opcode == "0010011") { 
        cw.RegWrite = 1;
        cw.AluOp = 3;
        cw.AluSrc = 1;
        cw.RegRead = 1;
    }
    else if (opcode == "0000011") { 
        cw.RegWrite = 1;
        cw.MemRead = 1;
        cw.Mem2Reg = 1;
        cw.AluSrc = 1;
        cw.RegRead = 1;
    }
    else if (opcode == "0100011") {
        cw.MemWrite = 1;
        cw.AluSrc = 1;
        cw.RegRead = 1;
    }
    else if (opcode == "1100011") { 
        cw.Branch = 1;
        cw.AluOp = 1;
        cw.RegRead = 1;
    }
    else if (opcode == "1101111") {  
        cw.RegWrite = 1;
        cw.Jump = 1;
        cw.AluSrc = 1;
        cw.RegRead = 1;
    }
    else if (opcode == "1100111") {  
        cw.RegWrite = 1;   
        cw.JumpBack = 1;       
        cw.AluSrc = 1;     
        cw.RegRead = 1;  
    }
    else if (opcode == "0110111") {  
        cw.RegWrite = 1;        
        cw.AluSrc = 1;     
        cw.RegRead = 1;
        cw.AluOp=5;  
    }
    else if (opcode == "0010111") {  
        cw.RegWrite = 1;        
        cw.AluSrc = 1;     
        cw.RegRead = 1;
        cw.AluOp=6;  
    }
    return cw;
}
string ALUControl(string alu,string func,int AluOp){
    if(AluOp==0){
        return "add";
    }
    else if(AluOp==1){
        return "sub";
    }
    else if(AluOp==2){
        if(alu=="0100000"){
            if(func=="000"){
                return "sub";
            }
            else if(func=="101"){
                return "sra";
            }
        }
        else if(alu=="0000000"){
            if(func=="000"){
                return "add";
            }
            else if(func=="100"){
                return "xor";
            }
            else if(func=="110"){
                return "or";
            }
            else if(func=="111"){
                return "and";
            }
            else if(func=="001"){
                return "sll";
            }
            else if(func=="101"){
                return "srl";
            }
            else if(func=="010"){
                return "slt";
            }
            else if(func=="011"){
                return "sltu";
            }
        }
    }
    else if(AluOp==3){
        if(alu=="0100000"){
            if(func=="101"){
                return "sra";
            }
        }
        else{
            if(func=="000"){
                return "add";
            }
            else if(func=="100"){
                return "xor";
            }
            else if(func=="110"){
                return "or";
            }
            else if(func=="111"){
                return "and";
            }
            else if(func=="001"){
                return "sll";
            }
            else if(func=="101"){
                return "srl";
            }
            else if(func=="010"){
                return "slt";
            }
            else if(func=="011"){
                return "sltu";
            }
        }
    }
    else if (AluOp == 4) {
        if (func == "000") {
            return "mul";
        }
        else if (func == "001") {
            return "mulh";
        }
        else if (func == "010") {
            return "mulsu";
        }
        else if (func == "011") {
            return "mulu"; 
        }
        else if (func == "100") {
            return "div"; 
        }
        else if (func == "101") {
            return "divu"; 
        }
        else if (func == "110") {
            return "rem"; 
        }
        else if (func == "111") {
            return "remu"; 
        }
    }
    else if(AluOp=5){
        return "lui";
    }
    else if(AluOp=6){
        return "auipc";
    }
}
int ALU(string ALUSelect,int rs1,int rs2){
    if(ALUSelect=="and"){
        return (rs1 & rs2);
    }
    else if(ALUSelect=="or"){
        return (rs1 | rs2);
    }
    else if(ALUSelect=="add"){
        return (rs1 + rs2);
    }
    else if(ALUSelect=="sub"){
        return (rs1 - rs2);
    }
     else if(ALUSelect=="xor"){
        return (rs1 ^ rs2);
    }
    else if(ALUSelect=="sll"){
        return (rs1 << rs2);
    }
    else if(ALUSelect=="srl"){
        return (rs1 >> rs2);
    }
    else if(ALUSelect=="sra"){
        return static_cast<unsigned int>(rs1) >> static_cast<unsigned int>(rs2);
    }
    else if(ALUSelect=="slt"){
        return (rs1 < rs2)?1:0;
    }
    else if(ALUSelect=="sltu"){
        return (static_cast<unsigned int>(rs1) < static_cast<unsigned int>(rs2)) ? 1 : 0;
    }
    else if (ALUSelect == "mul") {
        return rs1 * rs2; 
    }
    else if (ALUSelect == "mulsu") {
        return (static_cast<unsigned int>(rs1) * rs2);  
    }
    else if (ALUSelect == "mulu") {
        return static_cast<unsigned int>(rs1) * static_cast<unsigned int>(rs2);  
    }
    else if (ALUSelect == "div") {
        if (rs2 == 0) {
            throw std::overflow_error("Division by zero");
        }
        return rs1 / rs2;  
    }
    else if (ALUSelect == "divu") {
        if (rs2 == 0) {
            throw std::overflow_error("Division by zero");
        }
        return static_cast<unsigned int>(rs1) / static_cast<unsigned int>(rs2);
    }
    else if (ALUSelect == "rem") {
        if (rs2 == 0) {
            throw std::overflow_error("Division by zero");
        }
        return rs1 % rs2;  
    }
    else if (ALUSelect == "remu") {
        if (rs2 == 0) {
            throw std::overflow_error("Division by zero");
        }
        return static_cast<unsigned int>(rs1) % static_cast<unsigned int>(rs2); 
    }
    else if(ALUSelect=="lui"){
        return rs2;
    }
    else if(ALUSelect=="auipc"){
        return rs1 + (rs2);
    }
    throw std::invalid_argument("Invalid ALU operation");
}
int immediate_calculate(string IR) {
    string opcode = IR.substr(25, 7);
    int imm = 0;
    if (opcode == "0000011" || opcode == "0010011") { 
        string immStr = IR.substr(0, 12);
        if (immStr[0] == '1') { 
            immStr = "11111111111111111111" + immStr; 
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    } 
    else if (opcode == "1100011") { 
        string immStr = IR.substr(0, 1) + IR.substr(24, 1) + IR.substr(1, 6) + IR.substr(20, 4);
        if (immStr[0] == '1') {  
            immStr = "11111111111111111111" + immStr;  
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    } 
    else if (opcode == "0100011") {  
        string immStr = IR.substr(0, 7) + IR.substr(20, 5); 
        if (immStr[0] == '1') {  
            immStr = "11111111111111111111" + immStr;  
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    }
    else if (opcode == "1101111") { 
        string immStr = IR.substr(0, 1) + IR.substr(12, 8) + IR.substr(11, 1) + IR.substr(1, 10) + "0";
        if (immStr[0] == '1') {  
            immStr = "111111111111" + immStr;  
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    }
    else if (opcode == "1100111") { 
        string immStr = IR.substr(0, 12);
        if (immStr[0] == '1') {  
            immStr = "11111111111111111111" + immStr;  
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    }
    else if (opcode == "0110111") { 
        string immStr = IR.substr(0, 20) + "000000000000"; 
        if (immStr[0] == '1') {
            immStr = "111111" + immStr; 
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    }
    else if (opcode == "0010111") { 
        string immStr = IR.substr(0, 20) + "000000000000"; 
        if (immStr[0] == '1') {
            immStr = "111111" + immStr;
            unsigned long long unsignedInt = stoul(immStr, nullptr, 2);
            imm = static_cast<int>(unsignedInt);
        } else {
            imm = stoi(immStr, nullptr, 2);
        }
    }
    return imm; 
}
int aluZeroFlag(int rs1,int rs2, string func){
    if(func=="000"){
        return (rs1==rs2);
    }
    else if(func=="001"){
        return (rs1!=rs2);
    }
    else if(func=="100"){
        return (rs1<rs2);
    }
    else if(func=="101"){
        return (rs1>=rs2);
    }
    else if(func=="110"){
       return (static_cast<unsigned int>(rs1) < static_cast<unsigned int>(rs2));
    }
    else if(func=="111"){
       return (static_cast<unsigned int>(rs1) >= static_cast<unsigned int>(rs2));
    }
    else{
        return 0;
    }
}
int SignedExtent(int imm,int pc){
    int offset=imm;
    if (imm & (1 << 11)) {
        offset = imm | 0xFFFFF000;
    } else {
        offset = imm & 0x00000FFF;
    }
    return pc + offset;
}
PC PC_stage;
IFID IFID_stage;
IDEX IDEX_stage;
EXMO EXMO_stage;
MOWB MOWB_stage;
RegisterFile registerFile;
DataMemory dataMemory;

void Instruction_Fetch(){
    if( PC_stage.valid && !IFID_stage.stall && (PC_stage.value/4)<instructionMemory.size()){
        IFID_stage.IR=instructionMemory[PC_stage.value/4];
        IFID_stage.DPC=PC_stage.value;
        IFID_stage.NPC=PC_stage.value+4;
        IFID_stage.valid=true;
        PC_stage.value=PC_stage.value+4;
    }
    else{
        IFID_stage.valid=false;
        return;
    }
}
void Instruction_Decode(){
    if(!IDEX_stage.stall && IFID_stage.valid){
        IDEX_stage.imm=immediate_calculate(IFID_stage.IR);
        IDEX_stage.JPC=SignedExtent(IDEX_stage.imm,IFID_stage.DPC);
        IDEX_stage.DPC=IFID_stage.DPC;
        IDEX_stage.imm_alu=IFID_stage.IR.substr(0,7);
        IDEX_stage.func=IFID_stage.IR.substr(17,3);
        int rsl1=stoi(IFID_stage.IR.substr(12,5), nullptr, 2);
        int rsl2=stoi(IFID_stage.IR.substr(7,5), nullptr, 2);
        IDEX_stage.rdl=stoi(IFID_stage.IR.substr(20,5), nullptr, 2);
        IDEX_stage.CW = controller(IFID_stage.IR);
        IFID_stage.stall=false;
         if(IDEX_stage.CW.RegRead){
            if(registerFile.GPR_valid[rsl1]==0){
                IDEX_stage.rs1=registerFile.read(rsl1);
            }
            else{
                IFID_stage.stall=true;
                    stalls++;
                IDEX_stage.valid=false;
                return;
            }
        }
        if(IDEX_stage.CW.AluSrc){
            if(IDEX_stage.CW.RegRead){
                IDEX_stage.rs2=IDEX_stage.imm;
                if (registerFile.GPR_valid[rsl2] == 0)
                {
                    IDEX_stage.rs2_val = registerFile.read(rsl2);
                }
                else
                {
                    IFID_stage.stall = true;
                    stalls++;
                    IDEX_stage.valid = false;
                    return;
                }
            }
        }
        else{
            if(IDEX_stage.CW.RegRead){
                if(registerFile.GPR_valid[rsl2]==0){
                    IDEX_stage.rs2=registerFile.read(rsl2);
                }
                else{
                    IFID_stage.stall=true;
                    stalls++;
                    IDEX_stage.valid=false;
                    return;
                }
            }
        }
        
        if (IFID_stage.IR.substr(25, 7) != "0100011" && IFID_stage.IR.substr(25, 7) != "1100011"){
            registerFile.GPR_valid[IDEX_stage.rdl]+=1;
        }
        IFID_stage.stall=false;
        IDEX_stage.valid=true;
    }
    else{
        IDEX_stage.valid=false;
        return;
    }
}
void Execution(){
    if(!EXMO_stage.stall && IDEX_stage.valid){
        string ALU_Select = ALUControl(IDEX_stage.imm_alu,IDEX_stage.func,IDEX_stage.CW.AluOp);
        EXMO_stage.ALU_result=ALU(ALU_Select,IDEX_stage.rs1,IDEX_stage.rs2);
        int ALUZeroFlag=aluZeroFlag(IDEX_stage.rs1 , IDEX_stage.rs2, IDEX_stage.func);
        EXMO_stage.CW=IDEX_stage.CW;
        EXMO_stage.rdl=IDEX_stage.rdl;
        EXMO_stage.rs2=IDEX_stage.rs2_val;
        if(IDEX_stage.CW.Branch && ALUZeroFlag){
            flushed++;
            PC_stage.value=(IDEX_stage.imm<<1)+IDEX_stage.DPC;
            IFID_stage.valid=false;
            IDEX_stage.valid=false;
            PC_stage.valid=true;
            
        }
        if(IDEX_stage.CW.Jump){
            IDEX_stage.rdl=IDEX_stage.DPC+4;
            PC_stage.value=IDEX_stage.JPC;
            IFID_stage.valid=false;
            IDEX_stage.valid=false;
            PC_stage.valid=true;
        }
        if(IDEX_stage.CW.JumpBack){
            IDEX_stage.rdl=IDEX_stage.DPC+4;
            PC_stage.value=IDEX_stage.rs1+IDEX_stage.imm;
            IFID_stage.valid=false;
            IDEX_stage.valid=false;
            PC_stage.valid=true;
        }
        EXMO_stage.DPC=IDEX_stage.DPC;
        IDEX_stage.stall=false;
        EXMO_stage.valid=true;
    }
    else{
        EXMO_stage.valid=false;
        return;
    }
}
void Memory_Operation(){
    if(!MOWB_stage.stall && EXMO_stage.valid){
        if(EXMO_stage.CW.MemWrite){
            dataMemory.write(EXMO_stage.ALU_result,EXMO_stage.rs2);
        }
        if(EXMO_stage.CW.MemRead){
            MOWB_stage.LD_out=dataMemory.read(EXMO_stage.ALU_result);
        }
        MOWB_stage.ALU_result=EXMO_stage.ALU_result;
        MOWB_stage.rdl=EXMO_stage.rdl;
        MOWB_stage.CW=EXMO_stage.CW;
        // if(IDEX_stage.CW.Branch || IDEX_stage.CW.Jump){
        //     PC_stage.value=EXMO_stage.TPC;
        //     PC_stage.valid=true;
        // }
        MOWB_stage.DPC=EXMO_stage.DPC;
        EXMO_stage.stall=false;
        MOWB_stage.valid=true;
        check=MOWB_stage.DPC;
    }
    else{
        MOWB_stage.valid=false;
        return;
    }
}
void Register_Write_Back(){
    if(MOWB_stage.valid){
        instructions_executed++; 
        if(!MOWB_stage.CW.RegWrite){
            return;
        }
        if(MOWB_stage.CW.Mem2Reg && registerFile.GPR_valid[MOWB_stage.rdl]>0){
            registerFile.write(MOWB_stage.rdl,MOWB_stage.LD_out);
            registerFile.GPR_valid[MOWB_stage.rdl]-=1;
            IFID_stage.stall=false;
            if(PC_stage.valid){
                IFID_stage.valid=true;
             }
           
        }
        else{
            registerFile.write(MOWB_stage.rdl,MOWB_stage.ALU_result);
            registerFile.GPR_valid[MOWB_stage.rdl]-=1;
             IFID_stage.stall=false;
             if(PC_stage.valid){
                IFID_stage.valid=true;
             }
        }
        MOWB_stage.stall=false;
    }
    else{
        return;
    }
}

void printPipelineState(int cycle) {
    cout << "Cycle " << cycle << ":\n";
    cout << "PC: " << PC_stage.value << "\n";
    
    cout << "IF/ID Stage:\n";
    cout << "  Instruction: " << IFID_stage.IR << "\n";
    cout << "  NPC: " << IFID_stage.NPC << "\n";
    cout << "  DPC: " << IFID_stage.DPC << "\n";
    cout << "  Stall: " << IFID_stage.stall << "\n";
    cout << "  Valid: " << IFID_stage.valid << "\n";

    cout << "ID/EX Stage:\n";
    cout << "  Imm: " << IDEX_stage.imm << "\n";
    cout << "  Rs1: " << IDEX_stage.rs1 << "\n";
    cout << "  Rs2: " << IDEX_stage.rs2 << "\n";
    cout << "  Rdl: " << IDEX_stage.rdl << "\n";
    cout << "  Stall: " << IDEX_stage.stall << "\n";
    cout << "  Valid: " << IDEX_stage.valid << "\n";
    cout << "Control Word:\n";
    cout << "  RegWrite: " << IDEX_stage.CW.RegWrite << "\n";
    cout << "  MemWrite: " << IDEX_stage.CW.MemWrite << "\n";
    cout << "  MemRead: " << IDEX_stage.CW.MemRead << "\n";
    cout << "  Branch: " << IDEX_stage.CW.Branch << "\n";
    cout << "  Jump: " << IDEX_stage.CW.Jump << "\n";
    cout << "  Mem2Reg: " << IDEX_stage.CW.Mem2Reg << "\n";
    cout << "  AluSrc: " << IDEX_stage.CW.AluSrc << "\n";
    cout << "  AluOp: " << IDEX_stage.CW.AluOp << "\n";
    cout << "  RegRead: " << IDEX_stage.CW.RegRead << "\n";
    
    cout << "EX/MO Stage:\n";
    cout << "  ALU Result: " << EXMO_stage.ALU_result << "\n";
    cout << "  Rs2: " << EXMO_stage.rs2 << "\n";
    cout << "  Rdl: " << EXMO_stage.rdl << "\n";
    cout << "  Stall: " << EXMO_stage.stall << "\n";
    cout << "  Valid: " << EXMO_stage.valid << "\n";
    // cout << "Control Word for EX/MO Stage:\n";
    // cout << "  RegWrite: " << EXMO_stage.CW.RegWrite << "\n";
    // cout << "  MemWrite: " << EXMO_stage.CW.MemWrite << "\n";
    // cout << "  MemRead: " << EXMO_stage.CW.MemRead << "\n";
    // cout << "  Branch: " << EXMO_stage.CW.Branch << "\n";
    // cout << "  Jump: " << EXMO_stage.CW.Jump << "\n";
    // cout << "  Mem2Reg: " << EXMO_stage.CW.Mem2Reg << "\n";
    // cout << "  AluSrc: " << EXMO_stage.CW.AluSrc << "\n";
    // cout << "  AluOp: " << EXMO_stage.CW.AluOp << "\n";
    // cout << "  RegRead: " << EXMO_stage.CW.RegRead << "\n";

    cout << "MO/WB Stage:\n";
    cout << "  ALU Result: " << MOWB_stage.ALU_result << "\n";
    cout << "  LD_out: " << MOWB_stage.LD_out << "\n";
    cout << "  Rdl: " << MOWB_stage.rdl << "\n";
    cout << "  Stall: " << MOWB_stage.stall << "\n";
    cout << "  Valid: " << MOWB_stage.valid << "\n";
    cout << "  DPC: " << MOWB_stage.DPC << "\n";
    // cout << "Control Word for MO/WB Stage:\n";
    // cout << "  RegWrite: " << MOWB_stage.CW.RegWrite << "\n";
    // cout << "  MemWrite: " << MOWB_stage.CW.MemWrite << "\n";
    // cout << "  MemRead: " << MOWB_stage.CW.MemRead << "\n";
    // cout << "  Branch: " << MOWB_stage.CW.Branch << "\n";
    // cout << "  Jump: " << MOWB_stage.CW.Jump << "\n";
    // cout << "  Mem2Reg: " << MOWB_stage.CW.Mem2Reg << "\n";
    // cout << "  AluSrc: " << MOWB_stage.CW.AluSrc << "\n";
    // cout << "  AluOp: " << MOWB_stage.CW.AluOp << "\n";
    // cout << "  RegRead: " << MOWB_stage.CW.RegRead << "\n";    
}



//    Assembler        //
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
string assembler(string line,int linenumber,map<string,pair<int,int>>labels,int labels_used){
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
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7);
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
        ans += temp.substr(1,7)+rs2+rs1+temp.substr(14,3)+rd+temp.substr(19,7);
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
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7);
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
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7);
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
        ans += imm+rs1+temp.substr(7,3)+rd+temp.substr(12,7);
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
        ans += imm.substr(0,7)+rs2+rs1+temp.substr(11,3)+imm.substr(7,5)+temp.substr(18,7);
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
            //cout<<format[2]<<endl;
            int instruction_diff = labels[format[2]].first-labels[format[2]].second - linenumber +labels_used;
            //cout<<instruction_diff<<endl;
            // if(instruction_diff<0){
            //     instruction_diff+=1;
            // }
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
        ans += imm[0]+imm.substr(2,6)+rs2+rs1+temp.substr(11,3)+imm.substr(8,4)+imm[1]+temp.substr(18,7);
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
        ans += imm+rd+temp.substr(7,7);
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
            int instruction_diff = labels[format[1]].first-labels[format[1]].second - linenumber +labels_used;
            // if(instruction_diff<0){
            //     instruction_diff+=1;
            // }
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
        ans += imm.substr(0,1)+imm.substr(10,10)+imm.substr(9,1)+imm.substr(1,8)+rd+temp.substr(6,7);
        return ans;
    }
     else if(temp[0]=='K' || temp[0]=='E'){
        string ans="";
        ans=temp.substr(1,temp.length()-1);
        return ans;
    }
    }
}



int main() {
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
    cout << "Give the instruction set  (use labels with a colon, e.g., .loop: ; ADD x1,x2,x3 ; addi x1,x1,5 ; beq x2,x3,loop ; Dont use x0 for any arithmetic operations it is Hardwired to 0, If using jal donot use x1 also it is return address):\n";
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
    // for (const auto& label : labels) {
    //     cout << "Label: " << label.first << ", Line: " << label.second.first << ", Previous labels count: " << label.second.second << endl;
    // }

    cout << "\nThe converted assembly instructions to machine language code:\n";
    int labels_used=0;
    for (int i = 0; i < n; i++) {
        if (!instruction_set[i].empty() && instruction_set[i].back() == ':'){
            labels_used++;
        }
        string ans;
        ans = assembler(instruction_set[i],i+1,labels,labels_used);
        if(!ans.empty()){
            //cout << ans<<endl;
            instructionMemory.push_back(ans);
        }
    } 
    //instructionMemory.push_back("00000000000000000000000000110011");
    instructionMemory.push_back("00000000000000000000000000000000");
      for (int i = 0; i < instructionMemory.size()-1; i++) {
            cout<<instructionMemory[i]<<endl;   
    } 
    cout << "\n";               
    int count=0;
    PC_stage.valid=true;
   // while(count<25){
    while(true){
        count++;
        Register_Write_Back();
        Memory_Operation();
        Execution();
        Instruction_Decode();
        Instruction_Fetch();
       // printPipelineState(count);
        if(check/4==instructionMemory.size()-1){
            break;
        }
    }
    //cout<<count<<endl;
    cout<<"Total number of clock cycles = "<<count<<endl;
    cout<<"Total number of stalled cycles = "<<stalls*2<<endl;
    cout<<"Total number of flushed cycles = "<<flushed<<endl;
    cout<<"Total number of executed instructions = "<<instructions_executed<<endl;
    cout<<"Cycles per instruction = "<<(float)count/(float)instructions_executed<<endl;
    registerFile.printRegisterFile();
    dataMemory.printAccessedMemory();
    cout << "\n";
    cout << "========= [End of Pipeline] =========" << endl;

    return 0;
}



// addi x2,x2,1234
// jal x1,loop
// addi x3,x3,12
// add x4,x4,x3
// beq x0,x0,end
// .loop:
// add x2,x2,x2
// jalr x0,0(x1)
// .end:
// add x0,x0,x0
