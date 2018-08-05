#ifndef __TC_OPTION_H
#define __TC_OPTION_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �����в���������
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
 * �����������, ֧��������ʽ�Ĳ���
 * ./main.exe --name=value --with abc def
 */
class TC_Option
{
public:
    /**
     * ���캯��
     */
    TC_Option(){};

    /**
     * ����
     * @param argc
     * @param argv
     *
     */
    void decode(int argc, char *argv[]);

    /**
     * �Ƿ����ĳ��--��ʶ�Ĳ���
     * @param sName
     *
     * @return bool
     */
    bool hasParam(const string &sName);

    /**
     * ��ȡĳ��--��ʾ�Ĳ���, ������������ڻ��߲���ֵΪ��, ������""
     * @param sName
     *
     * @return string
     */
    string getValue(const string &sName);

    /**
     * ��ȡ����--��ʶ�Ĳ���
     *
     * @return map<string,string>
     */
    map<string, string>& getMulti();

    /**
     * ��ȡ������ͨ�Ĳ���, �����е�abc, def
     * ��������˳����vector��
     *
     * @return vector<string>
     */
    vector<string>& getSingle();

protected:

    /**
     *
     * @param s
     */
    void parse(const string &s);

protected:
    /**
     *
     */
    map<string, string> _mParam;

    /**
     *
     */
    vector<string>      _vSingle;
};

}

#endif

