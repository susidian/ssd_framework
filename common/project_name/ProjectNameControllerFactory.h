#ifndef _SSD_PROJECT_NAME_CONTROLLER_FACTORY_H_
#define _SSD_PROJECT_NAME_CONTROLLER_FACTORY_H_

#include <boost/scoped_ptr.hpp>
#include "util/FcgiRequest.h"


namespace ssd{

class ProjectNameControllerFactory: public ControllerFactory 
{
    public:
        virtual RequestPtr create(FCGX_Request* r);
        ~ProjectNameControllerFactory() {}
};

typedef boost::shared_ptr<ProjectNameControllerFactory> ProjectNameControllerFactoryPtr;

}

#endif 
