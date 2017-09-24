#ifndef _PATH_H_
#define _PATH_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _HAVE_DIRLIST_TYPE
#define _HAVE_DIRLIST_TYPE
typedef struct dirlist_s {
	struct dirlist_s *prev;
	struct dirlist_s *next;
	char* name;
	int dir;
} dirlist_t;
#endif

/* defined in path.c */
int path_init(void);
void path_exit(void);
int path_custom_setter(char* p);
int path_screenshot_setter(char* p);
int path_world_setter(char* p);
char* path_get(char* type, char* file, int must_exist, char* buff, int size);
int path_exists(char* path);
int path_create(char* type, char* file);
int path_remove(char* type, char* path);
dirlist_t *path_dirlist(char* type, char* path);

#ifdef __cplusplus
}
#endif

#endif
