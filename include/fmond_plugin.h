#ifndef _FMOND_PLUGIN_H
#define _FMOND_PLUGIN_H


typedef struct {
	void *(*init) ();
	int (*directory_update) (char *, uint32_t);
	int (*file_update) (char *, uint32_t);
	void (*cleanup) (void *);

	void *prv;
} fmon_plugin_t;

#endif
