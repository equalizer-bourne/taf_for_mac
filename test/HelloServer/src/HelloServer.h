#ifndef _HelloServer_H_
#define _HelloServer_H_

#include <iostream>
#include "util/tc_http.h"
#include "util/tc_cgi.h"
#include "servant/Application.h"

using namespace taf;

class HelloServer : public Application
{
public:
	/**
	 * ��������
	 **/
	virtual ~HelloServer() {};

	/**
	 * �����ʼ��
	 **/
	virtual void initialize();

	/**
	 * ��������
	 **/
	virtual void destroyApp();

public:
    /**
     * CCServer�Ķ�������
     */
    static string CCSERVER_OBJ;
};
////////////////////////////////////////////
#endif
