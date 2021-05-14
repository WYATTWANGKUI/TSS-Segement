#include <stdio.h>
#include <windows.h>
//This experiment is done on WINXP OS. 
//using WINDBG command below to modify WINXP's LDT and GDT, to create a interrupt gate0000E500`00480000 at the address 0x8003f400(inside LDT) 
//and a new TSS descriptor 0000E912`FF040068 at the address 0x8003f048(inside GDT).The size of a TSS table is 0x68 bytes.
//eq 8003f400 0000E500`00480000  eq 8003f048 0000E912`FF040068(The base address of the new TSS Table is at 0x0012FF04 in this case)
//WINXP is using its default TSS descriptor 0x80008b04`0x200020ab at address 0x8003f028, where 0x80042000 is the base of the default TSS table that WINXP is using.


DWORD dwGDT48;
DWORD dwEFLAG;
DWORD dwTSS_EFLAG;

void __declspec(naked) func() {
  __asm {
            pushad
            pushfd
		    
//the below instructions serve to get the current EFLAG value and put it into dwEFLAG  
  mov eax, dword ptr ds:[esp]
  mov dwEFLAG, eax
  
//the below instructions serve to get the EFLAG value being stored in the new TSS table, and put this EFLAG value into dwTSS_EFLAG	  
  mov eax, dword ptr ds:[0x0012FF04+0x24] //(the base address of the new TSS Table is at 0x0012FF04. EFLAG is stored at the offset 0x24 ) 
  mov dwTSS_EFLAG, eax

//WINXP is using its default TSS descriptor 0x80008b04`0x200020ab at address 0x8003f028, where 0x80042000 is the base of the default TSS table that WINXP is using.
//The below instructions serve to ensure the old default TSS descriptor is in correct form so that it will work properly when using iret at the end.
mov dword ptr ds:[0x8003f028], 0x200020ab
mov dword ptr ds:[0x8003f02c], 0x80008b04 //when using jmp instead of int/call to switch to a new TSS descriptor, need to change 9 to b which means from available(9) to busy(b)

            popfd
 	    popad
iret
	}
}



int main(int argc, char* argv[])
{

	char esp_buff[0x10]={0};
	int iCr3=0;

	DWORD TSS[26] = {
		        0x00000028,//offset 0, storing the previous TR descriptor
			0x00000000,//offset 4, storing new esp0
			0x00000000,//offset 8, storing new ss0
			0x00000000,//esp1
			0x00000000,//ss1
			0x00000000,//esp2
			0x00000000,//ss2
			(DWORD)iCr3,
			(DWORD)func,//eip
			0x00000002,//eflags
			0x00000000,//eax
			0x00000000,//ecx
			0x00000000,//edx
			0x00000000,//ebx
			(DWORD)esp_buff,//esp
			0x00000000,//ebp
			0x00000000,//esi
			0x00000000,//edi
			0x00000023,//es
			0x00000008,//cs
			0x00000010,//ss
			0x00000023,//ds
			0x00000030,//fs
			0x00000000,//gs
			0x00000000,//ldtr
			0x20ac0000
	};
	printf("tss=%p esp=%p func=%p", tss, esp_buff, func);
	printf("please input iCr3:\n");
	scanf("%x", tss+7);
	
	char buffer[6];
	*(DWORD*)&buffer[0] = 0x12345678;
	*(WORD*)&buffer[4] = 0x48;
	
	_asm {
			
		int 0 
	}

	
        int TSS_EFLAG =(int) *(tss +9) ;
	printf("\n TSS.EFLG AFTER CALL=%p\n, EFLG BEFORE IRET=%p, EFLG IN TSS AFTER IRET=%p", dwTSS_EFLAG, dwEFLAG, TSS_EFLAG);
	getchar();getchar();
	return 0;
}
