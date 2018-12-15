#ifndef _SSD_FCGI_REQUEST_H_
#define _SSD_FCGI_REQUEST_H_

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <fcgiapp.h>

namespace ssd {

class Request {
 public:
  Request(FCGX_Request* r) : fcgi_out_(r->out) {
    Init(r);
  }
  Request(){}
  virtual ~Request() {}
  std::string host() const {
    return GetParamFix("HTTP_HOST");
  }

  std::string path() const {
    return path_;
  }

  std::string body() const {
    return body_;
  }
  bool isConnect() const {
    if (fcgi_out_) 
      return true;
    else
      return false;
  }
  void writeResponse(const std::string& resp_data) {
    //FCGX_FPrintF(fcgi_out_, resp_data.c_str());
    FCGX_PutS(resp_data.c_str(), fcgi_out_);
  }

  void writeStrResponse(const std::string& resp_data) {
    //FCGX_FPrintF(fcgi_out_, resp_data.c_str());
    FCGX_PutStr(resp_data.c_str(), resp_data.length(), fcgi_out_);
  }

  int GetCookieInt(const std::string& key, int dft) const;
  // return the value of key in cookies
  std::string cookie(const std::string& key) const { 
    std::map<std::string, std::string>::const_iterator it = _cookies.find(key);
    if (it != _cookies.end()) {
      return it->second;
    }
    return "";
  }
  // return the value of key in properties
  std::string getProperty(const std::string& key) const;
  bool getProperty(const std::string& key,std::string &value) const;
  int getPropertyInt(const std::string& key, int dft = -1) const;
  double getPropertyDouble(const std::string& key, double dft = -1.0) const;
  int64_t getPropertyLong(const std::string& key, int64_t dft = -1) const;

  std::map<std::string, std::string> cookies() const {
    return _cookies;
  }

  std::map<std::string, std::string> getProperties() const {
    return _props;
  }

  std::string GetRemoteAddr() const;

  std::string UriDecode(const std::string & sSrc) const;
  void jsonEscape(std::string& str);
  bool ParseKeyValue(const char * data, const char * key_seps, char kv_sep,
                     std::map<std::string, std::string> & pairs);

  virtual bool response();
protected:
  void Init(FCGX_Request* r);
  std::string method_;
  std::string path_;
  std::string host_;
  std::string body_;
  std::string query_;
  std::string cookie_string_;
  std::map<std::string, std::string> _cookies;
  std::map<std::string, std::string> _props;
  std::string post_body_;
  FCGX_Stream * fcgi_out_;
  std::map<std::string, std::string> param_map_;
  void InitParamMap(FCGX_Request* r);
  std::string GetParamFix(const std::string & name) const;
};

typedef boost::shared_ptr<Request> RequestPtr;

class ControllerFactory {
 public:
  virtual RequestPtr create(FCGX_Request* r) = 0;
  virtual ~ControllerFactory() {}
  void debugRequestInfo(FCGX_Request* r);
  
};

typedef boost::shared_ptr<ControllerFactory> ControllerFactoryPtr;

}

#endif

