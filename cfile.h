#pragma once
#include <stddef.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

char *file_load(const char *path, size_t *len);
bool file_write(const char *path, const char *data, size_t len);
bool is_dir(const char *path);
bool is_file(const char *path);
long file_modified(const char *path);



#define with_dir(dir_name, path)					\
	for(DIR * dir_name = opendir(path); dir_name;)			\
	for(int __func__ ## __LINE__ ## _with_dir_n = 0;		\
	    __func__ ## __LINE__ ## _with_dir_n < 3;			\
	    __func__ ## __LINE__ ## _with_dir_n ++)			\
		if(__func__ ## __LINE__ ## _with_dir_n >= 1) {		\
			closedir(dir_name);				\
			dir_name = 0;					\
			break;						\
		}							\
		else							\
			for(;__func__ ## __LINE__ ## _with_dir_n < 1; __func__ ## __LINE__ ## _with_dir_n ++)
 
#define dir_each(dir, item)			\
	for(struct dirent * item = readdir((dir)); item; item = readdir((dir))) \
		if(!strcmp(item->d_name, ".") || !strcmp(item->d_name, "..")) \
			continue; \
		else


#define file_lines(line, len, file)					\
	for (FILE * __func__##__LINE__##file_lines_n = fopen(file, "r"); \
	     __func__##__LINE__##file_lines_n;				\
	     fclose(__func__##__LINE__##file_lines_n), __func__##__LINE__##file_lines_n = 0) \
		for(size_t len = 0; ! len; len = 1)			\
			for(char * line = 0; !line && ! len; free(line), line = line ? 0 : (void*)1) \
				for(len = -1; !line ; len = 0)			\
					if(getline(&line, &len, __func__##__LINE__##file_lines_n) < 0) \
					{ free(line); line = 0; break; } \
					else



#ifdef LIB_IMPLEMENTATION


char *file_load(const char *path, size_t *len)
{
	FILE *f = fopen(path, "r");
	if(!f)
		return 0;
	fseek(f, 0, SEEK_END);
	*len = ftell(f);
	if(!(*len)) {
		fclose(f);
		return 0;
	}
	rewind(f);
	char *b = malloc(*len);
	if(!b) {
		fclose(f);
		abort();
	}
	if(*len != fread(b, 1, *len, f)) {		
		fclose(f);
		free(b);
		return 0;
	}
	fclose(f);
	return b;
}





bool file_write(const char *path, const char *data, size_t len)
{
	FILE *f = fopen(path, "w");
	if(!f)
		return 0;

	bool ret = true;
	if(len != fwrite(data, 1, len, f)) {
		ret = false;
	}
	fclose(f);
	return ret;
}


bool is_dir(const char *path)
{
	struct stat buf;
	if(stat(path, &buf))
		return false;
	return S_ISDIR(buf.st_mode);
}

bool is_file(const char *path)
{
	if(!path)
		return 0;
	
	struct stat buf;
	if(stat(path, &buf))
		return false;

	return S_ISREG(buf.st_mode);
}

long file_modified(const char *path)
{
	if(!path)
		return -1;
	
	struct stat buf;
	if(stat(path, &buf))
		return -1;

	return buf.st_mtime;
}

#endif
