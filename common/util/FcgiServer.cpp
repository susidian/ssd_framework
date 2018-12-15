#include <boost/scoped_ptr.hpp>
#include "util/FcgiServer.h"
#include "util/FcgiThread.h"

using ssd::FcgiServer;
using ssd::RequestPtr;

FcgiServer::FcgiServer(const std::string& sock, int thread_count, int stack_size) 
  : socket_(sock), thread_count_(thread_count), stack_size_(stack_size) {
}

bool FcgiServer::start() {
  if (FCGX_Init()) {
    //BFC_LOG_ERROR(LOGGER_ERROR, message("FcgiServer::start FCGX_Init err"));
    return false;
  }

  int sock_fd = FCGX_OpenSocket(socket_.c_str(), 512);
  if (sock_fd < 0){
    // BFC_LOG_ERROR(LOGGER_ERROR, message("FcgiServe::start FCGX_OpenSocket err"));
    return false;
  }
  //BFC_LOG_INFO(LOGGER_SERVER, message("FcgiServer will create thread"));
  for(int i = 0; i< thread_count_; ++i) {
    FcgiThread* fcgiThreadPtr = new FcgiThread(this, sock_fd);
    //fcgiThreadPtr->execute();
    //ThreadPtr threadPtr(new boost::thread(boost::bind(&FcgiThread::run, 
    //                                                   fcgiThreadPtr.get())));
    //set stack size,the default size 8M is too big
    //BFC_LOG_INFO(LOGGER_SERVER, message("FcgiServe::start thread :") << i);
    threads_.push_back(fcgiThreadPtr);
  }
  for(int i = 0; i< thread_count_; ++i) {
    threads_[i]->wait();
  }
  return true;
}

RequestPtr FcgiServer::create(FCGX_Request* r){
  RequestPtr p;
  for(size_t i = 0; i < controller_factories_.size(); ++i) {
    p = controller_factories_[i]->create(r);
    if (p) {
      return p;
    }
  }
  return p;
}

int FcgiServer::registerControllerFactory(ControllerFactoryPtr factory) {
  controller_factories_.push_back(factory);
  return 0;
}
