#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<ctype.h>
#include"header.h"
#include<byteswap.h>
/* Function: BL 
 * 
 * Description: Function used to simulate BL function.
 * 
 * Arguments: offset and opcode passed in from the bitfield
 *            modifies LR, PC and SP.  
 * 
 */
void BL_(unsigned short offset, unsigned short opcode)
{
    unsigned short store;
    if(offset & MSB_CHECK)                                     //sign extension on offset
    {
        offset |= NEW_MSB_SET ;
    }
    else    
    {
        offset &= NEW_MSB_CLR;
    }                           
    offset = offset << 1;                                    //left shift offset 
    stack[registers[R6]] = registers[R5];                    //push old LR to stack 
    registers[R6]++;                                         //increase SP 
    registers[R5] = pc;                                      //save old pc to LR 
    pc = pc + offset;                                        //add offset to pc 
}
/* Function: BRA
 * 
 * Description: Function used to simulate BRA function.
 * 
 * Arguments: offset and opcode passed in from the bitfield
 *            PC  
 */
void BRA_(unsigned short offset, unsigned short opcode)
{
    if(offset & SIGN_EXT)                                        //sign extension on offset
    {
        offset |= BRA_MSB_SET;
    }
    else
    {
        offset &= BRA_MSB_CLR;
    }
    offset = offset << 1;                                   //Left shift offset 
    pc = pc + offset;                                       //add offset to pc 
}
/* Function: ADD 
 * 
 * Description: Function used to simulate ADD function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            register[dst] is set as the sum of its previous value and the reg_const value 
 */
void ADD_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);             //these are needed for calculating                                                   
    int of_b = registers[dst] & (BYTE_MSB_CMP);             //the check_psw function 
    
    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    
    registers[dst] = registers[dst] + reg_const_val;        //adds src to dst.
    CHECK_PSW(dst, of_w, of_b, word_byte);                  //used to check psw
}
/* Function: ADDC
 * 
 * Description: Function used to simulate ADDC function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            register[dst] is set as the sum of its previous value and the reg_const value 
 *            and the carry value
 */
void ADDC_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);             //these are needed for calculating
    int of_b = registers[dst] & (BYTE_MSB_CMP);             //the check_psw function 

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }

    registers[dst] = registers[dst] + reg_const_val + psw.carry;        //adds src to dst.
    CHECK_PSW(dst, of_w, of_b, word_byte);                              //used to check psw
}       
/* Function: SUB
 * 
 * Description: Function used to simulate sub function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void SUB_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);             //these are needed for calculating
    int of_b = registers[dst] & (BYTE_MSB_CMP);             //the check_psw function 

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }

    registers[dst] = (~registers[dst] + 1) + reg_const_val; //subtract src from dst. 
    CHECK_PSW(dst, of_w, of_b, word_byte);                  //used to check psw
}       
/* Function: SUBC
 * 
 * Description: Function used to simulate sub function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void SUBC_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);             //these are needed for calculating
    int of_b = registers[dst] & (BYTE_MSB_CMP);             //the check_psw function 

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    registers[dst] = (~registers[dst] + 1) + reg_const_val + psw.carry; //subtract src from dst. 
    CHECK_PSW(dst, of_w, of_b, word_byte);                              //used to check psw
}   
/* Function: XOR
 * 
 * Description: Function used to simulate XOR function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void XOR_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);             //these are needed for calculating
    int of_b = registers[dst] & (BYTE_MSB_CMP);             //the check_psw function 

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    registers[dst] = registers[dst] ^ reg_const_val;        //xor src with dst. 
    CHECK_PSW(dst, of_w, of_b, word_byte);                  //used to check psw
}
/* Function: AND
 * 
 * Description: Function used to simulate AND function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void AND_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{   
    int of_w = registers[dst] & (WORD_MSB_CMP);             //these are needed for calculating
    int of_b = registers[dst] & (BYTE_MSB_CMP);             //the check_psw function

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    registers[dst] = registers[dst] & reg_const_val;        //AND src with dst. 
    CHECK_PSW(dst, of_w, of_b, word_byte);
}
/* Function: BIT
 * 
 * Description: Function used to simulate BIT function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void BIT_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);
    int of_b = registers[dst] & (BYTE_MSB_CMP);

    unsigned long temp = 0;
    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    temp = registers[dst] & reg_const_val;                  //AND src with dst. THIS NEEDS TO AFFECT THE PSW.
    CHECK_PSW(temp, of_w, of_b, word_byte);
    
}
/* Function: BIC
 * 
 * Description: Function used to simulate BIC function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void BIC_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);
    int of_b = registers[dst] & (BYTE_MSB_CMP);

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    registers[dst] =registers[dst] & ~reg_const_val;        //AND the compliment of src with dst.
    CHECK_PSW(dst, of_w, of_b, word_byte);
}
/* Function: BIS
 * 
 * Description: Function used to simulate BIS function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *            result is saved to registers[dst]
 */
