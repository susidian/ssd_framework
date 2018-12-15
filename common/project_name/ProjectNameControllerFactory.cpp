#include "ProjectNameControllerFactory.h"
#include "project_name/ProjectNameHandler.h"
#include "util/FcgiRequest.h"

using namespace ssd; 
static const std::string kProjectNamePath = "/JR";
RequestPtr ProjectNameControllerFactory::create(FCGX_Request* r) 
{
	std::string path(FCGX_GetParam("SCRIPT_NAME", r->envp));
    RequestPtr req;
    if (!path.empty())
    {
        if(path.compare(0, kProjectNamePath.length(), kProjectNamePath) == 0)
        {
            req.reset(new ProjectNameHandler(r));
        }
    }
    return req;
}
