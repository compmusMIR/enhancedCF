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
    unsigned short count;
};

// user struct used in matrix
struct matrixuser_t {
    int id;
    void *sroot;     // here goes the (song,count) subtree
};

// song struct used in matrix
struct matrixsong_t {
    int id;
    unsigned short count; 
};

struct userratcount_t {
    char *name;
    int id;
    unsigned short count;     
};

// song struct used in song tree
struct songratcount_t {
    char *name;
    int id;
    unsigned short count; 
};

// global counter
int n_users;         // number of users
int n_songs;        //number of songs
int n_ratings;       // number of triples from input


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

    return (mu1->id < mu2->id) ? -1 : (mu1->id > mu2->id);
}


int compare_matrixsongname(const void *p1, const void *p2)
{
    const struct matrixsong_t *ms1, *ms2;

    ms1 = (const struct matrixsong_t *) p1;
    ms2 = (const struct matrixsong_t *) p2;

    return (ms1->id < ms2->id) ? -1 : (ms1->id > ms2->id);
}


int compare_songname(const void *p1, const void *p2)
{
    const struct songratcount_t *ms1, *ms2;

    ms1 = (const struct songratcount_t *) p1;
    ms2 = (const struct songratcount_t *) p2;

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
		n_songs++;
        break;
    
	case endorder:
    	break;
    
	case leaf:
        //msong = * (struct matrixsong_t **) nodep;
        //printf("song %s counted with %d ratings.\n", msong->name, msong->count);
		n_songs++;
        break;
    }
}

void usersongwalkaction(const void *nodep, const VISIT which, const int depth)
{
    switch (which) {
   	case preorder:
    	break;
    
	case postorder:
		n_ratings++;
        break;
    
	case endorder:
    	break;
    
	case leaf:
		n_ratings++;
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
		n_users++;
    	muser = * (struct matrixuser_t **) nodep;
		twalk(muser->sroot, usersongwalkaction);
        break;
    
	case endorder:
    	break;
    
	case leaf:
		n_users++;
    	muser = * (struct matrixuser_t **) nodep;
		twalk(muser->sroot, usersongwalkaction);
        break;
    }
}

void userratwalkaction(const void *nodep, const VISIT which, const int depth)
{
    //struct userratcount_t *user;

    switch (which) {
   	case preorder:
    	break;
    
	case postorder:
    	//user = * (struct userratcount_t **) nodep;
        //printf("user %s with %d ratings.\n", user->name, user->count);
        break;
    
	case endorder:
    	break;
    
	case leaf:
    	//user = * (struct userratcount_t **) nodep;
        //printf("user %s with %d ratings.\n", user->name, user->count);
        break;
    }
}


int getrating(void *matrixroot, int userid, int songid)
{
    void *res;
    
    struct matrixuser_t *mu, *resmu;
    struct matrixsong_t *ms, *resms;

    // find user in matrix
    mu = malloc(sizeof(struct matrixuser_t));
    mu->id = userid;
    mu->sroot = NULL;
    res = tfind((void *) mu, &matrixroot, compare_matrixusername);
    free(mu);
    if (res == NULL)
        return 0;
    
    resmu = * (struct matrixuser_t **) res; 
 
    // user found; now search subtree of song ratings
    ms = malloc(sizeof(struct matrixsong_t));
    ms->id = songid;
    ms->count = 0;
    res = tfind((void *) ms, &(resmu->sroot), compare_songname);
    free(ms);

    if (res == NULL)
        return 0;

    resms = * (struct matrixsong_t **) res;

    return resms->count;
}

int get_user_id(void *root, char *name){

    void *res;
    struct userratcount_t ur, *resur;
    ur.name = name;
    res = tfind((void *)&ur, &root, compare_username);
    if (res == NULL)
        return 0;

    resur = * (struct userratcount_t **) res;

    return resur->id;
}

