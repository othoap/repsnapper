#ifndef _COMMS_PRIVATE_H_
#define _COMMS_PRIVATE_H_

/* size_t */
#include <stdlib.h>

#include "comms.h"
#include "serial.h"

#define RR_DEBUG_ALWAYS 0
#define RR_DEBUG_MEDIUM 1
#define RR_DEBUG_HIGH   2

/* Do not change */
#define GCODE_BLOCKSIZE 256
#define SENDBUFSIZE (GCODE_BLOCKSIZE + BLOCK_TERMINATOR_LEN)
#define INITIAL_RECVBUFSIZE 256

typedef struct blocknode {
  struct blocknode *next;
  char *block;
  size_t blocksize;
  long long line;
} blocknode;

struct rr_dev_t {
  rr_proto      proto;
  int           dev_cmdqueue_size;
  rr_reply_fn   reply_cb;   void *reply_cl;
  rr_more_fn    more_cb;    void *more_cl;
  rr_error_fn   error_cb;   void *error_cl;
  rr_wait_wr_fn wait_wr_cb; void *wait_wr_cl;
  rr_log_fn     opt_log_cb; void *opt_log_cl;

  Serial fd;
  /* Line currently being sent */
  unsigned long lineno;

  int        init_send_count;
  int        send_next;
  int        sendsize[RR_PRIO_ALL_QUEUES];
  blocknode *sendhead[RR_PRIO_ALL_QUEUES];
  blocknode *sendtail[RR_PRIO_ALL_QUEUES];
  int        paused[RR_PRIO_ALL_QUEUES];
  char       sendbuf[SENDBUFSIZE];
  rr_prio    sending_prio;
  size_t     sendbuf_fill;
  size_t     bytes_sent;

  char *recvbuf;
  size_t recvbufsize;
  size_t recvbuf_fill;

  size_t sentcachesize;

  int    debug_output;
};

/* Re-send a line number */
int rr_dev_resend (rr_dev device, unsigned long lineno, const char *reply, size_t nbytes);

/* Helper for emitting and returning errors */
rr_error rr_dev_emit_error (rr_dev dev, rr_error err, const char *block, int nbytes);

/* Get flow control right */
void rr_dev_handle_ok (rr_dev dev);

/* Handle start semantics */
int  rr_dev_handle_start (rr_dev dev);

/* print to the log */
void rr_dev_log (rr_dev dev, int level, const char *format, ...);

/* internal queue command allowing explicit line no. */
int rr_dev_enqueue_internal (rr_dev dev, rr_prio priority,
			     const char *block, size_t nbytes,
			     long long line);


#endif
