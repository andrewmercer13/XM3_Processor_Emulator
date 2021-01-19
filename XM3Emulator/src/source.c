/* File: ArchA2
 * Author: Andrew Mercer
 * School: Dalhousie University 
 * Dept: Electrical and Computer Eng 
 * 
 * Purpose: To emeulate loading an dumping data from memory. 
 * 
 * Note: A2.c
 * 
 * Last Modified: 2020.06.25
 * 
 */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include"header.h"
#include<byteswap.h>
#include<signal.h> /* signal handling software */

/* Function: DEBUGGER
 * 
 * Description: Used to parse user commands and call appropriate functions
 * 
 * Arguments:  in put stream used used to parse retreive Srecord 
 * 
 */
void DEBUGGER(FILE* fin)
{
    char command_line1[COMMAND_LINE_ARG_SIZE];
    char command_line2[COMMAND_LINE_ARG_SIZE];
    char command = 0;
    char file_name;
    char *start;
    char *end;
    unsigned short data;
    int cex_count = 0; 
    
   /*
    - volatile - do not cache
    - sig_atomic_t - shared variable - handle possible race conditions
    */
    volatile sig_atomic_t ctrl_c_fnd; /* T|F - indicates if ^C detected */

    /* Call signal() - bind sigint_hdlr to SIGINT */
    ctrl_c_fnd = false;
    signal(SIGINT, sigint_hdlr);
    
    while(1)
    {
        if(command != NEW_LINE)
        {
            printf("Option:");
        }
        scanf("%c",&command);
        /*Parsing command portion of user input*/
        /*Detecting for load command*/
        if(tolower(command) == 'l')
        {
            scanf("%s",command_line1);
            fin = fopen(command_line1,"r");
            /*If file not valid file*/           
            if(fin == NULL)
            {
                exit(1);
            }
            LOADER(fin);
        }
        /*Detecting for dump command*/
        else if(command == 'd')
        {
            scanf("%s %s",command_line1, command_line2);
            DUMP(command_line1, command_line2);
        }
        /*Detecting for go command*/
        else if(command == 'g')
        {
            printf("Start: PC: %4x Brkpt: %4x Clk: %d\n",pc,brkpt,system_clk);
            /*Loop until breakpoint reached*/
            while(!ctrl_c_fnd && pc != brkpt)
            {
                /*for cex*/
                if(cex_flag == true)
                {
                    /* if cex flag false skip f count number of instructions*/
                    if(T_F_flag == false)
                    {
                        pc = pc + f_count;
                        
                    }
                    /* if cex flag false skip t count number of instructions*/
                    else
                    {
                        pc = pc + t_count;
                    }
                    cex_flag = false;
                }
                data = fetch();
                
                decode(data);
            }
            printf("End: PC: %4x Clk: %d\n",pc,system_clk);
            
        }
        /*Detecting for exit command*/
        else if(command == 'x')
        {
            exit(EXIT_SUCCESS);
        }
        else if(command == 'r')
        {
            printf("R0: %.4lx\n",registers[R0]);
            printf("R1: %.4lx\n",registers[R1]);
            printf("R2: %.4lx\n",registers[R2]);
            printf("R3: %.4lx\n",registers[R3]);
            printf("R4: %.4lx\n",registers[R4]);
            printf("R5: %.4lx\n",registers[R5]);
            printf("R6: %.4lx\n",registers[R6]);
            printf("R7: %.4x\n",pc);
        }
        else if(command == 'b')
        {
            scanf("%hx",&brkpt);
            printf("Breakpoint Set\n");
        }
    }

}

/* Function: DUMP
 * 
 * Description: Dumps valutemp_program_counteres from memory from a start address to an end address in groups
 * of 16 bytes. 
 * 
 * Arguments: start[], end[] starting and ending addresses for the dump function.
 * 
 */
void DUMP(char start[], char end[])
{   
    
    unsigned short data_portion;
    unsigned short end_address = (int)strtol(end, NULL, BUS_TRANSFER_SIZE);
    unsigned short start_address = (int)strtol(start, NULL, BUS_TRANSFER_SIZE);
    char symbols[end_address - start_address];
    int count = 0;
    int current_address = start_address;
    int mar = 0;
    /*Top loop used to print groupings of 16*/
    while(current_address <= end_address)
    {
        printf("%04x ",current_address);
        /*Fetches and prints individual bytes in each grouping of 16*/
        while(count < BUS_TRANSFER_SIZE)
        {
            mar = current_address + count;
            bus(mar, &data_portion, READ, BYTE);
            /*symbols table used to print ASCII version of each byte later*/
            symbols[count] = (char)data_portion;
            printf("%02X ",data_portion);
            count++;
        }
        count = 0; 
        /*Used to print ASCII version of each byte*/
        while(count < BUS_TRANSFER_SIZE)
        {
            mar = current_address + count;
            bus(mar, &data_portion, READ, BYTE);
            /*If outside of printable ASCII range*/
            if (symbols[count] < INVALID_ASCII_SIZE)
            {
                printf(".");
            }
            else
            {
                printf("%c",symbols[count]);
            }
            count++;
        }
        count = 0;
        printf("\n");
        /*increasing by 16 each time due to dumping in groups of 16*/
        current_address += BUS_TRANSFER_SIZE;
    }
    return;
}

