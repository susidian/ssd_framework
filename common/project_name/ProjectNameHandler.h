#ifndef _SSD_ProjectName_HANDLER_H_
#define _SSD_ProjectName_HANDLER_H_

#include "util/FcgiRequest.h"

namespace ssd {

class ProjectNameHandler : public Request 
{
	public:
		explicit ProjectNameHandler(FCGX_Request* r);
		virtual ~ProjectNameHandler(){}
		bool response();
};

}

#endif
