#include<LPC21XX.H>
typedef unsigned int U32;
void delaym(U32);
void delaym(U32 ms)
{
	
	T1PR = 15000-1;
		T1TCR = 1;
		while(T1TC<ms);
		T1TCR = 3;
		T1TCR = 0;

}