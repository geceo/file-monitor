#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <dirent.h>
#include <sys/inotify.h>

#include "logs.h"

#define WATCH_FLAGS IN_CREATE|IN_DELETE|IN_MODIFY|IN_MOVE_SELF|IN_MOVED_FROM|IN_MOVED_TO

int register_new_watch(int fd, GHashTable *watches, char *path){ 
	int handle;

	printf("adding watch to: %s\n",path);
	if ((handle = inotify_add_watch(fd, path, WATCH_FLAGS|IN_EXCL_UNLINK)) < 0)
		return -1;

	g_hash_table_insert(watches,GUINT_TO_POINTER(handle),path);
	return 0;
}



int register_new_watch_recurse(int fd, GHashTable *watches, char *path)
{	
	struct dirent *de;
	DIR *dir = NULL;

	dir = opendir(path);
	if (!dir) {
		perror("opendir() failed");
		return -1;
	}
	
	while((de = readdir(dir))) {
		/*  Skip . and .. to avoid infinite loop and parent watching */
		if ( !strncmp(".", de->d_name, strlen(de->d_name)) ||  !strncmp("..", de->d_name, strlen(de->d_name)))
			break;

		if ( de->d_type == DT_DIR ) {
			char *rpath;

			rpath = malloc(strlen(path) + strlen(de->d_name) + 2);
			if (path[strlen(path)-1] == '/')
				sprintf(rpath,"%s%s",path,de->d_name);
			else 
				sprintf(rpath,"%s/%s",path,de->d_name);

			register_new_watch_recurse(fd,watches,rpath);
		}
	}

	closedir(dir);
	return register_new_watch(fd,watches,path);
}



/* XXX: rewrite this shitty code with some libevent one */
int main_loop(int fd, GHashTable *watches)
{
	int len;
	/* basic size of struct inotify_event is 16 */
	char *buf;

	buf = calloc(1024,sizeof(uint8_t));
	while ( (len = read(fd,buf,1024)) > 0) {
		char what[1024];
		char *path;
		int i=0, offset=0;
		struct inotify_event *ev = NULL;

		while (len >= sizeof(struct inotify_event)) {
			int tmp_len;
			char *base;

			ev = (struct inotify_event *) (buf+offset);

			/* What happened ? */
			memset(what,0,1024);
			if ((ev->mask & IN_CREATE) == IN_CREATE)
				strncat(what,"created ",1023);
			if ((ev->mask & IN_DELETE) == IN_DELETE)
				strncat(what,"deleted ",1023);
			if ((ev->mask & IN_MODIFY) == IN_MODIFY)
				strncat(what,"modified ",1023);
			if ((ev->mask & IN_MOVE_SELF) == IN_MOVE_SELF)
				strncat(what,"move self ",1023);
			if ((ev->mask & IN_MOVED_FROM) == IN_MOVED_FROM)
				strncat(what,"move from",1023);
			if ((ev->mask & IN_MOVED_TO) == IN_MOVED_TO)
				strncat(what,"move to",1023);

			base = g_hash_table_lookup(watches,GUINT_TO_POINTER(ev->wd));

			/* Notify user */
			if (ev->len) {
				path = malloc(strlen(base) + ( ev->len ? ev->len + 1 : 0 ) + 1);
				sprintf(path,"%s/%s",base,ev->name);
			} else {
				path = strdup(base);
			}
			printf("%s %s\n",path,what);

			/* Here, we'll probably call plugins or something like that */

						
			/* offset & co stuff */
			i++;
			tmp_len = sizeof(struct inotify_event) + ev->len;
			len -= tmp_len;
			offset += tmp_len;

		}

		if (len !=0) 
			printf("datas weren't all consumed, probably read half a struct :/\n");
	}

	return 0;
}



int main(int ac, char **av)
{
	int i,nfd = 0;
	char *msg = NULL;
	GHashTable *watches = NULL;

	if(!(nfd = inotify_init())) {
		msg = "failed to initialize inotify";
		goto failure;
	}
		
	watches = g_hash_table_new(NULL,NULL);

	for (i=1; i<ac; i++) {
		/* First remove trailing / if present, cause we add it :p */
		if (av[i][strlen(av[i]) -1] == '/')
			av[i][strlen(av[i]) -1] = '\0';

		/* prefixing path with r: mean that we want recursion on it !*/
		if (av[i][0] == 'r' && av[i][1] == ':') {
			struct stat st;
			char *path = av[i]+2;

			if (stat(path, &st) != 0) {
				printf("%s doesn't exists !\n",path);
				break;
			}

			if (!S_ISDIR(st.st_mode)) {
				printf("%s isn't a directory, no recursion !\n",path);
				register_new_watch(nfd,watches,path);
			} else {
				register_new_watch_recurse(nfd,watches,path);
			}
		} else {
			register_new_watch(nfd,watches,av[i]);
		}
	}

	main_loop(nfd,watches);

failure:
	if (msg)
		printf("%s %s",msg,strerror(errno));

	return (msg ? 255 : 0);

}
