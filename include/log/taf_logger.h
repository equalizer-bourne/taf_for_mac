#ifndef __TAF_LOGGER_H__
#define __TAF_LOGGER_H__

#include "util/tc_logger.h"
#include "util/tc_file.h"
#include "util/tc_singleton.h"
#include "servant/Global.h"
#include "log/Log.h"

#define DYEING_DIR "taf_dyeing"
#define DYEING_FILE "dyeing"

namespace taf
{

/**
 * LOG�Ŀ�˵��:
 * �����뿴test/log�Ĵ���
 * 1 ѭ����־����LOG->error(), LOG->debug()
 * 2 ѭ����־���ϴ���������
 * 3 ������־����DLOG, FDLOG����¼
 * 4 ������־Ҳ���Բ��ϴ���Զ�̷�����:DLOG("")->disableRemote();
 * 5 ������־���Ըı�ÿ��һ���ļ��ķ�ʽ:
 *   DLOG("abc3")->setFormat("%Y%m%d%H");
 *   ÿ��Сʱһ���ļ�
 */

/*****************************************************************************
ʵ�ַ�ʽ˵��(ֻ���ܰ�ʱ�����־, ��д��taflog):
    1 �Զ���ʱ����־��WriteT��:RemoteTimeWriteT
    2 ��RemoteTimeWriteT����, д�뵽Զ��
    3 ����Զ����־��:typedef TC_Logger<RemoteTimeWriteT, TC_RollByTime> RemoteTimeLogger;
    4 Ϊ�˱�֤Զ�̵�д��־Ҳ���ڵ����̴߳���,���¶��屾�ذ�����־��
    5 �Զ���ʱ����־��WriteT��:TimeWriteT
    6 ��TimeWriteT���а���RemoteTimeLogger����
    7 ��TimeWriteT���д�������, д�뱾���ļ���, ͬʱд�뵽RemoteTimeLogger������
    8 RemoteTimeLogger����RemoteTimeWriteT������, �첽д�뵽Զ��
    9 �Ӷ������ļ�д��Զ��д����һ���߳���.
*****************************************************************************/

///////////////////////////////////////////////////////////////////////////////

class RollWriteT
{
public:
    RollWriteT();
    ~RollWriteT();

    void operator()(ostream &of, const deque<pair<int, string> > &ds);

    void setDyeingLogInfo(const string &sApp, const string &sServer, const string & sLogPath,
            int iMaxSize, int iMaxNum, const CommunicatorPtr &comm, const string & sLogObj);

protected:

    TC_RollLogger *_pDyeingRollLogger;

    static int  _iDyeingThread;

    string _sApp;
    string _sServer;
    string _sLogPath;
    int _iMaxSize;
    int _iMaxNum;

    /**
     * ȾɫԶ�̹�����־����
     */
    LogPrx                _logPrx;


};


/**
 * ������־������, ����
 * ѭ����־����������������, ��֤�κεط�������ʹ��
 * ���ö��������Ժ�, ��ֱ��cout����
 */
class TafRollLogger : public TC_Singleton<TafRollLogger, CreateUsingNew, PhoneixLifetime>
{
public:
    typedef TC_Logger<RollWriteT, TC_RollBySize> RollLogger;

    /**
     * ���ñ�����Ϣ
     * @param app, ҵ������
     * @param server, ��������
     * @param logpath, ��־·��
     * @param iMaxSize, �ļ�����С,�ֽ�
     * @param iMaxNum, �ļ������
     */
    void setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize = 1024*1024*50, int iMaxNum = 10, const CommunicatorPtr &comm=NULL, const string &sLogObj="");

    /**
     * ����ͬ��д��־
     *
     * @param bSync
     */
    void sync(bool bSync = true);

    /**
     * ��ȡѭ����־
     *
     * @return RollLogger
     */
    RollLogger *logger()          { return &_logger; }

    /**
     * Ⱦɫ��־�Ƿ�����
     * @param bEnable
     */
    void enableDyeing(bool bEnable);

protected:

    /**
     * Ӧ��
     */
    string                  _sApp;

    /**
     * ��������
     */
    string                  _sServer;

    /**
     * ��־·��
     */
    string                  _sLogpath;

    /**
     * ѭ����־
     */
    RollLogger           _logger;

    /**
     * �����߳���
     */
    TC_LoggerThreadGroup    _local;

};

///////////////////////////////////////////////////////////////////////////////////////
//
/**
 * д��־�߳�
 * ��д������־��Զ�̷ֿ�����ͬ���߳�
 * ��Ϊ��������, �������������ĵ���
 */
class TafLoggerThread : public TC_Singleton<TafLoggerThread, CreateUsingNew, PhoneixLifetime>
{
public:
    /**
     * ���캯��
     */
    TafLoggerThread();

