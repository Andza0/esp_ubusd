#include <libserialport.h>

#define VID 10c4
#define PID ea60

struct sp_port* get_port_vidpid(char *vid, char *pid);