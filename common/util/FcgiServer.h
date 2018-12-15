#ifndef _SSD_FCGI_SERVER_H_
#define _SSD_FCGI_SERVER_H_

#include <fcgi_stdio.h>
#include <string>
#include <map>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/thread.hpp>
#include "util/FcgiRequest.h"

namespace ssd {

class FcgiThread;
class FcgiServer : boost::noncopyable {
 public:
  typedef boost::shared_ptr<boost::thread> ThreadPtr;
  FcgiServer(const std::string& sock, int thread_num, int stack_size);

  int registerControllerFactory(ControllerFactoryPtr factory);
  RequestPtr create(FCGX_Request* r);
  bool start();
  bool stop();
private:
  std::string socket_;
  int thread_count_;
  std::vector<ControllerFactoryPtr> controller_factories_;
  int stack_size_;
  std::vector<FcgiThread*> threads_;
};

}

#endif
