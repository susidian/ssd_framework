#ifndef _SSD_FCGI_THREAD_H_
#define _SSD_FCGI_THREAD_H_

#include "FcgiRequest.h"
#include "FcgiServer.h"
#include <fcgi_stdio.h>
#include <boost/noncopyable.hpp>
#include <pthread.h>

namespace ssd {

class FcgiThread {
 public:
  FcgiThread(FcgiServer* fcgi_server, int sock_fd); 
  ~FcgiThread();
  int wait();
 private:
  static void* run(void* arg);
  void execute();
  FcgiServer* fcgi_server_;
  int sock_fd_;
  pthread_t m_tid;
  bool thread_alive;
};

}

#endif
