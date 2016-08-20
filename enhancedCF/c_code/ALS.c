// compute:
// v - which latent feature a song contains
// u - extend to witch a user likes latent factors

#define _GNU_SOURCE
#include <search.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct rating_t {
    char *songname;
    char *username;
    int count;
};

struct matrixuser_t {
    char *name;
    void *sroot;     // here goes the (song,count) subtree
};

struct userratcount_t {
    char *name;
    int count;     
};

struct matrixsong_t {
    char *name;
    int count; 
};


int num_users_post, num_songs_post, num_ratings_post;



int compare_username(const void *p1, const void *p2)
{
    const struct userratcount_t *ur1, *ur2;

    ur1 = (const struct userratcount_t *) p1;
    ur2 = (const struct userratcount_t *) p2;

    return strcmp(ur1->name, ur2->name);
}

int compare_matrixusername(const void *p1, const void *p2)
{
    const struct matrixuser_t *mu1, *mu2;

    mu1 = (const struct matrixuser_t *) p1;
    mu2 = (const struct matrixuser_t *) p2;

    return strcmp(mu1->name, mu2->name);
}


int compare_songname(const void *p1, const void *p2)
{
    const struct matrixsong_t *ms1, *ms2;

    ms1 = (const struct matrixsong_t *) p1;
    ms2 = (const struct matrixsong_t *) p2;

    return strcmp(ms1->name, ms2->name);
}

struct rating_t *parse_input_line(char *line)
{
    char *token;
    struct rating_t *rat = malloc(sizeof(struct rating_t));
    
    token = strsep(&line, "\t");
    rat->username = strdup(token);            
    
    token = strsep(&line, "\t");
    rat->songname = strdup(token);            

    token = strsep(&line, "\t");
    rat->count = atoi(token);            

    return rat;
}

void delete_rating(void *r)
{
    struct rating_t *rat = (struct rating_t *) r;
    free(rat);
}

void print_rating(struct rating_t *r)
{
    printf("%s\t%s\t%d\n", r->username, r->songname, r->count);
}

void songwalkaction(const void *nodep, const VISIT which, const int depth)
{
    //struct matrixsong_t *msong;

    switch (which) {
   	case preorder:
    	break;
    
	case postorder:
        //msong = * (struct matrixsong_t **) nodep;
        //printf("song %s counted with %d ratings.\n", msong->name, msong->count);
		num_songs_post++;
        break;
    
	case endorder:
    	break;
    
	case leaf:
        //msong = * (struct matrixsong_t **) nodep;
        //printf("song %s counted with %d ratings.\n", msong->name, msong->count);
		num_songs_post++;
        break;
    }
}

void usersongwalkaction(const void *nodep, const VISIT which, const int depth)
{
    switch (which) {
   	case preorder:
    	break;
    
	case postorder:
		num_ratings_post++;
        break;
    
	case endorder:
    	break;
    
	case leaf:
		num_ratings_post++;
        break;
    }
}

void userwalkaction(const void *nodep, const VISIT which, const int depth)
{
    struct matrixuser_t *muser;

    switch (which) {
   	case preorder:
    	break;
    
	case postorder:
		num_users_post++;
    	muser = * (struct matrixuser_t **) nodep;
		twalk(muser->sroot, usersongwalkaction);
        break;
    
	case endorder:
    	break;
    
	case leaf:
		num_users_post++;
    	muser = * (struct matrixuser_t **) nodep;
		twalk(muser->sroot, usersongwalkaction);
        break;
    }
}

void userratwalkaction(const void *nodep, const VISIT which, const int depth)
{
    struct userratcount_t *user;

    switch (which) {
   	case preorder:
    	break;
    
	case postorder:
		num_users_post++;
    	user = * (struct userratcount_t **) nodep;
        printf("user %s with %d ratings.\n", user->name, user->count);
        break;
    
	case endorder:
    	break;
    
	case leaf:
    	user = * (struct userratcount_t **) nodep;
        printf("user %s with %d ratings.\n", user->name, user->count);
		num_users_post++;
        break;
    }
}