void BIS_(unsigned short dst, unsigned short src,unsigned short word_byte,unsigned short reg_const)
{
    int of_w = registers[dst] & (WORD_MSB_CMP);
    int of_b = registers[dst] & (BYTE_MSB_CMP);

    int reg_const_val = 0;                      
    int reg_convert[] = {R0,R1,R2,R3,R4,R5,R6,R7};          //register conversion table
    int const_convert[] = {0,1,2,4,8,16,32,-1};             //const conversion table

    switch (reg_const)
    {
        case REG:                                           //checks if register 
            reg_const_val = registers[reg_convert[src]];    //sets reg_const_val to value in given register
        case CONST:                                         //checks if constant  
            reg_const_val = const_convert[src];             //sets reg_const_val to value in given const table
    }
    registers[dst] = registers[dst] | reg_const_val;        //OR src and dst.

    CHECK_PSW(dst, of_w, of_b, word_byte);
}
/* Function: BIS
 * 
 * Description: Function used to simulate BIS function.
 * 
 * Arguments: dst, src and word_byte  passed in from the bitfield
 *            register[dst] becomes register[src]
 */
void MOV_(unsigned short dst, unsigned short src, unsigned short word_byte)
{
    if(word_byte == BYTE)
    {
        src = src & UP_BYTE_CLR;                                 //set upper byte to 0 
    }
    registers[dst] = registers[src];                        //move value at inital register to dst register
}
/* Function: SWAP
 * 
 * Description: Function used to simulate SWAP function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *           registers[dst] becomes registers[src] and vice versa 
 */
void SWAP_(unsigned short dst, unsigned short src)
{
    unsigned short temp;
    temp = registers[dst];
    registers[dst] = registers[src];
    registers[src] = temp;
}
/* Function: SRA
 * 
 * Description: Function used to simulate SRA function.
 * 
 * Arguments: dst, src, word_byte and reg_const passed in from the bitfield
 *           registers[dst] contains new value 
 */
void SRA_(unsigned short dst,unsigned short word_byte)
{
    unsigned int msb_val;
    switch(word_byte)                                  //switch based on word or byte 
    {
        case 0:         
            msb_val = registers[dst] & (WORD_MSB_CMP);
            registers[dst] >>= 1;                       
            if(msb_val == 1)                            //if msb before shift is 1 
            {
                registers[dst] |= WORD_MSB_CMP;         //set new msb to 1 
            }
            else
            {
                registers[dst] &= NEW_MSB_SET;               //clears MSB
            }
            registers[dst] &= SRA_BYTE_CLR;                   //clears 14th bit (previous MSB)
        case 1:
            msb_val = registers[dst] & (BYTE_MSB_CMP);
            registers[dst] >>= 1;
            if(msb_val == 1)                            //if msb before shift is 1 
            {
                registers[dst] |= BYTE_MSB_CMP;         //set new msb to 1 
            }
            else
            {
                registers[dst] &= SRA_WRD_SET ;                 //clears msb           
            }
            registers[dst] &= SRA_WRD_CLR;                     //clears 7th bit (previous MSB)
    }   
    psw.carry = registers[dst] & 1;                     //sets carry if there is a carry value
}
/* Function: RRC
 * 
 * Description: Function used to simulate RRC function.
 * 
 * Arguments: dst, word_byte passed in from the bitfield
 *           registers[dst] contains new value 
 */
