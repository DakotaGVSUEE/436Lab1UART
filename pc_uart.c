/*
Author: Dakota Culbertson
Date: 1/21/2021
Class: EGR 436 10
Title: Lab 1 UART communication
*/


#include "pc_uart.h"

//DCB dcbMasterInitState;
int bpm;
int buf;
char user = '\0';
int tempo;


int main(void)
{
    initialize_comm(&hMasterCOM,UART_PORT);
    change_tempo(0);
    printf("\n\n u - Increase Tempo\t d - Decrease Tempo\t r - Reset Tempo\n\n");

    while(1){
        while(user == '\0'){  //wait for input
            scanf("%c",&user);
            fflush(stdin);
        }
        if(user == 'u'){
            change_tempo(1);
            user = '\0';
        }else if(user == 'd'){
            if(bpm == 2){
                printf("\n tempo cannot be decreased further \n");
            }else{
            change_tempo(-1);
            user = '\0';
            }
        }else if(user == 'r'){
            change_tempo(0);
            user = '\0';
        }else if(user == 'q'){
            CloseHandle(&hMasterCOM);
            break;
        }else{
            printf("\n\n%c",user);
            printf(" is invalid\n\n");
            user = '\0';
        }
    }
/*

 //HANDLE* hComm
 printf("\n comm initialize\n");
 HANDLE hMasterCOM = CreateFile("\\\\.\\COM6",
                                GENERIC_READ | GENERIC_WRITE,
                                0,
                                0,
                                OPEN_EXISTING,
                                FILE_ATTRIBUTE_NORMAL,
                                0);

    if(hMasterCOM == INVALID_HANDLE_VALUE){
        printf("invalid handle value\n");
        exit(0);
    }


    printf("Purge Port\n\n");

    purge = PurgeComm(hMasterCOM, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    printf("\n purge value is %d\n",purge);

    printf("getting original com state \n");

    comstate = GetCommState(hMasterCOM, &dcbMasterInitState);
    if(comstate == 0){
        printf("\n could not retrieve com state");
    }
    printf("\n Com state is %d\n",comstate);
 //if(initialize_comm(hMasterCOM, UART_PORT,&part));
 //setSerialParams(phComm, dcbMaster);
*/
}

