//
//  main.c
//  test
//
//  Created by Nicholas Bouckaert on 3/8/18.
//  Copyright © 2018 Nicholas Bouckaert. All rights reserved.
//

#include <stdio.h>

int main(void)
{
    int a;
    a = fork();
    if( a == 0)
    {
        a = fork();
    }
    
    
    a = fork();
    if( a != 0 )
        printf("(ID: %d) Hello world\n", a);
    
    return 0;
}
