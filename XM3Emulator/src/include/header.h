/* File: ArchA2
 * Author: Andrew Mercer
 * School: Dalhousie University 
 * Dept: Electrical and Computer Eng 
 * 
 * Purpose: To emeulate loading an dumping data from memory. 
 * 
 * Note: header.h
 * 
 * Last Modified: 2020.06.25
 * 
 */

#ifndef HEADER_H
#define HEADER_H

#define COMMAND_LINE_ARG_SIZE 100
#define BUS_TRANSFER_SIZE     16
#define INVALID_ASCII_SIZE    32
#define MAX_RECORD_SIZE       100
#define DATA_STARING_COUNT    8
#define DATA_BACKEND_CUTOFF   4
#define HEX_CONVERSION        16
#define COUNT_INCREASE        2
#define LENGTH_TENS           2
#define LENGTH_ONES           3
#define TENS_SHIFT_VALUE      4
#define BYTE_TO_BIT           2
#define COUNT_START           2
#define SX_CUTOFF             2
#define COUNT_INCREASE        2
#define UPPER_BYTE_MASK       0x0FF
#define ASCII_CASE_SHIFT      10
#define ERROR_RETURN          -1
#define THOUSANDS_INDEX       4 
#define HUNDREDS_INDEX        5
#define TENS_INDEX            6
#define ONES_INDEX            7
#define THOUSANDS_SHIFT       12
#define HUNDREDS_SHIFT        8
#define TENS_SHIFT            4
#define SIZE_DOUBLE           2
#define INDEX_INCREASE        2
#define NO_ARGS               1
#define TWO_ARGS              3
#define FIRST_ARG             2
#define REG                   0
#define CONST                 1
#define CLEAR                 0
#define SET                   1
#define WORD_MSB_CMP          0x8000
#define BYTE_MSB_CMP          0x80
#define MAX_SHORT_VAL         65536
#define MAX_BYTE_VAL          255
#define NEW_LINE              10
#define BUS_SYS_CLK_INCREASE  3
#define BYTE_SIZE             8
#define PC_INCREASE           2
#define COUNT_MAX             24
#define MSB_CHECK             0x1000  
#define NEW_MSB_SET           0x7FFF
#define NEW_MSB_CLR           0x1FFF
#define SIGN_EXT              0x200
#define BRA_MSB_SET           0x7C00
#define BRA_MSB_CLR           0x3FF
#define UP_BYTE_CLR           0x00FF
#define SRA_BYTE_CLR          0xBFFF
#define SRA_WRD_SET           0x7F
#define SRA_WRD_CLR           0x7F      
#define UP_BYTE_HIGH          0xFF00  
#define LDR_MSB               0x40 
#define LDR_MSB_SET           0xFF10   

enum BOOL   {true = 1, false = 0};
enum ACTION {READ = 1, WRITE = 0};
enum SIZE   {BYTE = 1, WORD =  0};



struct PSW_STRUCT
{
    unsigned short carry: 1;
    unsigned short zero:  1;
    unsigned short negative: 1;
    unsigned short overflow: 1;
};

struct PSW_STRUCT psw;

/*Every index is one byte*/
unsigned short memory[65536];
unsigned long registers[8];

