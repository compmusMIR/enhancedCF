// build a database from given ratings
// 2 trees: 
//      song-user-rating MATRIX (containing per song ratings and song/user -> ratings)
//      user-rating TREE (containing per user ratings)

#define _GNU_SOURCE
#include <search.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static void *matrixroot = NULL;
static void *userroot = NULL;
static void *maproot = NULL; 

static int num_users, num_songs;

struct rating_t {
    char *songname;
    char *username;
    unsigned short count;
};

struct matrixsong_t {
	int id;
	int rating;
	void *uroot; 	// here goes the (user, rating) subtree
};

struct matrixuser_t {
	int id; 
	unsigned short rating;
};

struct userrating_t {
	int id; 
	int rating;
};

struct namemap_t {
	char *name;
	int id;
};

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

int compare_mapname(const void *p1, const void *p2)
{
    const struct namemap_t *nm1, *nm2;
    nm1 = (const struct namemap_t *) p1;
    nm2 = (const struct namemap_t *) p2;
    return strcmp(nm1->name, nm2->name);
}

int compare_matrixsongid(const void *p1, const void *p2)
{
    const struct matrixsong_t *nm1, *nm2;
    nm1 = (const struct matrixsong_t *) p1;
    nm2 = (const struct matrixsong_t *) p2;
    return (nm1->id < nm2->id) ? -1 : (nm1->id > nm2->id);
}

int compare_matrixuserid(const void *p1, const void *p2)
{
    const struct matrixuser_t *nm1, *nm2;
    nm1 = (const struct matrixuser_t *) p1;
    nm2 = (const struct matrixuser_t *) p2;
    return (nm1->id < nm2->id) ? -1 : (nm1->id > nm2->id);
}

int compare_userid(const void *p1, const void *p2)
{
    const struct userrating_t *nm1, *nm2;
    nm1 = (const struct userrating_t *) p1;
    nm2 = (const struct userrating_t *) p2;
    return (nm1->id < nm2->id) ? -1 : (nm1->id > nm2->id);
}

int getmapid(char* name)
{
    static int idcount = 1;
    
    struct namemap_t *nm, *resnm; 
    
    nm = malloc(sizeof(struct namemap_t));
    nm->name = name;
    nm->id = 0;
    resnm = * (struct namemap_t **) tsearch((void *) nm, &maproot, compare_mapname);
    if (resnm->id == 0){
        // was new inserted
        resnm->id = idcount;
        idcount++;
    } else {
        free(nm->name);
        free(nm);
    }
    return resnm->id;
}

void matrixuserwalkaction(const void *nodep, const VISIT which, const int depth)
{
    struct matrixuser_t *mu;

    switch (which) {
    case preorder:
        break;
    case postorder:
        mu = * (struct matrixuser_t **) nodep;
        printf("id: %d rating: %d", mu->id, mu->rating);
		break;
    case endorder:
        break;
    case leaf:
        mu = * (struct matrixuser_t **) nodep;
        printf("id: %d rating: %d", mu->id, mu->rating);
        break;
    }
}

void matrixwalkaction(const void *nodep, const VISIT which, const int depth)
{
    //struct matrixsong_t *ms;

    switch (which) {
    case preorder:
        break;
    case postorder:
        num_songs++;
        //ms = * (struct matrixsong_t **) nodep;
        //printf("songid: %d ratings: %d { ", ms->id, ms->rating);
        //twalk(ms->uroot, matrixuserwalkaction);
        //printf(" }\n");
		break;
    case endorder:
        break;
    case leaf:
        num_songs++;
        //ms = * (struct matrixsong_t **) nodep;
        //printf("songid: %d ratings: %d { ", ms->id, ms->rating);
        //twalk(ms->uroot, matrixuserwalkaction);
        //printf(" }\n");
        break;
    }
}

void userwalkaction(const void *nodep, const VISIT which, const int depth)
{
    //struct userrating_t *ur;

    switch (which) {
    case preorder:
        break;
    case postorder:
        num_users++;
        //ur = * (struct  userrating_t **) nodep;
        //printf("id: %d\tnumrat: %d\n", ur->id, ur->rating);
		break;
    case endorder:
        break;
    case leaf:
        num_users++;
        //ur = * (struct  userrating_t **) nodep;
        //printf("id: %d\tnumrat: %d\n", ur->id, ur->rating);
        break;
    }
}

void addsonguserrating(int songid, int userid, int rating)
{
    struct matrixsong_t *ms, *resms;
	struct matrixuser_t *mu, *resmu;

    //insert/find song
    ms = malloc(sizeof(struct matrixsong_t));
	ms->id = songid;
	ms->rating = 0;
	ms->uroot = NULL;
    
    resms = * (struct matrixsong_t **) 
        tsearch((void *) ms, &matrixroot, compare_matrixsongid);
    
    if (resms->rating != 0){
        // entry already existed
        free(ms);
    }
    // set/update rating
    resms->rating += rating;

    // insert user + rating
    mu = malloc(sizeof(struct matrixuser_t));
    mu->id = userid;
    mu->rating = 0;

    resmu = * (struct matrixuser_t **) 
        tsearch((void *) mu, &(resms->uroot), compare_matrixuserid);
    
    if (resmu->rating != 0){
        // entry already existed
        free(mu);
    }
    resmu->rating += rating;

    //printf("song %d - user %d- rating %d\n", songid, userid, rating);
}

void adduserrating(int userid, unsigned short rating)
{
    struct userrating_t *ur, *resur;
    ur = malloc(sizeof(struct userrating_t));
	ur->id = userid;
	ur->rating = 0;
    
    resur = * (struct userrating_t **) 
        tsearch((void *) ur, &userroot, compare_userid);
    
    if (resur->rating != 0){
        // entry already existed
        free(ur);
    }
    // set/update rating
    resur->rating += rating;
}

void free_mapnode(void *entry)
{
    struct namemap_t *nm = (struct namemap_t *) entry;
    free(nm->name);
    free(nm);
}


void fill_db (char *filename)
{
    FILE *ifilestream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    struct rating_t *rat = NULL;

    if ((ifilestream = fopen(filename, "r")) == NULL)
        fprintf(stderr, "can't open %s", filename);

    // read the file line by line and build databases (trees)
    while ((read = getline(&line, &len, ifilestream)) != -1)
    {
        // split line in user, song and count
        rat = parse_input_line(line);
        //insert username and songname in maptree, and get id
        int userid = getmapid(rat->username);
        int songid = getmapid(rat->songname);
        //insert song-user-rating in matrix and update song rating count
        addsonguserrating(songid, userid, rat->count);      
        //insert user usertree and update user rating count
        adduserrating(userid, rat->count);
    
        free(rat);
    }
    // map name -> id is no longer necessary
    tdestroy(maproot, free_mapnode);
    free(line);
}

int get_num_users(void)
{
    num_users = 0;
    twalk(userroot, userwalkaction);
    return num_users;
}

int get_num_songs(void)
{
    num_songs = 0;
    twalk(matrixroot, matrixwalkaction);
    return num_songs;
}
