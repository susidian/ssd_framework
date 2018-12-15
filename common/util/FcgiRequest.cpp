#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <vector>
#include "FcgiRequest.h"
using namespace std;

namespace ssd {

const char HEX2DEC[256] = {
/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
/* 0 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 1 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 2 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 3 */0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

/* 4 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 5 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 6 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 7 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* 8 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 9 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* A */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* B */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* C */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* D */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* E */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* F */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

//将 sSrc 进行解码，例如把 &amp;&amp;++&#37;&#37 解析成  &&++%% 并返回
std::string Request::UriDecode(const std::string & sSrc) const {
  // Note from RFC1630:  "Sequences which start with a percent sign
  // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
  // for future extension"

  const unsigned char * pSrc = (const unsigned char *) sSrc.c_str();
  const int SRC_LEN = sSrc.length();
  const unsigned char * const SRC_END = pSrc + SRC_LEN;
  const unsigned char * const SRC_LAST_DEC = SRC_END - 2; // last decodable '%'

  char * const pStart = new char[SRC_LEN];
  char * pEnd = pStart;

  while (pSrc < SRC_LAST_DEC) {
    if (*pSrc == '%') {
      char dec1, dec2;
      if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)]) && -1 != (dec2
          = HEX2DEC[*(pSrc + 2)])) {
        *pEnd++ = (dec1 << 4) + dec2;
        pSrc += 3;
        continue;
      }
    }
    *pEnd++ = *pSrc++;
  }

  // the last 2- chars
  while (pSrc < SRC_END)
    *pEnd++ = *pSrc++;

  std::string sResult(pStart, pEnd);
  delete[] pStart;
  return sResult;
}

bool Request::ParseKeyValue(const char * data, const char * key_seps,
    char kv_sep, std::map<std::string, std::string> & pairs) {
  if (!data)
    return false;

  std::vector<std::string> strs;
  boost::split(strs, data, boost::is_any_of(key_seps),
      boost::token_compress_on);
  for (size_t i = 0; i < strs.size(); ++i) {
    size_t pos = strs[i].find_first_of(kv_sep);
    if (pos != std::string::npos) {
      pairs[strs[i].substr(0, pos)] = UriDecode(strs[i].substr(pos + 1));
    } else {
      pairs[strs[i]] = "";
    }
  }
  return true;
}

void Request::jsonEscape(std::string& str) {
  for (std::string::iterator iter = str.begin(); iter != str.end(); ++iter) {
    if ((*iter) == '\'' || (*iter) == '"' || (*iter) == '\\' 
        || (*iter) == '/') {
      iter = str.insert(iter, '\\');
      ++iter;
    }
  }
}

std::string Request::getProperty(const std::string& key) const 
{
    std::map<std::string, std::string>::const_iterator it = _props.find(key);
    if (it != _props.end()) {
        return it->second;
    }
    //BFC_LOG_ERROR(LOGGER_ERROR, message("Request::getProperty not find key:") << key);
    return "";
}

bool Request::getProperty(const std::string& key,std::string &value) const 
{
    std::map<std::string, std::string>::const_iterator it = _props.find(key);
    if (it != _props.end()) {
        value = it->second; 
        return true;
    }else{
        return false;
    }
}

int Request::GetCookieInt(const std::string& key, int dft) const {
  int ret = dft;
  std::map<std::string, std::string>::const_iterator it = _cookies.find(key);
  if (it != _cookies.end()) {
    try {
      ret = boost::lexical_cast<int>(it->second);
    } catch (boost::bad_lexical_cast& e) {
      //BFC_LOG_ERROR(LOGGER_SERVER, message("bad_lexical_cast to int from string:")
         //            << it->second << " key:" << key << " error:"<< e.what());
    }
  }
  return ret;
}

int Request::getPropertyInt(const std::string& key, int dft) const {
  int ret = dft;
  std::map<std::string, std::string>::const_iterator it = _props.find(key);
  if (it != _props.end()) {
    try {
      ret = boost::lexical_cast<int>(it->second);
    } catch (boost::bad_lexical_cast& e) {
      //BFC_LOG_ERROR(LOGGER_SERVER, message("bad_lexical_cast to int from string:")
        //            << it->second << " key:" << key << " error:"<< e.what());
    }
  }
  return ret;
}

int64_t Request::getPropertyLong(const std::string& key, int64_t dft) const {
  int64_t ret = dft;
  std::map<std::string, std::string>::const_iterator it = _props.find(key);
  if (it != _props.end()) {
    try {
      ret = boost::lexical_cast<int64_t>(it->second);
    } catch (boost::bad_lexical_cast& e) {
      //BFC_LOG_ERROR(LOGGER_SERVER, message("bad_lexical_cast to int from string:")
        //            << it->second << " key:" << key << " error:"<< e.what());
    }
  }
  return ret;
}

