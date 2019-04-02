#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H
#pragma once

#include <iostream>
#include <atomic>
#include <unistd.h>
#include <sys/epoll.h>
#include <mutex>
#include <thread>
#include <future>
#include <condition_variable>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unordered_map>
#include <unordered_set>
#include <signal.h>
#include <fstream>

#include "config.h"
#include "error.h"
#include "mstring.h"

using std::ios;
using std::cout;
using std::endl;
using std::cerr;

template <typename Key_Type, typename Value_Type, class Hash=std::hash<Key_Type>>
using HashMap = std::unordered_map<Key_Type, Value_Type, Hash>;

template <typename T>
using HashSet = std::unordered_set<T>;

#endif // COMMON_INCLUDES_H
