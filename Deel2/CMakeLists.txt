cmake_minimum_required(VERSION 3.10)
# set the project name
project(SocketHTTP)

# add wiringPI library
find_library(wiringPi_LIB wiringPi)
link_libraries(${wiringPi_LIB})

# add the server executable
add_executable(Server main.c)

