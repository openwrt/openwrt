#include <Judy.h>

int main()
{
    int Rc_int;

    int i;

    // Judy array to hold cached addresses
    Pvoid_t addrArray = (Pvoid_t) NULL;

    for(i = 0; i < 100000; i++)
	J1S(Rc_int, addrArray, (Word_t) i);

    for(i = 0; i < 100000; i++){
	J1T(Rc_int, addrArray, (Word_t) i);
	if(!Rc_int){
	    printf("Something bad happened\n");
	    return -1;
	}
    }

    return 0;
}