void change_tempo(int delta){

    DWORD dww;
    DWORD dwr;
    DWORD length;

    if(delta == -1){
        if(bpm <= 1){
            printf("\n Tempo cannot be decreased any further\n");
        }
        else{
            bpm -= 2;
        }
    }if(delta == 1){
        bpm += 2;
    }if(delta == 0){
        bpm = 60;
    }
    buf = (floor(log10(abs(bpm))) + 1) + 2;
    length = buf - 1;
    printf("\n buffer is %d  length is %d \n",buf,length);
    /*
    if(bpm <= 9){
        buf = 1;
    }else if(bpm <= 99){
        buf = 2;
    }else if(bpm <= 999){
        buf = 3;
    }
*/
    BYTE wbuf[buf];
    BYTE rbuf[buf];

    itoa(bpm,wbuf,10);
    strcat(wbuf,"\0");

    printf("\n writing data %s",wbuf);
    WriteData(hMasterCOM,&wbuf,length,&dww);
    ReadData(hMasterCOM,&rbuf,1,&dwr,UART_TIMEOUT);
    tempo = rbuf[0];
    printf("\nTempo Changed: %d BPM\n",tempo);
    //delta = 0;
}
void initialize_comm(HANDLE* hComm, int comInt)
{
    int purge = 0;
    int comstate = 0;
    int params = 0;
    printf("\n opening com %d\n",comInt);
    //printf("\n hComm point %d\n",hComm);
    openComm(hComm, comInt);
    printf("\n attempting to purge com\n");
    //printf("hComm handle %d",*hComm);
    //purge = PurgeComm(hComm)
    purge = PurgeComm(*hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
    if(purge == 0){
        printf("\nCom could not be purged\n");
        exit(0);
    }
          //printf("\n trying to purge com\n");
    //{
     //sprintf("\n com %d purged\n",comInt);
     printf("\n retrieving com state\n");
     comstate = GetCommState(*hComm, &dcbMasterInitState);
     if(comstate == 0 ){
        printf("\n could not retrieve com %d state \n", comInt);
        exit(0);
     }
    //}else{exit(0);}
    //GetCommState(hComm, &dcbMasterInitState);
    printf("\n Setting Serial Parameters\n");
    params = setSerialParams(hComm, dcbMaster);
    if(params == 0){
        printf("\n Serial Parameters Were Not Set\n");
        exit(0);
    }
    //return(params)
}
void openComm(HANDLE* phComm, int comInt)
{

    printf("creating port string\n\n");
    char portStr[13] = "\\\\.\\COM6";
    //printf("\n phComm point %d\n\n",phComm);
    //sprintf(portStr, "%d",comInt);
    //if(comInt < 10)
    //{
        //sprintf(portStr, "COM%d", comInt);
        *phComm = CreateFile("\\\\.\\COM3",
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            0,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                            0);
    //}
    /*else{
        sprintf(portStr, "\\\\.\\COM%d", comInt);
        *phComm = CreateFile(portStr,
                            GENERIC_READ | GENERIC_WRITE,
                            0,
                            NULL,
                            OPEN_EXISTING,
                            0,
                            NULL);
    }
*/
    if( *phComm == INVALID_HANDLE_VALUE)
    {
        printf("Error in opening serial port... Exiting\n\n");
        exit(0);

    }
    else
    {
        printf(portStr);
        printf(" port has been connected");
    }
}

BOOL setSerialParams(HANDLE* phComm, DCB dcbMaster)
{
    bool status;

    dcbMaster.BaudRate = UART_BAUD;
    dcbMaster.Parity = UART_PARITY;
    dcbMaster.ByteSize = UART_BYTESIZE;
    dcbMaster.StopBits = UART_STOPBITS;

    status = SetCommState(*phComm, &dcbMaster);

    //delay(60);

    return(status);
}

bool WriteData(HANDLE handle, BYTE* data, DWORD length, DWORD* dwWritten)
{
     bool success = false;
     OVERLAPPED o = {0};
     o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
     if (!WriteFile(handle, (LPCVOID)data, length, dwWritten, &o))
         {
         if (GetLastError() == ERROR_IO_PENDING)
             if (WaitForSingleObject(o.hEvent, INFINITE) == WAIT_OBJECT_0)
                 if (GetOverlappedResult(handle, &o, dwWritten, FALSE))
                 success = true;
         }
     else
     success = true;
     if (*dwWritten != length)
     success = false;
     CloseHandle(o.hEvent);
     PurgeComm(handle, PURGE_TXABORT | PURGE_TXCLEAR);
     return success;
}

bool ReadData(HANDLE handle, BYTE* data, DWORD length, DWORD* dwRead, UINT timeout)
{
     bool success = false;
     OVERLAPPED o = {0};

     o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
     if (!ReadFile(handle, data, length, dwRead, &o))
         {
         if (GetLastError() == ERROR_IO_PENDING)
             if (WaitForSingleObject(o.hEvent, timeout) == WAIT_OBJECT_0)
             success = true;
             GetOverlappedResult(handle, &o, dwRead, FALSE);
         }
     else
     success = true;
     CloseHandle(o.hEvent);
     PurgeComm(handle, PURGE_RXABORT | PURGE_RXCLEAR);
     return success;
}

void CloseCom(HANDLE* hComm, DCB dcbMasterInitState){
    SetCommState(*hComm, &dcbMasterInitState);

    //delay(60);

    CloseHandle(*hComm);
    *hComm = INVALID_HANDLE_VALUE;
}
