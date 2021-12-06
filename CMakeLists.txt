cmake_minimum_required(VERSION 3.10)
# set the project name
project(ClientServerUDP)
#add the libraries
link_libraries(libtlpi.a)

# add PJ_RPI library
link_libraries(PJ_RPI)

# add wiringPI library
find_library(wiringPi_LIB wiringPi)
link_libraries(${wiringPi_LIB})


# add the client executable
add_executable(Client ud_ucase_cl.c)
# add the server executable
add_executable(Server ud_ucase_sv.c)
# add the client executable
add_executable(Test test.c)


TARGET_LINK_LIBRARIES( Test
pthread
)
