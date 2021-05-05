#include <stdio.h>
#include <windows.h>


DWORD dwGDT48;
DWORD dwEFLAG;
DWORD dwTSS_EFLAG;

void __declspec(naked) func() {



	__asm {
// 		pushad
            pushfd
			mov eax, dword ptr ds:[esp]
			mov dwEFLAG, eax
			add esp, 0x00000004

			mov eax, dword ptr ds:[0x0012FF04+0x24] // (TSS Table @ 0012FF04)
			mov dwTSS_EFLAG, eax

		    mov dword ptr ds:[0x8003f02c], 0x80008b04 // when using jmp, change 9 to b
            mov dword ptr ds:[0x8003f028], 0x200020ab

			mov eax, dword ptr ds:[0x8003f04c] 
			mov dwGDT48, eax

			
			
// 		popfd
// 	    popad
			iret
	}
}



int main(int argc, char* argv[])
{

	char bu[0x10]={0};
	int iCr3=0;

	DWORD tss[26] = {
		0x00000000,//link£¬ÓÉcpu×Ô¶¯Ìî³ä
			0x00000000,//esp0
			0x00000000,//ss0
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
			(DWORD)bu,//esp
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
	printf("tss=%p esp=%p func=%p", tss, bu,func);
	printf("please input iCr3:\n");
	scanf("%x", tss+7);
	
	char buff[6];
	*(DWORD*)&buff[0] = 0x12345678;
	*(WORD*)&buff[4] = 0x48;
	
	_asm {
   		str tss
			
		int 0 //eq 8003f400 0000E500`00480000  eq 8003f048 0000E912`FF040068(TSS Table @ 0012FF04)
		//call fword ptr ds:[buff]
		//jmp 0x0048:0x12345678
	}
//  	int * p =(int *)0x8003f048; //0x004011E5,//eip
//  	int n =*p;
    int tss_eflg =(int) *(tss +9) ;
	printf("\n TSS.EFLG AFTER CALL=%p\n,GDT48 AFTER JMP=%p\n, EFLG BEFORE IRET=%p, EFLG IN TSS AFTER IRET=%p", dwTSS_EFLAG, dwGDT48, dwEFLAG, tss_eflg);
	getchar();getchar();
	return 0;
}
