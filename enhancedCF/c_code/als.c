// compute:
// v - which latent feature a song contains
// u - extend to witch a user likes latent factors

#include <stdio.h>
#include <stdlib.h>

#include "ecfdb.h"

int main(int argc, char *argv[])
{
    fill_db(argv[1]);    
    printf("DB filled with songs, users and ratings.\n");
    printf("number of users: %d\n", get_num_users());
    printf("number of songs: %d\n", get_num_songs());
    
    exit(EXIT_SUCCESS);
}
