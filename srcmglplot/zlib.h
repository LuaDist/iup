/* Dummy module to avoid the inclusion of the ZLIB modules */

#ifndef ZLIB_H
#define ZLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/*
typedef void* gzFile;

gzFile gzopen(const char *path, const char *mode);
int gzread(gzFile file, void* buf, unsigned len);
int gzprintf(gzFile file, const char *format, ...);
char* gzgets(gzFile file, char *buf, int len);
int gzgetc(gzFile file);
int gzclose(gzFile file);
*/

#define gzFile FILE*
#define gzopen fopen
#define gzread(_file, _buf, _len) fread(_buf, 1, _len, (FILE*)_file)
#define gzprintf fprintf
#define gzgets(_file, _buf, _len) fgets(_buf, _len, (FILE*)_file)
#define gzgetc(_file) fgetc((FILE*)_file)
#define gzclose(_file) fclose((FILE*)_file)

#ifdef __cplusplus
}
#endif

#endif /* ZLIB_H */
