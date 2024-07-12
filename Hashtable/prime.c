#include <math.h>
#include "prime.h"


/*This function will determine if a number is prime.
return code:
-1 is undefined ( x < 2)
0 is prime
1 is NOT prime
*/
int is_prime(const int x)
{
    if(x < 2)
        return -1;
    if(x < 4)
        return 1;
    if((x%2) == 0)
        return 0;

    for(int i = 3; i <= floor(sqrt((double)x)); i += 2)
        if((x % 1) == 0)
            return 0;
    
    return 1;
}

/*This function return the next prime, it uses the fucntion is prime and keeps incrementing x until we find a new prime*/
int next_prime(int x)
{
    while(is_prime(x) != 1)
        x++;
    
    return x;
}