void RRC_(unsigned short dst, unsigned short word_byte)
{
    switch(word_byte)
    {
        case 0:                                     //word or byte switch case 
            registers[dst] >>= 1;
            if(psw.carry == 1)                      //if carry 
            {   
                registers[dst] |= WORD_MSB_CMP;     //set msb to 1 
            }
            else
            {
                registers[dst] &= SRA_WRD_SET ;           //clears msb
            }
        case 1:
            registers[dst] >>= 1;               
            if(psw.carry == 1)                      //if carry
            {
                registers[dst] |= BYTE_MSB_CMP;     //set msb to 1 
            }
            else
            {
                registers[dst] &= SRA_WRD_CLR;             //clears msb
            }
    }
    psw.carry = registers[dst] & 1;
}
/* Function: SWAB
 * 
 * Description: Function used to simulate SWAB function.
 * 
 * Arguments: dst from the bitfield
 *           registers[dst] contains new value 
 */
void SWPB_(unsigned short dst)
{
   registers[dst] = __bswap_16(registers[dst]);             //upper and lower byes swapped 
}
/* Function: SWAB
 * 
 * Description: Function used to simulate SWAB function.
 * 
 * Arguments: dst in from the bitfield
 *            registers[dst] contains new value 
 */
void SXT_(unsigned short dst)
{
    if((registers[dst] & BYTE_MSB_CMP) == BYTE_MSB_CMP)
    {
        registers[dst] |= UP_BYTE_HIGH;                           //sets upper byte high if msb equals 1 
    }
    else
    {
        registers[dst] &= UP_BYTE_CLR;                           //sets upper byte low if msb equals 0 
    }
}
/* Function: MOVL
 * 
 * Description: Function used to simulate MOVL function.
 * 
 * Arguments: dst and bit in from the bitfield
 *            registers[dst] contains new value 
 */
void MOVL_(unsigned short dst, unsigned short bit)
{
    registers[dst] &= UP_BYTE_HIGH;                             //clear low byte, leave upper byte unchanged 
    registers[dst] |= bit;                                //places bit on lower byte of registers[dst]
}
/* Function: MOVZ
 * 
 * Description: Function used to simulate MOVZ function.
 * 
 * Arguments: dst and bit in from the bitfield
 *            registers[dst] contains new value 
 */
void MOVZ_(unsigned short dst, unsigned short bit)
{   
    registers[dst] &= 0;                               //clears upper and lower byte 
    registers[dst] |= bit;                             //places bit on lower byte of registers[dst]
}
/* Function: MOVLS
 * 
 * Description: Function used to simulate MOVLS function.
 * 
 * Arguments: dst and bit in from the bitfield
 *            registers[dst] contains new value 
 */
void MOVLS_(unsigned short dst, unsigned short bit)
{
    registers[dst] |= UP_BYTE_HIGH;                       //SET upper byte high
    registers[dst] &= UP_BYTE_HIGH;                       //clear lower byte
    registers[dst] |= bit;                          //places bit on lower byte of registers[dst]
}
/* Function: MOVH
 * 
 * Description: Function used to simulate MOVH function.
 * 
 * Arguments: dst and bit in from the bitfield
 *            registers[dst] contains new value 
 */
void MOVH_(unsigned short dst, unsigned short bit)
{
    registers[dst] &= UP_BYTE_CLR;                    //clears upper byte 
    bit <<= 8;                                   //left shift bit 8 times
    registers[dst] |= bit;                       //places bit on upper byte of registers[dst]  
}
/* Function: LDR
 * 
 * Description: Function used to simulate LDR function.
 * 
 * Arguments: dst, bit, src, word_byte, and off in from the bitfield
 *            registers[dst] contains new value 
 */
void LDR_(unsigned short dst, unsigned short src, unsigned short word_byte, unsigned short off)
{
    int temp;
    unsigned short data;

    if((off & LDR_MSB ) == LDR_MSB )                //if msb equals 1 
    {
        off |= LDR_MSB_SET;                   //set upper byte high 
    }

    switch(word_byte)
    {
        case 0:

            temp = registers[src] + (off >> 1);     
            bus(temp,&data,READ,WORD);              //pulls data at temp location 
            registers[dst] = data;                  //set register[dst] to pulled data 
        case 1:
            temp = registers[src] + off;        
            bus(temp,&data,READ,WORD);              //pulls data at temp location
            registers[dst] = data;                  //set register[dst] to pulled data
    }
}
/* Function: STR
 * 
 * Description: Function used to simulate STR function.
 * 
 * Arguments: dst, bit, src, word_byte, and off in from the bitfield
 *            registers[dst] contains new value 
 */
