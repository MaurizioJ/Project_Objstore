#if !defined(LIB_CLIENT) /*libreria client*/
#define LIB_CLIENT

int os_connect(char *name);
int os_store(char *name, void *block, size_t len);
void *os_retrieve (char *name);
int os_delete(char *name);
int os_disconnect();

#endif /* FINE LIB_CLIENT */