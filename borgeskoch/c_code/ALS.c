// compute:
// v - which latent feature a song contains
// u - extend to witch a user likes latent factors

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct rating {
    char *song;
    char *user;
    int count;
};

int compareuser(const void *r1p, const void *r2p)
{
    const struct rating *r1, *r2;

    r1 = (const struct rating *) r1p;
    r2 = (const struct rating *) r2p;

    if ( strcmp(r1->user, r2->user) != 0 )
        return 1;

    return 0;
}

struct rating *parse_input_line(char *line)
{
    char *token;
    struct rating *rat = malloc(sizeof(struct rating));
    
    token = strsep(&line, "\t");
    rat->user = strdup(token);            
    
    token = strsep(&line, "\t");
    rat->song = strdup(token);            

    token = strsep(&line, "\t");
    rat->count = atoi(token);            

    return rat;
}

void print_rating(struct rating *r)
{
    printf("%s\t%s\t%d\n", r->user, r->song, r->count);
}

int main(int argc, char *argv[])
{
    //void *musicroot = NULL;
    //void *userroot = NULL;

    FILE *ifilestream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    struct rating *r;

    if ((ifilestream = fopen(argv[1], "r")) == NULL)
        fprintf(stderr, "can't open %s", argv[1]);
    

    while ((read = getline(&line, &len, ifilestream)) != -1) {
        
        // split line in user, song and count
        r = parse_input_line(line);
        
        print_rating(r);

        //add song + count to userdb (avl tree)
        
        
        //add user + count to musicdb (avl tree)
        //
    }
    
    free(line);

    exit(EXIT_SUCCESS);
}
