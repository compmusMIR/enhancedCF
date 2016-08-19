// compute:
// v - which latent feature a song contains
// u - extend to witch a user likes latent factors


#include "gl_avltreehash_list.h"

struct entry {
    char* music;
    char* user;
    int count;
};

int compareuser(const void *e1p, const void *e2p)
{
    const struct entry *e1, *e2;
    int last, first;

    e1 = (const struct employee *) e1p;
    e2 = (const struct employee *) e2p;

    if ((last = strcmp(e1->user, e2->user)) != 0)
        return last;

    

}

int main(int argc, char *argv[])
{
    void *musicroot = NULL;
    void *userroot = NULL;

    FILE *ifilestream;
    size_t len = 0;
    ssize_t read;


    if ((ifilestream = fopen(iargv[1], "r")) == NULL)
        fprintf(stderr, "can't open %s", argv[1]);

    while ((read = getline(&line, &len, ifilestream)) != -1) {

        // split line in user, song and count
        
        //add song + count to userdb (avl tree)
        //add user + count to musicdb (avl tree)
        //
    
    }


}