int getrating(void *matrixroot, char* username, char* songname)
{
    void *res;
    
    struct matrixuser_t *mu, *resmu;
    struct matrixsong_t *ms, *resms;

    

    printf("getrating entered.\n");
    // find user in matrix
    mu = malloc(sizeof(struct matrixuser_t));
    mu->name = username;
    mu->sroot = NULL;
    printf("getrating: got mem for user.\n");
    res = tfind((void *) mu, &matrixroot, compare_matrixusername);
    printf("getrating: returned from tfind user.\n");
    
    if (res == NULL)
        return 0;
    
    resmu = * (struct matrixuser_t **) res; 
 
    // user found; now search subtree of song ratings
    ms = malloc(sizeof(struct matrixsong_t));
    ms->name = songname;
    ms->count = 0;
    res = tfind((void *) ms, &(resmu->sroot), compare_songname);
    printf("getrating: returned from tfind song.\n");
    
    if (res == NULL)
        return 0;

    resms = * (struct matrixsong_t **) res;

    return resms->count;
}

int main(int argc, char *argv[])
{
    void *matrixroot = NULL;
    void *songroot = NULL;
    void *userroot = NULL;

    FILE *ifilestream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;


    struct rating_t *rat;
    struct matrixuser_t *mu, *insmu;
    struct matrixsong_t *ms, *insms;

    struct userratcount_t *ur, *insur;

    int ratcount = 0;

    if ((ifilestream = fopen(argv[1], "r")) == NULL)
        fprintf(stderr, "can't open %s", argv[1]);

    while ((read = getline(&line, &len, ifilestream)) != -1) {
       
        // split line in user, song and count
        rat = parse_input_line(line);

        mu = malloc(sizeof(struct matrixuser_t));
        mu->name = rat->username;
        mu->sroot = NULL;
        
        ms = malloc(sizeof(struct matrixsong_t));
        ms->name = rat->songname;
        ms->count = rat->count;
        
        //add song + count to matrix (tree of trees)
        //search/insert user
        insmu = * (struct matrixuser_t **) tsearch((void *) mu, &matrixroot, compare_matrixusername);
        // insert song at user
        insms = * (struct matrixsong_t **) tsearch((void *) ms, &(insmu->sroot), compare_songname);
        
        //add usercount (num of ratings by user)
        ur = malloc(sizeof(struct userratcount_t));
        ur->name = rat->username;
        ur->count = 0;
        insur = * (struct userratcount_t **) tsearch((void *) ur, &userroot, compare_username);
        insur->count += rat->count;
        
        //add songcount (num of ratings of song)
        ms = malloc(sizeof(struct matrixsong_t));
        ms->name = rat->songname;
        ms->count = 0;
        insms = * (struct matrixsong_t **) tsearch((void *) ms, &songroot, compare_songname);
        insms->count += rat->count;

        ratcount++;
    }
    

    twalk(matrixroot, userwalkaction);
    twalk(songroot, songwalkaction);
    twalk(userroot, userratwalkaction);

    //printf("%d ratings included.\n", ratcount);
    //printf("%d ratings/songs found for %d users.\n", num_ratings_post, num_users_post);
    printf("%d different songs found.\n", num_songs_post);
    
    int count = getrating(matrixroot, "b7815dbb206eb2831ce0fe040d0aa537e2e800f7", "SOZDENG12A8C13C632");
    printf("rating is: %d\n",count);
    
    count = getrating(matrixroot, "b7815dbb206eb28315e0fe040d0aa537e2e800f7", "SODJQXO12A6D4F697D");
    printf("rating is: %d\n",count);
    
    count = getrating(matrixroot, "b7815dbb206eb28315e0fe040d0aa537e2e800f7", "SODJQXO52A6D4F697D");
    printf("rating is: %d\n",count);
    
    
    free(line);
    //tdestroy(musicroot, delete_rating);
    //tdestroy(userroot, delete_rating);
    exit(EXIT_SUCCESS);
}