void STR_(unsigned short dst, unsigned short src, unsigned short word_byte, unsigned short off)
{

    unsigned short temp;
    unsigned short data;
    if((off & LDR_MSB) == LDR_MSB)                    //if msb equals 1              
    {
        off |= LDR_MSB_SET;                          //set upper byte high 
    }
    switch(word_byte)
    {
        case 0:
            temp = registers[dst] + (off >> 1);
            bus(temp,&registers[src],WRITE, WORD);      //pushes data in register to memory 
        case 1:
            temp = registers[dst] + off;
            bus(temp, &registers[src],WRITE, WORD);     //pushes data in register to memory 
    }
}
/* Function: CHECK_PSW
 * 
 * Description: Function used to simulate CHECK_PSW function.
 * 
 * Arguments: dst, of_w, of_b, word_byte, and off in from the bitfield
 *            registers[dst] contains new value 
 */
void CHECK_PSW(unsigned short dst, int of_w, int of_b, int word_byte)
{
    switch(word_byte)
    {
        case 0:                             
            if(registers[dst] & WORD_MSB_CMP)               //checks if msb equals 1 
            {
                psw.negative = SET;
            }
            else
            {
                psw.negative = CLEAR;
            }
            if(registers[dst] > MAX_SHORT_VAL)              //checks if value in register is larger than max value
            {
                psw.carry = SET;
            }
            else
            {
                psw.carry  = CLEAR;
            }
            if(of_w != (registers[dst] & WORD_MSB_CMP))        
            {
                psw.overflow = SET;
            }
            else
            {
                psw.overflow = CLEAR;
            }
        case 1:
            if(registers[dst] & BYTE_MSB_CMP)               //checks if msb equals 1
            {
                psw.negative = SET;
            }
            else
            {
                psw.negative = CLEAR;
            }
            if(registers[dst] > MAX_BYTE_VAL)               //checks if value in register is larger than max value
            {
                psw.carry = SET;
            }
            else
            {
                psw.carry = CLEAR;
            }
            if(of_w != (registers[dst] & BYTE_MSB_CMP))
            {
                psw.overflow = SET;
            }
            else
            {
                psw.overflow = CLEAR;
            }
    }
    if(registers[dst] == CLEAR)                             //checks if registers is 0 
    {
        psw.zero = SET;
    }
    else
    {
        psw.zero = CLEAR;
    }

}
/* Function: CEX
 * 
 * Description: Function used to simulate CEX function.
 * 
 * Arguments: F,T, and condition n from the bitfield
 *            registers[dst] contains new value 
 */
void CEX_(unsigned short F, unsigned short T, unsigned short condition)
{
    t_count = T;
    f_count = F;
    switch(condition)                           //switch case that compare all condition values to possible values 
    {                                           //and checks there respective comparisions. Sets T_F_flag high if 
        case EQ:                                // test passes and low if test fails
            if(psw.zero == 1)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case NE:
            if(psw.zero == 0)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case CS_HS:
            if(psw.carry == 1)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case CC_LO:
            if(psw.carry == 0)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case MI:
            if(psw.negative == 1)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case PL:
            if(psw.negative == 0)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case VS:
            if(psw.overflow == 1)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case VC:
            if(psw.overflow == 0)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case HI:
            if(psw.carry == 1 && psw.zero == 0)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case LS:
            if(psw.carry == 0 && psw.zero == 1)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case GE:
            if(psw.negative == psw.overflow )
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case LT:
            if(psw.negative != psw.overflow )
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case GT:
            if(psw.zero = 0 &&  psw.negative == psw.overflow)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case LE:
            if(psw.zero = 1 &&  psw.negative != psw.overflow)
            {
                T_F_flag = true;
            }
            else
            {
                T_F_flag = false;
            }
            break;
        case AL:
            T_F_flag = true;
            break;
    }
}
