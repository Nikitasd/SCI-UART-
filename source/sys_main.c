/** @file sys_main.c 
*   @brief Application main file
*   @date 11-Dec-2018
*   @version 04.07.01
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
#include "sci.h"
#include <string.h>
#include "stdint.h"
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
static uint8 command[17];
static bool ASCII_input = 0;
static uint16 move_on = 0;
static uint16 index_of_mass = 0;
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
void ASCI_Convert(){
    sciSendByte(scilinREG, '\r');
    sciSendByte(scilinREG, '\n');
    for(uint32 i = 0; i < 16; i++){
        uint32 decimal = command[i];
        uint16 j = 0;
        uint8 conv[3] = {0, 0, 0};
        while(decimal){
            conv[2-j] = (uint8)(decimal%10 | 0x30);
            j++;
            decimal /= 10;
        }
        if(command[i] == '\x0d'){
            sciSend(scilinREG, sizeof(conv), (uint8 *)conv);
            sciSend(scilinREG, 1, (uint8 *)" ");
        }else{
            sciSend(scilinREG, 1, (uint8 *)&command[i]);
            sciSend(scilinREG, sizeof(conv), (uint8 *)conv);
            sciSend(scilinREG, 1, (uint8 *)" ");
        }
    }
    sciSendByte(scilinREG, '\n');
    ASCII_input = 0;
}
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
    sciInit();
    //sciSend(scilinREG, sizeof(Hi_World) - 1, (uint8 *)Hi_World);
    //sciReceive(scilinREG, 1, (unsigned char *)&command);
    //sciReceive(scilinREG, 5, (uint8 *)&command);
    sciNotification(scilinREG, (uint8)*command);
    _enable_IRQ();
    while(1){

        if(move_on){
            for(int i = 0; i < 17; i++){
                command[i] = command[i + 1];
            }
            if(command[16] == '\x0d'){
                ASCII_input = 1;
            }else{
                sciSendByte(scilinREG, '\r');
                sciSendByte(scilinREG, '\n');
            }
            move_on = 0;
        }
        if(ASCII_input){
            sciDisableNotification(scilinREG, (uint8)*command);
            ASCI_Convert();
            sciEnableNotification(scilinREG, (uint8)*command);
        }
    }
/* USER CODE END */

}


/* USER CODE BEGIN (4) */
void sciNotification(sciBASE_t *sci, uint32 flags)
{
//  enter user code between the USER CODE BEGIN and USER CODE END.
    sciReceive(sci, 1, (uint8 *)&command[index_of_mass]);
    sciSendByte(sci, command[index_of_mass - 1]);
    index_of_mass++;
    if(index_of_mass == 18){
        index_of_mass--;
        move_on = 1;
    }
}
 /* USER CODE END */