/* Function: LOADER
 * 
 * Description: Loads s records from file into memory. 
 * 
 * Arguments: fin input stream used to load srecords from debug file
 * 
 */
void LOADER(FILE * fin)
{
    
    enum BOOL valid_s_record;
    char record[MAX_RECORD_SIZE];
    int mar;
    int count;
    int i = 0;
    char *test;
    char name_of_file[MAX_RECORD_SIZE];
    int starting_address = 0;
    unsigned int file_name;
    int index;
    unsigned short *mbr;
    /*New line each loop*/
    while(fgets(record,MAX_RECORD_SIZE,fin) != NULL)
    {
        
            /*starts at 8 to pass Sx, length and address portions*/
            count = DATA_STARING_COUNT;
            valid_s_record = s_record_check(record);

            /*if s_record_check returns a false, record is invalid*/
            if(valid_s_record == false)
            {
                printf("INVALID S RECORD\n");
                return;
            }
            else
            {
                /*check for S0 records, used to print name of file*/
                if(record[1] == '0')
                {
                    
                    /* Gets data portion of S0 record by cutting off sx, length,
                     * address, and checksum portions.
                     */
                    while(count <= (strlen(record) - DATA_BACKEND_CUTOFF))
                    {
                        /*Gets byte of data portion and converts to hex*/
                        name_of_file[i] = record[count];
                        name_of_file[i + 1] = record[count + 1];
                        file_name = (int)strtol(name_of_file, NULL, HEX_CONVERSION);
                        count = count + COUNT_INCREASE;
                        printf("%c",file_name);
                        mar = address_parse(record);
                        mbr_load(record,mar);
                        
                    }
                    printf("Starting Address: %04X\n",mar);
                    printf("Data Loaded Correctly\n");
                    //printf("\n");
                }
                else if(record[1] == '9')
                {
                    pc = address_parse(record);

                    printf("Inital PC: %.4X\n", pc);
                    printf("Data Loaded Correctly\n");


                }else
                {
                    mar = address_parse(record);
                    printf("Starting Address: %04X\n",mar);
                    mbr_load(record,mar);
                    printf("Data Loaded Correctly\n");
                }
                
                
                

            }
    }  
    return;
}
/* Function: s_record_check
 * 
 * Description: Returns false flag if any S record is invalid
 * 
 * Arguments: record; all values in the s record
 * 
 */

enum BOOL s_record_check(char record[MAX_RECORD_SIZE])
{
    char length;
    char tens;
    char ones;
    int actual_length = 0;
    char c;
    char *temp;
    temp = strtok(record,"\r\n\0");
    /*checks if first character is 'S'*/
    if (temp[0] != 'S')
    {
        return false;
    }
    /*Checks if second character is one of the valid s record types*/
    if((temp[1] != '0') && (temp[1] != '1') && (temp[1] != '9'))
    {
        return false;
    }
    /*This block of code converts 2 array indexes to a single hex value*/
    c = temp[LENGTH_TENS];
    tens = chartoint(c);
    c = temp[LENGTH_ONES];
    ones = chartoint(c);
    length = (tens << TENS_SHIFT_VALUE); 
    length = (length | ones); 
    /* Gets actual number of bytes in S record*/
    actual_length = strlen(temp);
    actual_length = actual_length - DATA_BACKEND_CUTOFF;
    actual_length = (actual_length / BYTE_TO_BIT );
    if(actual_length != length)
    {
        return false;
    }

    if((check_sum(record)) == false)
    {
        return false;
    }
    return true;
}
/* Function: check_sum
 * 
 * Description: calculates the check sum and compares it with 
 * the S record check sum.
 * 
 * Arguments: record; all data in the s record
 * 
 */
enum BOOL check_sum(char record[150])
{
    char c;
    int count = COUNT_START; 
    unsigned int total_check_sum = 0;
    unsigned int running_check_sum; 
    unsigned int length = strlen(record) - SX_CUTOFF;
    /*Gets check sum values from s record and saves it as single hex value*/
    c =record[length];
    unsigned char tens = chartoint(c);
    
