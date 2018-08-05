#ifndef __TAF_CONFIG_H_
#define __TAF_CONFIG_H_

#include "util/tc_autoptr.h"
#include "util/tc_singleton.h"
#include "servant/Global.h"
#include "config/Config.h"

using namespace std;

namespace taf
{

/**
 *
 * ���ܣ���ȡԶ��ϵͳ���ã����ɱ����ļ���֧�ֱ��ݺͻع�
 *
 * ˵�����ö���ֻʵ�ֽ�ConfigServer��ָ���������ļ�
 * ��ȡ������(���浽Ӧ�ó���ִ��Ŀ¼)���������ý���
 * �ɿͻ���ʵ��
 *
 * �ͻ���ͨ������addConfig�ӿ�Ϊ
 * ÿ�������ļ�����һ��TafRemoteConfigʵ��
 *
 * �����ļ���Ŀ�ڶ��󴴽�ʱָ����ȱʡΪ5����
 * �ܻع��Ĵ������ڱ����ļ���Ŀ
 *
 */

class TafRemoteConfig : public TC_Singleton<TafRemoteConfig>
{
public:
    /**
     * ��ʼ��
     * @param comm, ͨ����
     * @param obj, ��������
     * @param app, Ӧ������
     * @param serverName, ��������
     * @param basePath, ����·��
     * @param maxBakNum, ��󱸷��ļ�����
     *
     * @return int
     */
	int setConfigInfo(const CommunicatorPtr &comm, const string &obj, const string & app, const string &serverName, const string& basePath, int maxBakNum=5);

	/**
     * ��ȡConfigServer�������ļ������أ�������ԭ�ļ�
     * @param  sFullFileName �ļ�����
     * @param  result        ���
     * @param  bAppOnly      �Ƿ�ֻ��ȡӦ�ü��������
     *
     * @return bool
	 */
	bool addConfig(const string & filename, string &result, bool bAppConfigOnly = false);

private:
	/**
     *  ʵ������ConfigServer����������ļ���ʽ���浽����Ŀ¼
     * @param  sFullFileName �ļ�����
     * @param  bAppOnly      �Ƿ�ֻ��ȡӦ�ü��������
	 *
     * @return string       ���ɵ��ļ�����
	 */
	string getRemoteFile(const string & sFullFileName, bool bAppConfigOnly = false);

	/**
     * ʵ�ֱ����ļ��Ļع����ɻع�����������󱸷��ļ�����ÿ��
     * ��ʹ������ı����ļ����ǵ�ǰ�����ļ�
	 *
	 * @return string
	 */
	string recoverSysConfig(const string & sFullFileName);

	/**
     * �����ļ����� Config.conf.1.bak,Config.conf.2.bak ...
     * �÷����ṩ�±굽�ļ�����ת��
     *
     * @param index         �ڼ��������ļ�
	 *
     * @return string       �����ļ�ȫ·��
	 */
	inline string index2file(const string & sFullFileName, int index);

	/**
     *  renameϵͳ�����ķ�װ����oldFile������ʱ�׳��쳣
     *
     * @param oldFile   ԭ�ļ�·��������
     * @param newFile   ���ļ��߼�������
	 */
	inline void localRename(const string& oldFile, const string& newFile);

protected:

    /**
     * ͨ����
     */
    CommunicatorPtr _comm;

    /**
     * ���ô���
     */
	ConfigPrx       _configPrx;

    /**
     * Ӧ��
     */
    string          _sApp;

    /**
     * ��������
     */
	string          _sServerName;

    /**
     * ·��
     */
	string          _sBasePath;

    /**
     * ��󱸷���
     */
	int             _iMaxBakNum;

	/**
     * �߳���
	 */
	TC_ThreadMutex 	_mutex;
};

}

#endif
