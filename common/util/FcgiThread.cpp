#include <string>
#include "util/FcgiRequest.h"
#include "util/FcgiThread.h"
#include "util/FcgiRequest.h"

using ssd::FcgiThread;
using namespace std;

FcgiThread::FcgiThread(FcgiServer* fcgi_server, int sock_fd) 
  :  fcgi_server_(fcgi_server), sock_fd_(sock_fd) {
    thread_alive = false;
    pthread_create(&m_tid, NULL, run, (void *)this);
    thread_alive = true;

}

int FcgiThread::wait() {
    return pthread_join(m_tid, NULL);
}

FcgiThread::~FcgiThread() {
	if (thread_alive) {
       pthread_kill(m_tid, 0);
	}
}

void* FcgiThread::run(void *arg) {
	FcgiThread* f_thread = (FcgiThread*)arg;
	f_thread->execute();

	return NULL;
}

void FcgiThread::execute() {
    struct timeval start_tv;
    struct timeval end_tv;
    long long cost;
    FCGX_Request request;
    if (FCGX_InitRequest(&request, sock_fd_, 0) != 0) {
        //BFC_LOG_ERROR(LOGGER_ERROR, message("FcgiThread::execute InitRequest error fd:")
          //      << sock_fd_);
        return;
    }
    //PerformanceTimer pt(__FILE__, __func__, query);
    while(FCGX_Accept_r(&request) >= 0) {
        gettimeofday(&start_tv, NULL);
		std::string query(FCGX_GetParam("QUERY_STRING", request.envp));
        //query = UriDecode(query);
        RequestPtr req(fcgi_server_->create(&request));
        if (req) {
            try{
                req->response();
                gettimeofday(&end_tv, NULL);
                cost = (end_tv.tv_sec - start_tv.tv_sec)*1000000 + end_tv.tv_usec - start_tv.tv_usec;
         //       BFC_LOG_INFO(LOGGER_SERVER, message("FcgiThread::execute query: ") << query << " , cost: " << cost/1000 << "ms");

            }catch(std::exception& e){
      //          BFC_LOG_ERROR(LOGGER_ERROR, message("FcgiThread::execute exception:") 
        //                << e.what());
            }
        } else {
     //       BFC_LOG_ERROR(LOGGER_ERROR, message("FcgiThread::run unhandled request."));
            FCGX_FPrintF(request.out, "Content-type: text/html\r\n\r\n");
        }
        FCGX_Finish_r(&request); // FCGX_Accept_r will call this function
    }
    //BFC_LOG_ERROR(LOGGER_ERROR, message("FcgiThread::run FCGX_Accept_r error"));
}
