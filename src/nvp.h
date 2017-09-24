#ifndef _NVP_H_
#define _NVP_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _HAVE_NVP_TYPE
#define _HAVE_NVP_TYPE
typedef struct nvp_s {
	struct nvp_s *prev;
	struct nvp_s *next;
	char* name;
	unsigned int h;
	char* value;
	void *data;
} nvp_t;
#endif

/* defined in nvp.c */
void nvp_free(nvp_t **list, int data);
nvp_t *nvp_get(nvp_t **list, char* name);
char* nvp_get_str(nvp_t **list, char* name);
int nvp_get_int(nvp_t **list, char* name);
float nvp_get_float(nvp_t **list, char* name);
int nvp_get_bool(nvp_t **list, char* name);
void *nvp_get_data(nvp_t **list, char* name);
void nvp_set(nvp_t **list, char* name, char* value, void *data);
void nvp_set_int(nvp_t **list, char* name, int value);
void nvp_set_float(nvp_t **list, char* name, float value);

#ifdef __cplusplus
}
#endif

#endif
