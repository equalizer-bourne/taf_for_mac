#include "log/taf_logger.h"
#include "servant/Communicator.h"

namespace taf
{

int RollWriteT::_iDyeingThread = 0;
int TimeWriteT::_bDyeing = 0;

/////////////////////////////////////////////////////////////////////////////////////

RollWriteT::RollWriteT():_pDyeingRollLogger(NULL), _iMaxSize(10000), _iMaxNum(1), _logPrx(NULL)
{
}

RollWriteT::~RollWriteT()
{
    if(_pDyeingRollLogger)
    {
        delete _pDyeingRollLogger;
    }
}

void RollWriteT::operator()(ostream &of, const deque<pair<int, string> > &ds)
{
    vector<string> vRemoteDyeing;

    deque<pair<int, string> >::const_iterator it = ds.begin();
    while(it != ds.end())
    {
        of << it->second;

        //Ⱦɫ�߳�id������
        if(it->first != 0)
        {
            if(!_pDyeingRollLogger)
            {
                TC_File::makeDirRecursive(_sLogPath + "/"DYEING_DIR"/");

                //��ʼ��Ⱦɫѭ����־
                _pDyeingRollLogger = new TC_RollLogger();

                _pDyeingRollLogger->init(_sLogPath + "/"DYEING_DIR"/"DYEING_FILE, _iMaxSize, _iMaxNum);
                _pDyeingRollLogger->modFlag(TC_DayLogger::HAS_TIME, false);
                _pDyeingRollLogger->modFlag(TC_DayLogger::HAS_TIME|TC_DayLogger::HAS_LEVEL|TC_DayLogger::HAS_PID, true);
                _pDyeingRollLogger->setLogLevel("DEBUG");
            }

            _pDyeingRollLogger->roll(make_pair(it->first, _sApp + "." + _sServer + "|" + it->second ));

            vRemoteDyeing.push_back(_sApp + "." + _sServer + "|" + it->second);
        }

        ++it;
    }
    of.flush();

    if(_logPrx && vRemoteDyeing.size() > 0)
    {
        try
        {
            _logPrx->logger(DYEING_DIR, DYEING_FILE, "roll", "%Y%m%d", vRemoteDyeing);
        }
        catch(exception &ex)
        {
            LOG->error() << "[TAF] dyeing log write to remote log server error:" << ex.what() << endl;
        }
    }
}

void RollWriteT::setDyeingLogInfo(const string &sApp, const string &sServer, const string & sLogPath, int iMaxSize, int iMaxNum, const CommunicatorPtr &comm, const string &sLogObj)
{
    _sApp     = sApp;
    _sServer  = sServer;
    _sLogPath = sLogPath;
    _iMaxSize = iMaxSize;
    _iMaxNum  = iMaxNum;

    if(comm && !sLogObj.empty())
    {
        _logPrx = comm->stringToProxy<LogPrx>(sLogObj);
        //�������ó�ʱʱ��
        _logPrx->taf_timeout(3000);
    }
}


/////////////////////////////////////////////////////////////////////////////////////

void TafRollLogger::setLogInfo(const string &sApp, const string &sServer, const string &sLogpath, int iMaxSize, int iMaxNum, const CommunicatorPtr &comm, const string &sLogObj)
{
    _sApp       = sApp;
    _sServer    = sServer;
    _sLogpath   = sLogpath;

    //����Ŀ¼
    TC_File::makeDirRecursive(_sLogpath + "/" + _sApp + "/" + _sServer);

    _local.start(1);

    //��ʼ������ѭ����־
    _logger.init(_sLogpath + "/" + _sApp + "/" + _sServer + "/" + _sApp + "." + _sServer, iMaxSize, iMaxNum);
    _logger.modFlag(TC_DayLogger::HAS_TIME, false);
    _logger.modFlag(TC_DayLogger::HAS_TIME|TC_DayLogger::HAS_LEVEL|TC_DayLogger::HAS_PID, true);

    //����Ϊ�첽
    sync(false);


    //����Ⱦɫ��־��Ϣ
    _logger.getWriteT().setDyeingLogInfo(sApp, sServer, sLogpath, iMaxSize, iMaxNum, comm, sLogObj);

}


void TafRollLogger::sync(bool bSync)
{
    if(bSync)
    {
        _logger.unSetupThread();
    }
    else
    {
        _logger.setupThread(&_local);
    }
}

void TafRollLogger::enableDyeing(bool bEnable)
{
    _logger.getRoll()->enableDyeing(bEnable);
}

/////////////////////////////////////////////////////////////////////////////////////

TafLoggerThread::TafLoggerThread()
{
    _local.start(1);
    _remote.start(1);
}

TafLoggerThread::~TafLoggerThread()
{
    //��ˢ�±�����־
    _local.flush();

    //��ˢ��Զ����־, ��֤���ᶪ��־
    _remote.flush();
}

TC_LoggerThreadGroup* TafLoggerThread::local()
{
    return &_local;
}

TC_LoggerThreadGroup* TafLoggerThread::remote()
{
    return &_remote;
}

/////////////////////////////////////////////////////////////////////////////////////

RemoteTimeWriteT::~RemoteTimeWriteT()
{
}

void RemoteTimeWriteT::setTimeWriteT(TimeWriteT *pTimeWrite)
{
    _pTimeWrite = pTimeWrite;
}

void RemoteTimeWriteT::operator()(ostream &of, const deque<pair<int, string> > &buffer)
{
    const static uint32_t len = 2000;

    //дԶ����־
    if(_pTimeWrite->_logPrx && !buffer.empty())
    {
        //����50w��, ֱ��������,�������׵����ڴ�й©
        if(buffer.size() > 500000)
        {
            _pTimeWrite->writeError(buffer);
            return;
        }

        vector<string> v;
        v.reserve(len);

        deque<pair<int, string> >::const_iterator it = buffer.begin();
        while(it != buffer.end())
        {
            v.push_back(it->second);

            ++it;

            //ÿ�����ͬ��len��
            if(v.size() >= len)
            {
                sync2remote(v);
                v.clear();
                v.reserve(len);
            }
        }

        if(v.size() > 0)
        {
            sync2remote(v);
        }
    }
}

void RemoteTimeWriteT::sync2remote(const vector<string> &v)
{
    try
    {
        _pTimeWrite->_logPrx->logger(_pTimeWrite->_sApp, _pTimeWrite->_sServer, _pTimeWrite->_sFile, _pTimeWrite->_sFormat, v);
    }
    catch(exception &ex)
    {
        LOG->error() << "[TAF] write to remote log server error:" << ex.what() << ": buffer size:" << v.size() << endl;
        _pTimeWrite->writeError(v);
    }
}

void RemoteTimeWriteT::sync2remoteDyeing(const vector<string> &v)
{
    try
    {
        _pTimeWrite->_logPrx->logger(DYEING_DIR, DYEING_FILE, "", _pTimeWrite->_sFormat, v);
    }
    catch(exception &ex)
    {
        LOG->error() << "[TAF] write dyeing log to remote log server error:" << ex.what() << ": buffer size:" << v.size() << endl;
        _pTimeWrite->writeError(v);
    }
}
/////////////////////////////////////////////////////////////////////////////////////
//
TimeWriteT::~TimeWriteT()
{
    if(_pRemoteTimeLogger)
    {
        delete _pRemoteTimeLogger;
    }
}

TimeWriteT::TimeWriteT() : _pRemoteTimeLogger(NULL), _bLocal(true), _bRemote(true), _pDyeingTimeLogger(NULL)
{
}

void TimeWriteT::setLogInfo(const LogPrx &logPrx, const string &sApp, const string &sServer, const string &sFile, const string &sLogpath, const string &sFormat)
{
    _logPrx     = logPrx;
    _sApp       = sApp;
    _sServer    = sServer;
    _sFormat    = sFormat;
    _sFile      = sFile;

    _sFilePath = sLogpath + "/" + _sApp + "/" + _sServer + "/" + _sApp + "." + _sServer;
    if(!_sFile.empty())
    {
        _sFilePath += "_" + _sFile;
    }
    _sDyeingFilePath = sLogpath + "/"DYEING_DIR"/";

    _pRemoteTimeLogger = new RemoteTimeLogger();
    _pRemoteTimeLogger->init(_sFilePath, _sFormat);
    _pRemoteTimeLogger->modFlag(0xffff, false);
    _pRemoteTimeLogger->setupThread(TafLoggerThread::getInstance()->remote());
    _pRemoteTimeLogger->getWriteT().setTimeWriteT(this);

    if(!_bLocal)
    {
        initError();
    }
}

void TimeWriteT::initDyeingLog()
{
    TC_File::makeDirRecursive(_sDyeingFilePath);

    _pDyeingTimeLogger = new DyeingTimeLogger();
    _pDyeingTimeLogger->init(_sDyeingFilePath + "/"DYEING_FILE, _sFormat);
    _pDyeingTimeLogger->modFlag(0xffff, false);
}

void TimeWriteT::setLogPrx(const LogPrx &logPrx)
{
    _logPrx     = logPrx;
}

void TimeWriteT::initError()
{
    //Զ�̴�����־
    _elogger.init(_sFilePath + ".remote.error", _sFormat);
    _elogger.modFlag(0xffff, false);
}

void TimeWriteT::enableLocal(bool bEnable)          
{ 
    _bLocal = bEnable; 
    if(!_bLocal)
    {
        initError();
    }
}

void TimeWriteT::operator()(ostream &of, const deque<pair<int, string> > &buffer)
{

    if(_bLocal && of && !buffer.empty())
    {
        try
        {
            _wt(of, buffer);
        } 
        catch(...)
        {
        }
    }

    if(_bRemote && _pRemoteTimeLogger && !buffer.empty())
    {
        deque<pair<int, string> >::const_iterator it = buffer.begin();
        while(it != buffer.end())
        {
            _pRemoteTimeLogger->any() << it->second;
            ++it;
        }
    }

    vector<string> vDyeingLog;
    deque<pair<int, string> >::const_iterator it = buffer.begin();
    while(it != buffer.end())
    {
        if(it->first != 0)
        {
            if(!_pDyeingTimeLogger)
            {
                initDyeingLog();
            }
            _pDyeingTimeLogger->any() << _sApp << "." << _sServer << "|" << it->second;

            vDyeingLog.push_back(_sApp + "." + _sServer + "|" + it->second);
        }
        ++it;
    }
    if(_logPrx && !vDyeingLog.empty())
    {
        try
        {
            _logPrx->logger(DYEING_DIR, DYEING_FILE, "day", "%Y%m%d", vDyeingLog);
        }
        catch(exception &ex)
        {
            LOG->error() << "[TAF] dyeing log write to remote log server error:" << ex.what() << endl;
        }
    }
}

void TimeWriteT::writeError(const vector<string> &buffer)
{
    if(!_bLocal)
    {
        for(size_t i = 0; i < buffer.size(); i++)
        {
            _elogger.any() << buffer[i];
        }
    }
}

void TimeWriteT::writeError(const deque<pair<int, string> > &buffer)
{
    if(!_bLocal)
    {
        deque<pair<int, string> >::const_iterator it = buffer.begin();
        while(it != buffer.end())
        {
            _elogger.any() << it->second;
            ++it;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////

TafTimeLogger::TafTimeLogger() : _pDefaultLogger(NULL)
{
}

TafTimeLogger::~TafTimeLogger()
{
    if(_pDefaultLogger != NULL)
    {
        delete _pDefaultLogger;
    }

    map<string, TimeLogger*>::iterator it = _loggers.begin();
    while(it != _loggers.end())
    {
        delete it->second;
        ++it;
    }
    _loggers.clear();
}

void TafTimeLogger::initTimeLogger(TimeLogger *pTimeLogger, const string &sFile, const string &sFormat)
{
    string sFilePath = _sLogpath + "/" + _sApp + "/" + _sServer + "/" + _sApp + "." + _sServer;
    if(!sFile.empty())
    {
        sFilePath += "_" + sFile;
    }

    pTimeLogger->init(sFilePath, sFormat);
    pTimeLogger->modFlag(0xffff, false);
    pTimeLogger->modFlag(TC_DayLogger::HAS_TIME, true);
    pTimeLogger->setupThread(TafLoggerThread::getInstance()->local());
    pTimeLogger->getWriteT().setLogInfo(_logPrx, _sApp, _sServer, sFile, _sLogpath, sFormat);
}

void TafTimeLogger::initTimeLogger(TimeLogger *pTimeLogger,const string &sApp, const string &sServer, const string &sFile, const string &sFormat)
{
    string sFilePath = _sLogpath + "/" + sApp + "/" + sServer + "/" + sApp + "." + sServer;
    if(!sFile.empty())
    {
        sFilePath += "_" + sFile;
    }
     //����Ŀ¼
    //TC_File::makeDirRecursive(_sLogpath + "/" + sApp + "/" + sServer);

    pTimeLogger->init(sFilePath, sFormat);
    pTimeLogger->modFlag(0xffff, false);
    pTimeLogger->modFlag(TC_DayLogger::HAS_TIME, true);
    pTimeLogger->setupThread(TafLoggerThread::getInstance()->local());
    pTimeLogger->getWriteT().setLogInfo(_logPrx, sApp, sServer, sFile, _sLogpath, sFormat);
}

void TafTimeLogger::setLogInfo(const CommunicatorPtr &comm, const string &obj, const string &sApp, const string &sServer, const string &sLogpath)
{
    _sApp       = sApp;
    _sServer    = sServer;
    _sLogpath   = sLogpath;
    _comm       = comm;

    if(!obj.empty())
    {
        _logPrx = _comm->stringToProxy<LogPrx>(obj);
        //�������ó�ʱʱ��
        _logPrx->taf_timeout(3000);

        if(_pDefaultLogger)
        {
            _pDefaultLogger->getWriteT().setLogPrx(_logPrx);
        }
    }

    //����Ŀ¼
    TC_File::makeDirRecursive(_sLogpath + "/" + _sApp + "/" + _sServer);
}

void TafTimeLogger::initFormat(const string &sFile, const string &sFormat)
{
    if(sFile.empty())
    {
        if(!_pDefaultLogger)
        {
            _pDefaultLogger = new TimeLogger();

        }
        initTimeLogger(_pDefaultLogger, "", sFormat);
    }
    else
    {
        Lock lock(*this);
        map<string, TimeLogger*>::iterator it = _loggers.find(sFile);
        if( it == _loggers.end())
        {
            TimeLogger *p = new TimeLogger();
            initTimeLogger(p, sFile, sFormat);
            _loggers[sFile] = p;
            return;
        }

        initTimeLogger(it->second, sFile, sFormat);
    }
}

TafTimeLogger::TimeLogger* TafTimeLogger::logger(const string &sFile)
{
    if(sFile.empty())
    {
        if(!_pDefaultLogger)
        {
            _pDefaultLogger = new TimeLogger();
            initTimeLogger(_pDefaultLogger, "", "%Y%m%d");
        }
        return _pDefaultLogger;
    }

    Lock lock(*this);
    map<string, TimeLogger*>::iterator it = _loggers.find(sFile);
    if( it == _loggers.end())
    {
        TimeLogger *p = new TimeLogger();
        initTimeLogger(p, sFile, "%Y%m%d");
        _loggers[sFile] = p;
        return p;
    }

    return it->second;
}

TafTimeLogger::TimeLogger* TafTimeLogger::logger(const string &sApp, const string &sServer,const string &sFile)
{
    string s = sApp + "/" + sServer + "/"+ sFile;
 
    Lock lock(*this);
    map<string, TimeLogger*>::iterator it = _loggers.find(s);
    if( it == _loggers.end())
    {
        TimeLogger *p = new TimeLogger();
        initTimeLogger(p, sApp, sServer, sFile, "%Y%m%d");
        _loggers[s] = p;
        return p;
    }

    return it->second;
}


void TafTimeLogger::sync(const string &sFile, bool bSync)
{
    if(bSync)
    {
        logger(sFile)->unSetupThread();
    }
    else
    {
        logger(sFile)->setupThread(TafLoggerThread::getInstance()->local());
    }
}

void TafTimeLogger::enableRemote(const string &sFile, bool bEnable)
{
    logger(sFile)->getWriteT().enableRemote(bEnable);
}

void TafTimeLogger::enableLocal(const string &sFile, bool bEnable)
{
    logger(sFile)->getWriteT().enableLocal(bEnable);
}

}