double Request::getPropertyDouble(const std::string& key, double dft) const {
    double ret = dft;
    std::map<std::string, std::string>::const_iterator it = _props.find(key);
    if (it != _props.end()) {
      try {
        ret = boost::lexical_cast<double>(it->second);
    } catch (boost::bad_lexical_cast& e) {
        //BFC_LOG_ERROR(LOGGER_ERROR, message("bad_lexical_cast to double from string:")
          //         << it->second << " key:" << key << " error:"<< e.what());
    }
  }
  return ret;
}

bool Request::response() {
  if (!fcgi_out_) {
    //BFC_LOG_ERROR(LOGGER_ERROR, message( "fcgi out is null, request string:") 
      //            << query_ << " cookie_string:" << cookie_string_);
    return false;
  }
  /*
  int http_uid = 0;
  if (_cookies.find("id") != _cookies.end()) {
    try {
      http_uid = boost::lexical_cast<int>(_cookies["id"]);
    } catch (boost::bad_lexical_cast& e) {
      BFC_LOG_ERROR(LOGGER_ERROR, message("bad_lexical_cast to int from cookieid:")
                    << _cookies["id"] << " key:id" <<  " error:"<< e.what());
    }
    }*/
  return true;
}

string Request::GetParamFix(const std::string & name) const {
  std::map<std::string, std::string>::const_iterator i = param_map_.find(name);
  if (i != param_map_.end()) {
    return i->second;
  }return std::string();
}

string Request::GetRemoteAddr() const 
{
    string addr = GetParamFix("REMOTE_ADDR");
    string forwarded_for = GetParamFix("HTTP_X_FORWARDED_FOR");

    while (!forwarded_for.empty())
    {
        size_t pos = forwarded_for.find_first_of(',');
        if(forwarded_for.substr(0, pos) != "unknown") 
        {
            addr = forwarded_for.substr(0, pos);
            break;
        } 
        else 
        {
            //BFC_LOG_INFO(LOGGER_SERVER, message("Request::GetRemoteAdd unknown forwarded-for:")
              //      << forwarded_for);
        }
        if(pos == string::npos)
        {
            break;
        }
        if(pos + 2 <= forwarded_for.size()) 
        {
            forwarded_for = forwarded_for.substr(pos + 2);
        }
    }

    return addr;
}

void Request::InitParamMap(FCGX_Request* req) {
  for(int i = 0; req->envp[i]; ++i) {
    std::string s = req->envp[i];
    //BFC_LOG_DEBUG(LOGGER_DEBUG, message("RequestEnvp K:") << i << " V:" << s);
    size_t pos = s.find_first_of('=');
    if (pos > 0 && pos < s.size() - 1) {
      param_map_.insert(make_pair(s.substr(0,pos), s.substr(pos+1)));
    }
  }
}

void Request::Init(FCGX_Request* r) {
  InitParamMap(r); 
  path_ = GetParamFix("SCRIPT_NAME");
  //parse query_string
  query_ = GetParamFix("QUERY_STRING");
  if (!query_.empty()) {
    ParseKeyValue(query_.c_str(), "&", '=', _props);
  }
  //parse cookie
  cookie_string_ = GetParamFix("HTTP_COOKIE");
  if (!cookie_string_.empty()) {
    ParseKeyValue(cookie_string_.c_str(), "; ", '=', _cookies);
  }
  //parse post body
  int content_len = 0;
  string content_length = GetParamFix("CONTENT_LENGTH");
  try {
    if (!content_length.empty()) {
      content_len = boost::lexical_cast<int>(content_length);
    }
  } catch (boost::bad_lexical_cast& e) {
    //BFC_LOG_ERROR(LOGGER_ERROR, message("bad_lexical_cast to int from content length:")
      //            << content_length << " error:"<< e.what());
  }

  if (content_len > 0) {
    char* str = new char[content_len + 1];
    int count = FCGX_GetStr(str, content_len, r->in);
    str[content_len] = 0;
    if (count > 0) {
      //_props["post_body"] = UriDecode(str);
      _props["post_body"] = string(str, count);
      post_body_ = string(str, count);
      //BFC_LOG_DEBUG(LOGGER_DEBUG, message("post body lenght:") << count 
        //            << " content:" << str);
    }
    delete [] str;
  }
}

void ControllerFactory::debugRequestInfo(FCGX_Request* req) {
  for(int i = 0; req->envp[i]; ++i) {
    std::string s = req->envp[i];
    //BFC_LOG_DEBUG(LOGGER_SERVER, message("RequestEnvp:") << i << " NameValue:" << s);
  }
}

}
