#ifndef SocketException_class
#define SocketException_class

#include <string>
#include <stdio.h>

class SocketException {
	std::string m_s;
public:
	SocketException ( std::string s ) : m_s ( s ) {};
	SocketException ( std::string s, int n) : m_s ( s ) {char buf[10];sprintf(buf," n=%d",n);m_s.append(buf);};
	~SocketException (){};
	std::string description() { return m_s; }
};

#endif