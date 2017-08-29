#ifndef _MPTCONST_H
#define _MPTCONST_H

#ifndef likely
#define likely(x) __builtin_expect(!!(x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define PROTOCOL_HTTP  "HTTP"
#define PROTOCOL_FTP   "FTP"
#define PROTOCOL_RMDB  "RMDB"
#define PROTOCOL_TCP   "TCP"
#define PROTOCOL_UDP   "UDP"

#endif