    /**
     * ��������
     */
    ~TafLoggerThread();

    /**
     * ����д��־�߳�
     */
    TC_LoggerThreadGroup* local();

    /**
     * Զ��д��־�߳�
     *
     * @return TC_LoggerThreadGroup*
     */
    TC_LoggerThreadGroup* remote();

protected:

    /**
     * �����߳���
     */
    TC_LoggerThreadGroup    _local;

    /**
     * Զ��д�߳���
     */
    TC_LoggerThreadGroup    _remote;
};

///////////////////////////////////////////////////////////////////////////////////////
class TimeWriteT;

/**
 * Զ�̵�Logд������
 */
class RemoteTimeWriteT
{
public:
    ~RemoteTimeWriteT();

    /**
     * ���캯��
     */
    void setTimeWriteT(TimeWriteT *pTimeWrite);

    /**
     * �������
     * @param of
     * @param buffer
     */
    void operator()(ostream &of, const deque<pair<int, string> > &buffer);

protected:
    /**
     * ͬ����Զ��
     */
    void sync2remote(const vector<string> &buffer);

    /**
     * Ⱦɫ��־ͬ����Զ��
     */
    void sync2remoteDyeing(const vector<string> &buffer);

protected:
    /**
     * ָ��
     */
    TimeWriteT          *_pTimeWrite;

};

////////////////////////////////////////////////////////////////////////////
/**
 * дLogger
 */
class TimeWriteT
{
public:
    typedef TC_Logger<RemoteTimeWriteT, TC_RollByTime> RemoteTimeLogger;

    typedef TC_Logger<TC_DefaultWriteT, TC_RollByTime> DyeingTimeLogger;

    /**
     * ����
     */
    TimeWriteT();

    /**
     * ����
     */
    ~TimeWriteT();

    /**
     * ���û�����Ϣ
     * @param app, Ӧ������
     * @param server, ��������
     * @param file, ��־�ļ���
     * @param sFormat, ��ʽ
     */
    void setLogInfo(const LogPrx &logPrx, const string &sApp, const string &sServer, const string &sFile, const string &sLogpath, const string &sFormat);

    /**
     * ���ô���
     * @param logPrx ������Ϣ
     */
    void setLogPrx(const LogPrx &logPrx);

    /**
     * Զ����־���ܴ򿪻�ر�
     * @param bEnable
     */
    void enableRemote(bool bEnable)         { _bRemote = bEnable; }

    /**
     * ������־���ܹ��ܴ򿪻�ر�
     * @param bEnable
     */
    void enableLocal(bool bEnable);

    /**
     * Ⱦɫ��־���ܴ򿪻�ر�
     * @param bEnable
     */
    void enableDyeing (bool bEnable);

    /**
     * ����ʱ���ʽ("%Y%m%d")
     * @param sFormat
     */
    void setFormat(const string &sFormat)   { _sFormat = sFormat; }

    /**
     * �������
     * @param of
     * @param buffer
     */
    void operator()(ostream &of, const deque<pair<int, string> > &buffer);

protected:

    /**
     * ��Ԫ
     */
    friend class RemoteTimeWriteT;

    /**
     * ��¼�����ļ�
     * @param buffer
     */
    void writeError(const vector<string> &buffer);

    /**
     * ��¼�����ļ�
     * @param buffer
     */
    void writeError(const deque<pair<int, string> > &buffer);

    /**
     * ��ʼ��logger
     */
    void initError();

    /**
     * ��ʼ��Ⱦɫ��־
     */
    void initDyeingLog();

protected:
    
    /**
     * Զ��ʱ����־
     */
    RemoteTimeLogger    *_pRemoteTimeLogger;

    /**
     * Զ�̹���
     */
    bool                _bLocal;

    /**
     * Զ�̹���
     */
    bool                _bRemote;

    /**
     * Զ�̷�����
     */
    LogPrx              _logPrx;

    /**
     * app����
     */
    string              _sApp;

    /**
     * ��������
     */
    string              _sServer;

    /**
     * ��־�ļ�����
     */
    string              _sFile;

    /**
     * ʱ���ʽ
     */
    string              _sFormat;

    /**
     * �����ļ�
     */
    string              _sFilePath;

    /**
     * �����ļ�
     */
    TC_DayLogger        _elogger;

    /**
     * ȱʡдģʽ
     */
    TC_DefaultWriteT    _wt;

    /**
     * Ⱦɫ��־
     */
    static int                _bDyeing;

    /**
     * Ⱦɫ��־Ŀ¼·��
     */
    string              _sDyeingFilePath;

    /**
     * Զ��ʱ����־
     */
    DyeingTimeLogger    *_pDyeingTimeLogger;
};