    c = record[length + 1];
    unsigned int ones = chartoint(c);
    
    unsigned int check_sum = (tens << TENS_SHIFT_VALUE);
    check_sum = (check_sum | ones);
    /*Loops though every byte except for Sx*/
    while(count <= (length - SX_CUTOFF))
    {
        c = record[count];
        tens = chartoint(c);
        c = record[count+1];
        ones = chartoint(c);
        running_check_sum = tens << TENS_SHIFT_VALUE;
        running_check_sum = running_check_sum | ones;

        /*Get total check sum*/
        total_check_sum = total_check_sum + running_check_sum;

        count = count + COUNT_INCREASE;
    }
    /*removed upper bit*/
    total_check_sum = total_check_sum & UPPER_BYTE_MASK;
    /* Ones complement */
    total_check_sum = ~total_check_sum;
    total_check_sum = total_check_sum & UPPER_BYTE_MASK;

    if(total_check_sum != check_sum)
    {
        return false;
    }
    return true;
}
/* Function: chartoint
 * 
 * Description: converts ASCII value to hex value.
 * 
 * Arguments: c; temp char used to return converted value
 * 
 */
int chartoint(char c)
{
    int value;

    if(c >= '0' && c <= '9')
    {
        value = c -'0';
    }
    else if(c >= 'a' && c <= 'f')
    {
        value = c -('a' - ASCII_CASE_SHIFT);
    }
    else if (c >= 'A' && c <= 'F')
    {
        value = c -('A' - ASCII_CASE_SHIFT);
    }
    else
    {
        value = ERROR_RETURN; 
    }

    return value; 
}
/* Function: address_parse
 * 
 * Description: Gets Address as int from record string
 * 
 * Arguments: record; s record data 
 * 
 */

int address_parse(char record[MAX_RECORD_SIZE])
{
    unsigned int ones;
    unsigned int tens;
    unsigned int hundreds;
    unsigned int thousands; 
    unsigned int address;
    char c;
    /*converts ASCII values to hex*/
    c = record[THOUSANDS_INDEX];
    
    thousands = chartoint(c);
    c = record[HUNDREDS_INDEX];
    hundreds = chartoint(c);
    c = record[TENS_INDEX];
    tens = chartoint(c);
    c = record[ONES_INDEX];
    ones = chartoint(c);

    thousands = thousands << THOUSANDS_SHIFT;
    
    hundreds = hundreds << HUNDREDS_SHIFT;
    tens = tens << TENS_SHIFT ;

    address = thousands | hundreds;
    address = address | tens;
    address = address | ones;

    return address;
}
/* Function: mbr_load
 * 
 * Description: load data into mbr and call bus function.
 * 
 * Arguments: record, mar 
 * 
 */
void mbr_load(char record[MAX_RECORD_SIZE], int mar)
{
    
    unsigned short data_length = strlen(record) - DATA_BACKEND_CUTOFF;
    unsigned short *mbr;
    unsigned int index = DATA_STARING_COUNT;
    char *data;
    /*Allocate Data for data variable*/
    data = (char *)malloc(SIZE_DOUBLE  * sizeof(char));
    /*for all data bytes of the record*/
    while(index <= data_length)
    {
        /*load byte into mbr as hex value*/
        data[0] = record[index];
        data[1] = record[index + 1];
        mbr = (unsigned short *)strtol(data,NULL,HEX_CONVERSION);
        bus(mar, mbr,WRITE,BYTE);
        mar++;
        index = index + INDEX_INCREASE;
    }
    printf("\n");
    return;
}
/* Function: bus
 * 
 * Description: Used to load data into memory and pass data back to be displayed .
 * 
 * Arguments: record, mar 
 * 
 */
void bus(unsigned short mar,unsigned short *mbr,enum ACTION rw,enum SIZE bw)
{
    /*increase system clock by 3 every time memory is accessed*/
    system_clk = system_clk + BUS_SYS_CLK_INCREASE;
    /* used for accessing by word */
    if(bw == WORD)
    {
        
        if(rw == READ)
        {
            unsigned short next_mar = mar + 1;
            *mbr = memory[next_mar] << BYTE_SIZE | memory[mar];
        }else
        {
            memory[mar] = *mbr;
        }
    }
    /* used for accessing by byte */
    else
    {
        
        int carry = mar % PC_INCREASE;
        if(rw == READ)
        {
            *mbr = memory[mar];
        }else
        {
            memory[mar] = mbr;
        }

    }
}
/* Function: fetch
 * 
 * Description: gets instruction data from memory based on pc .
 * 
 * Arguments: data; returns instruction data to be used by debug  
 * 
 */
