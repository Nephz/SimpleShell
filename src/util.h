typedef enum {
  Nothing,
  Msg,
  Error
} errnoflag;

void nullchecker(void *p, char* msg, errnoflag option);
void* d_realloc(void* buf, size_t size);