#include <iostream>
#include <string>
#include "util/FcgiServer.h"
#include "util/FcgiRequest.h"
#include "project_name/ProjectNameControllerFactory.h"

using namespace ssd;

int main(int argc, char** argv) {
	std::string ip = "0.0.0.0";
	std::string fcgi_socket = ip + ":8002";

    FcgiServer* fcgi_server = new FcgiServer(fcgi_socket, 60, 1024 * 1024);  

    ControllerFactoryPtr project_factory(new ProjectNameControllerFactory());
    fcgi_server->registerControllerFactory(project_factory);
	std::cout << "server start...." << std::endl;

    fcgi_server->start();

    return 0;
}
