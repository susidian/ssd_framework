bin_PROGRAMS = projectname
INCLUDES = -I$(top_srcdir)/common/ -I$(BOOST_HOME) 
bindir = $(prefix)/bin
bin_SCRIPTS = $(top_srcdir)/common/main/projectname

projectname_SOURCES = ProjectName.cpp 
projectname_LDADD = $(top_srcdir)/common/project_name/libssdobj.a  \
               	    $(top_srcdir)/common/util/libutil.a  \
			    	-L/usr/local/lib \ 
				    -lpthread -lfcgi -lboost_system