int get_song_id(void *root, char *name){

    void *res;
    struct songratcount_t sr, *ressr;
    sr.name = name;
    res = tfind((void *)&sr, &root, compare_songname);
    if (res == NULL)
        return 0;

    ressr = * (struct songratcount_t **) res;

    return ressr->id;
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
    struct matrixsong_t *ms;

    struct userratcount_t *ur, *insur;
    struct songratcount_t *sr, *inssr;

    int ratcount, usercount, songcount;

    if ((ifilestream = fopen(argv[1], "r")) == NULL)
        fprintf(stderr, "can't open %s", argv[1]);

    // read the file line by line and build databases (trees)
    while ((read = getline(&line, &len, ifilestream)) != -1) 
    {
        printf("reading rating. ");
        // split line in user, song and count
        rat = parse_input_line(line);
        ratcount++;
        printf("got rating. ");
        //add usercount to USER TREE (num of ratings by user)
        ur = malloc(sizeof(struct userratcount_t));
        ur->name = rat->username;
        ur->id = 0;
        ur->count = 0;
        insur = * (struct userratcount_t **) tsearch((void *) ur, &userroot, compare_username);
        insur->count += rat->count;
        //adding id 
        if (insur->id == 0){
            insur->id = usercount + 1;
            usercount++;
        }
        printf("user inserted. ");

        //add soncount to SONG TREE (num of ratings of song)
        sr = malloc(sizeof(struct songratcount_t));
        sr->name = rat->songname;
        sr->id = 0;
        sr->count = 0;
        inssr = * (struct songratcount_t **) tsearch((void *) sr, &songroot, compare_songname);
        inssr->count += rat->count;
        if (inssr->id == 0){
            inssr->id = songcount +1;
            songcount++;
        }
        printf("song inserted. ");

        //add song + count to USERSONG MATRIX (tree of trees) (num ratings for (user,song))
        mu = malloc(sizeof(struct matrixuser_t));
        mu->id = get_user_id(userroot, rat->username);
        mu->sroot = NULL;
        
        ms = malloc(sizeof(struct matrixsong_t));
        ms->id = get_song_id(songroot, rat->songname);
        ms->count = rat->count;
       
        //search/insert user
        insmu = * (struct matrixuser_t **) tsearch((void *) mu, &matrixroot, compare_matrixusername);
        // insert song at user
        (void) tsearch((void *) ms, &(insmu->sroot), compare_songname);
        
        printf("user/song rating inserted. ");
        

        free(rat);
        printf("rating freeed. \n");
    }
    
    //twalk(matrixroot, userwalkaction);
    //twalk(songroot, songwalkaction);
    //twalk(userroot, userratwalkaction);

    printf("%d triplets entered / ratings registered.\n", ratcount);
    printf("%d different songs and %d users found.\n\n", songcount, usercount);
    
    //user not found
    //int count = getrating(matrixroot, "8f993580642c7b58566a6dd69bcd8053dcd1c716", "SOZDENG12A8C13C632");
    //printf("test1 (should be 0 ) rating is: %d\n",count);
    //
    //// user and song found
    //count = getrating(matrixroot, "cf8289419383259189afe6bb50c5115fd84f1064", "SOZDENG12A8C13C632");
    //printf("test2 (should be 1 ) rating is: %d\n",count);
 
    //// user and song found
    //count = getrating(matrixroot, "b7815dbb206eb2831ce0fe040d0aa537e2e800f7", "SOSQJWM12A6D4F79E0");
    //printf("test3 (should be 6 ) rating is: %d\n",count);
   
    ////song not found
    //count = getrating(matrixroot, "b7815dbb206eb2831ce0fe040d0aa537e2e800f7", "SODJQXO52X6D4F697D");
    //printf("test3 (should be 0 ) rating is: %d\n",count);
    
    
    free(line);
    //tdestroy(musicroot, delete_rating);
    //tdestroy(userroot, delete_rating);
    exit(EXIT_SUCCESS);
}