unsigned short stack[65536];
unsigned short system_clk;
unsigned short pc;
unsigned short brkpt;
unsigned short t_count;
unsigned short f_count;
enum BOOL T_F_flag;
enum BOOL   cex_flag;
enum BOOL   debugger_flag;
enum ACTION rw;
enum SIZE   bw;
enum
{
    R0,
    R1,
    R2,
    R3,
    R4,
    R5,
    R6,
    R7
};
void DUMP(char start[], char end[]);
int chartoint(char c);
int address_parse(char record[150]);
enum BOOL s_record_check(char record[150]);
enum BOOL check_sum(char record[150]);
void bus(unsigned short mar,unsigned short *mbr,enum ACTION rw,enum SIZE bw);
void mbr_load(char record[150], int mar);
void DEBUGGER(FILE* fin);
void LOADER(FILE * fin);
int fetch();
void decode(unsigned short data);
void sigint_hdlr();
void BRA_(unsigned short offset, unsigned short opcode);
void ADD_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void ADDC_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void SUB_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void SUBC_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void XOR_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void AND_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void BIT_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void BIC_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void BIS_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const);
void MOV_(unsigned short dst, unsigned short src,unsigned short word_byte);
void SWAP_(unsigned short dst, unsigned short src);
void SRA_(unsigned short dst, unsigned short word_byte);
void RRC_(unsigned short dst, unsigned short word_byte);
void SWPB_(unsigned short dst);
void SXT_(unsigned short dst);
void BL_(unsigned short offset, unsigned short opcode);
void MOVL_(unsigned short dst, unsigned short bit);
void MOVZ_(unsigned short dst, unsigned short bit);
void MOVLS_(unsigned short dst, unsigned short bit);
void MOVH_(unsigned short dst, unsigned short bit);
void LDR_(unsigned short dst, unsigned short src, unsigned short word_byte, unsigned short off);
void STR_(unsigned short dst, unsigned short src, unsigned short word_byte, unsigned short off);
void CHECK_PSW(unsigned short dst, int of_w, int of_b, int word_byte);
void CEX_(unsigned short F, unsigned short T, unsigned short condition);
static int max_instrct_val[] = 
{
    8192,
    9215,
    10239,
    16639,
    16895,
    17151,
    17407,
    18175,
    18431,
    18687,
    18943,
    19199,
    19583,
    19647,
    19783,
    19911,
    19975,
    19983,
    26623,
    28671,
    30719,
    32767,
    49151,
    65535
};

typedef union {
    struct
    { 
        unsigned int offset : 13;
        unsigned int opcode : 3;
    } BL;

    struct
    { 
        unsigned int F : 3;
        unsigned int T : 3;
        unsigned int condition : 4;
        unsigned int opcode    : 6;
    } CEX;

    struct
    { 
        unsigned int dst : 3;
        unsigned int zeros : 3;
        unsigned int word_byte : 1;
        unsigned int opcode    : 9;
    } SRA_RRC;

    struct
    { 
        unsigned int offset : 10;
        unsigned int opcode : 6;
    } BRA;

    struct
    { 
        unsigned int dst : 3;
        unsigned int src : 3;
        unsigned int word_byte : 1;
        unsigned int opcode    : 9;
    } MOV;
    
    struct
    { 
        unsigned int dst : 3;
        unsigned int src : 3;
        unsigned int word_byte : 1;
        unsigned int reg_const : 1;
        unsigned int opcode    : 8;
    } ALU;

    struct
    { 
        unsigned int dst : 3;
        unsigned int src : 3;
        unsigned int opcode  : 10;
    } SWAP;

    struct
    { 
        unsigned int dst : 3;
        unsigned int src : 3;
        unsigned int word_byte : 1;
        unsigned int off : 7;
        unsigned int opcode  : 2;
    } LDR_STR;

    struct
    { 
        unsigned int dst : 3;
        unsigned int opcode  : 13;
    } SWPB_SXT;

    struct
    { 
        unsigned int dst : 3;
        unsigned int bit : 8;               
        unsigned int opcode  : 5;
    } MOVX;


    short instruction; 
} decode_union;
/*enums used for execuate switch case */
static enum 
{
    BL,
    BRA,
    CEX,
    ADD,
    ADDC,
    SUB,
    SUBC,
    XOR,
    AND,
    BIT,
    BIC,
    BIS,
    MOV,
    SWAP,
    SRA,
    RRC,
    SWPB,
    SXT,
    MOVL,
    MOVZ,
    MOVS,
    MOVH,
    LDR,
    STR
};
/*enums used for cex switch case */
static enum 
{
    EQ,
    NE,
    CS_HS,
    CC_LO,
    MI,
    PL,
    VS,
    VC,
    HI,
    LS,
    GE,
    LT,
    GT,
    LE,
    AL
};
#endif