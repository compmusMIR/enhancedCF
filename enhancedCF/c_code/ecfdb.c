// build a database from given ratings
// 2 trees: 
//      song-user-rating MATRIX (containing per song ratings and song/user -> ratings)
//      user-rating TREE (containing per user ratings)

#define _GNU_SOURCE
#include <search.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void *matrixroot = NULL;
//void *userroot = NULL;
void *maproot = NULL; 

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
        free(nm);
    }
    return resnm->id;
}

void mapwalkaction(const void *nodep, const VISIT which, const int depth)
{
    struct namemap_t *nm;

    switch (which) {
    case preorder:
        break;
    case postorder:
        nm = * (struct namemap_t **) nodep;
        printf("name: %s id: %d", nm->name, nm->id);
		break;
    case endorder:
        break;
    case leaf:
        nm = * (struct namemap_t **) nodep;
        printf("name: %s id: %d", nm->name, nm->id);
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
    resms = * (struct matrixsong_t **) tsearch((void *) ms, &matrixroot, compare_matrixsongid);
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

    resmu = * (struct matrixuser_t **) tsearch((void *) mu, &(resms->uroot), compare_matrixuserid);
    if (resmu->rating != 0){
        // entry already existed
        free(mu);
    }
    resmu->rating += rating;

    //printf("song %d - user %d- rating %d\n", songid, userid, rating);
}

int main(int argc, char *argv[])
{
    FILE *ifilestream;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    struct rating_t *rat;
    int ratcount;

    if ((ifilestream = fopen(argv[1], "r")) == NULL)
        fprintf(stderr, "can't open %s", argv[1]);

    // read the file line by line and build databases (trees)
    while ((read = getline(&line, &len, ifilestream)) != -1)
    {
        // split line in user, song and count
        rat = parse_input_line(line);
        ratcount++;

        //insert username and songname in maptree, and get id
        int userid = getmapid(rat->username);
        int songid = getmapid(rat->songname);

        //printf("user %d\tsong %d\n", userid, songid);

        //insert song-user-rating in matrix and update song rating count
        addsonguserrating(songid, userid, rat->count);      
        //insert user usertree and update user rating count
	}
  
	if(maproot == NULL)
		printf("no entries.\n"); 
	
	//twalk(maproot, mapwalkaction);
	 
    free(line);
    exit(EXIT_SUCCESS);
}

