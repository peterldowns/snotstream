#include "pkt_types.h"
#include "pkt_utils.h"
#include "lib/ringbuf/ringbuf.h"

#define MAX_MON_CONS 10
#define SERVER /*"localhost:5984"*/"snoplus:snoplustest@snoplus.cloudant.com"
#define DATABASE "pmt_test"

/*
 * Structures
 */
typedef enum {
	EV_BUILDER, // TODO: EV_BUILDER data
	XL3,
	MTC,
	CAEN,
	ORCA,       // TODO: ORCA data
    UNKNOWN,    // TODO: default size
	con_type_max
} con_type;
typedef struct {
    struct bufferevent *bev;
    con_type type;
    char *host;
    int port;
    int valid;
    size_t pktsize;
} connection;
typedef struct {
//{ crate=..., board=..., chan=..., qlx=..., qhl=..., qhs=... } //pmt0000
	uint32_t crate;
	uint32_t board;
	uint32_t chan;
	uint32_t pmt;// = crate*512+board*32+chan
	uint32_t qlx;
	uint32_t qhl;
	uint32_t qhs;
	int count;
} pmt_upls;
	
// Gets the size of a con_type.
// (pkt_size_of[EV_BUILDER]);
size_t pkt_size_of[] =
{
	0,
	sizeof(XL3_Packet),
	sizeof(mtcPacket),
	sizeof(caenPacket),
	0,
	0
};
typedef void(*com_ptr)(char *, void *); // command pointer
typedef struct {
	// holds a list of commands
	char *key;
	com_ptr function;
} command;

// Controller Callbacks
static void controller_read_cb(struct bufferevent *bev, void *ctx);
static void controller_event_cb(struct bufferevent *bev, short events,void *ctx);

// Listener Callbacks
static void listener_accept_cb(struct evconnlistener *listener,
			       evutil_socket_t fd, struct sockaddr *address,
			       int socklen, void *ctx);
static void listener_error_cb(struct evconnlistener *listener, void *ctx);

// Signal Callbacks
void signal_cb(evutil_socket_t sig, short events, void *user_data);

/*
 * Globals
 */
struct event_base *base;
struct evdns_base *dnsbase;
struct evconnlistener *listener;
int have_controller = 0;
CURLM *multi;
struct event timer_event;
int still_running = 0;