int fetch()
{
    /* system clock increase once for fetch */
    system_clk++;
    unsigned short data;
    int count = 0;
    int mar;

    mar = pc;
    bus(mar, &data, READ, WORD);
    /*Increase pc 2 times for word access */
    pc = pc + PC_INCREASE;
    return data;
}
/* Function: decode 
 * 
 * Description: determines what instruction is being called and calls each instructions execution function
 * 
 * Arguments: data; instruction data that is being decoded. 
 */
void decode(unsigned short data)
{
    /*increase system clock once for decode*/
    system_clk++;
    decode_union inst_data;
    /*load data into bitfield */
    inst_data.instruction = data;
    int count = 0;
    /*Loops comparing ever instruction with its max possible value*/
    while(count < COUNT_MAX )
    {
        if(data <= max_instrct_val[count])
        {
            break;
        }
        count++;
    }
    /*Increase clock for execution phase */
    system_clk++;
    /*Switch case that calls each of the execution functions */
    switch(count)
    {
        case BL:
            /*printf("bl\n");*/
            BL_(inst_data.BL.offset, inst_data.BL.opcode);
            break;
        case BRA:
            /*printf("bra\n");*/
            BRA_(inst_data.BRA.offset, inst_data.BRA.opcode);
            break;
        case CEX:
            /*printf("cex\n");*/
            CEX_(inst_data.CEX.F, inst_data.CEX.T,inst_data.CEX.condition);
            break;
        case ADD:
            /*printf("add\n");*/
            ADD_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;

        case ADDC:
            /*printf("addc\n");*/
            ADDC_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case SUB:
            /*printf("sub\n");*/
            SUB_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case SUBC:
            /*printf("subc\n");*/
            SUBC_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case XOR:
            /*printf("xor\n");*/
            XOR_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case AND:
            /*printf("AND\n");*/
            AND_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case BIT:
            /*printf("BIT\n");*/
            BIT_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case BIC:
            /*printf("BIC\n");*/
            BIC_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case BIS:
            /*printf("BIS\n");*/
            BIS_(inst_data.ALU.dst, inst_data.ALU.src, inst_data.ALU.word_byte, inst_data.ALU.reg_const);
            break;
        case MOV:
            /*printf("MOV\n");*/
            MOV_(inst_data.MOV.dst, inst_data.MOV.src, inst_data.MOV.word_byte);
            break; 
        case SWAP:
            /*printf("SWAP\n");*/
            SWAP_(inst_data.SWAP.dst, inst_data.SWAP.src);
            break; 
        case SRA:
            /*printf("SRA\n");*/
            SRA_(inst_data.SRA_RRC.dst, inst_data.SRA_RRC.word_byte);
            break;
        case RRC:
            /*printf("RRC\n");*/
            RRC_(inst_data.SRA_RRC.dst, inst_data.SRA_RRC.word_byte);
            break;   
        case SWPB:
            /*printf("SWPB\n");*/
            SWPB_(inst_data.SWPB_SXT.dst);
            break; 
        case SXT:
            /*printf("SXT\n");*/
            SXT_(inst_data.SWPB_SXT.dst);
            break;
        case MOVL:
            /*printf("MOVL\n");*/
            MOVL_(inst_data.MOVX.dst, inst_data.MOVX.bit);
            break;  
        case MOVZ:
            /*printf("MOVZ\n");*/
            MOVZ_(inst_data.MOVX.dst, inst_data.MOVX.bit);
            break; 
        case MOVS:
            /*printf("MOVS\n");*/
            MOVLS_(inst_data.MOVX.dst, inst_data.MOVX.bit);
            break; 
        case MOVH:
            /*printf("MOVH\n");*/
            MOVH_(inst_data.MOVX.dst, inst_data.MOVX.bit);
            break; 
        case LDR:
            /*printf("LDR\n");*/
            LDR_(inst_data.LDR_STR.dst, inst_data.LDR_STR.src, inst_data.LDR_STR.word_byte, inst_data.LDR_STR.off);
            break;
        case STR:
            /*printf("STR\n");*/
            STR_(inst_data.LDR_STR.dst, inst_data.LDR_STR.src, inst_data.LDR_STR.word_byte, inst_data.LDR_STR.off);
            break;   
    }
    return;
}
/* Function: sigint_hdlr
 * 
 * Description: handles control + c 
 * 
 * Arguments: N/A 
 * 
 */
void sigint_hdlr()
{
    /*
    - Invoked when SIGINT (control-C) is detected
    - changes state of waiting_for_signal
    - signal must be reinitialized
    */
    volatile sig_atomic_t ctrl_c_fnd;
    ctrl_c_fnd = true;
    signal(SIGINT, sigint_hdlr); /* Reset SIGINT */
}
