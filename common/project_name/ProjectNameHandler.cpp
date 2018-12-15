#include <string>
#include "ProjectNameHandler.h"

namespace ssd {

ProjectNameHandler::ProjectNameHandler(FCGX_Request* r) : Request(r) {}

bool ProjectNameHandler::response() {
	std::string ret_str = "Content-type: text/html\r\n\r\n";
    ret_str += "Hello World!";
    ret_str += "\r\n";
    writeResponse(ret_str);

    return true;
}

}