////////////////////////////////////////////////////////////////////////////

/**
 * Զ����־������, ����
 */
class TafTimeLogger : public TC_HandleBase
                    , public TC_ThreadLock
                    , public TC_Singleton<TafTimeLogger, CreateUsingNew, DefaultLifetime>
{
public:

    //���尴ʱ���������־
    typedef TC_Logger<TimeWriteT, TC_RollByTime> TimeLogger;

    /**
     * ����
     */
    TafTimeLogger();

    /**
     * ����
     */
    ~TafTimeLogger();

    /**
     * ���ñ�����Ϣ
     * @param comm, ͨ����
     * @param obj, ��־��������
     * @param app, ҵ������
     * @param server, ��������
     * @param logpath, ��־·��
     */
    void setLogInfo(const CommunicatorPtr &comm, const string &obj, const string &sApp, const string &sServer, const string &sLogpath);

    /**
     * ��ʼ������ʱ���ʽ("%Y%m%d")
     * ��Ҫ��̬�޸�, �̲߳���ȫ
     * �������Ҫ, ��ʼ����ֱ���޸�
     * @param sFormat, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     */
    void initFormat(const string &sFile, const string &sFormat);

    /**
     * ��ȡʱ����־
     * @param file
     */
    TimeLogger *logger(const string &sFile = "");

    /**
     * ��ȡʱ����־ 
     * @param app, ҵ������
     * @param server, �������� 
     * @param file
     */
    TimeLogger *logger(const string &sApp, const string &sServer,const string &sFile = "");

    /**
     * ͬ��д����ʱ����־(Զ����־һ�����첽д��, �޷�����)
     * @param bSync
     */
    void sync(const string &sFile, bool bSync);

    /**
     * Զ��ʱ����־
     * @param sFile, �ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param bEnable
     */
    void enableRemote(const string &sFile, bool bEnable);

    /**
     * ����ʱ����־
     * @param sFile,�ļ�����, Ϊ�ձ�ʾȱʡ��ʱ����־
     * @param bEnable
     */
    void enableLocal(const string &sFile, bool bEnable);

protected:

    /**
     * ��ʼ��ʱ����־
     * @param pTimeLogger
     * @param sFile
     * @param sFormat
     */
    void initTimeLogger(TimeLogger *pTimeLogger, const string &sFile, const string &sFormat);

    /**
     * ��ʼ��ʱ����־
     * @param pTimeLogger
     * @param sApp 
     * @param sServer
     * @param sFile
     * @param sFormat
     */

    void initTimeLogger(TimeLogger *pTimeLogger,const string &sApp, const string &sServer, const string &sFile, const string &sFormat);

protected:

    /**
     * ͨ����
     */
    CommunicatorPtr         _comm;

    /**
     * Զ�̷�����
     */
    LogPrx                  _logPrx;

    /**
     * Ӧ��
     */
    string                  _sApp;

    /**
     * ��������
     */
    string                  _sServer;

    /**
     * ��־·��
     */
    string                  _sLogpath;

    /**
     * ȱʡ������־
     */
    TimeLogger              *_pDefaultLogger;

    /**
     * Զ����־
     */
    map<string, TimeLogger*>    _loggers;

};

/**
 * Ⱦɫ�����࣬����ʱ�ر�
 */
class TafDyeingSwitch
{
public:
    /**
     * ���캯����Ĭ�ϲ���Ⱦɫ��־
     */
    TafDyeingSwitch():bNeedDyeing(false) {}

    /**
     * �����������ر��Ѵ򿪵�Ⱦɫ��־
     */
    ~TafDyeingSwitch()  
	{
		if(bNeedDyeing) 
		{
			TafRollLogger::getInstance()->enableDyeing(false);

			ServantProxyThreadData* td = ServantProxyThreadData::getData();
			if (td)
			{
				td->data()->_bDyeing = false;
			}
		}
	}

    /**
     * ����Ⱦɫ��־
     */
    void enableDyeing() 
	{
		TafRollLogger::getInstance()->enableDyeing(true); 

		ServantProxyThreadData* td = ServantProxyThreadData::getData();
		if (td)
		{
			td->data()->_bDyeing	= true;
		}

		bNeedDyeing = true;
	}

protected:
    bool bNeedDyeing;
};

/**
 * ѭ����־
 */
#define LOG             (TafRollLogger::getInstance()->logger())

/**
 * ������־
 */
#define DLOG            (TafTimeLogger::getInstance()->logger()->any())
#define FDLOG(x)        (TafTimeLogger::getInstance()->logger(x)->any())
#define FFDLOG(x,y,z)   (TafTimeLogger::getInstance()->logger(x,y,z)->any())

}

#endif

