#ifndef __TC_LOGGER_H
#define __TC_LOGGER_H

#include <streambuf>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <time.h>
#include <sys/syscall.h>
#include <iomanip>
#include "util/tc_ex.h"
#include "util/tc_autoptr.h"
#include "util/tc_common.h"
#include "util/tc_file.h"
#include "util/tc_thread.h"
#include "util/tc_monitor.h"
#include "util/tc_thread_pool.h"
#include "util/tc_timeprovider.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: ��־��
// Author : j@syswin.com              
// Modify : j@syswin.com   
// Modify : j@syswin.com               
/////////////////////////////////////////////////
/*****************************************************************************
˵��:
	����չ����־��,�����Զ���������Ժ�д����.
	�ṩ�����ַ�ʽ�Ĺ�����ʽ:
	1 ���ļ���С,�ļ���Ŀ����;
	2 ��ʱ�����(����/Сʱ/���ӵ�)
	��������Ĭ�ϵĲ�����,����Ĭ��д���ļ���,��û�г�ʼ��,��Ĭ��д�뵽cout

	�Զ��������ʽ�Ĳ�������:
	1 ʵ���Զ���ģ����,�̳���TC_LoggerRoll,ʵ��roll����,�ú���ʵ��д�߼�, ����:
	template<typename WriteT>
	class MyRoll : public TC_LoggerRoll, public TC_ThreadMutex
	2 ģ���е�WriteT�Ǿ����д���߼�,�ṩȱʡ��:TC_DefaultWriteT
	3 �����Զ���ʵ��WriteT, ����:ʵ�ַ��͵�������߼�.
	4 ��MyRoll��ʵ���ڲ���RollWrapperI, ���̳�RollWrapperBase<MyRoll<WriteT> >
	5 RollWrapperI��Ҫ�����ṩMyRoll�ĳ�ʼ���ӿ�, ��Щ�ӿ�����ֱ�ӷ�Ӧ��TC_Logger��.
	6 RollWrapperI����ʱ, ����MyRoll����(��������ָ��),���ҽӿڵĲ���ֱ��ת����MyRoll������.

	ʵ����Щ�߼���, ����ģ�巽ʽ���������:
	typedef TC_Logger<MyWriteT, MyRoll> MyLogger;

	TAF��Զ��logʵ������,���Բμ�src/libtaf/taf_logger.h
*****************************************************************************/
/**
* ��־�쳣��
*/
	struct TC_Logger_Exception : public TC_Exception
	{
		TC_Logger_Exception(const string &buffer) : TC_Exception(buffer){};
		TC_Logger_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
		~TC_Logger_Exception() throw(){};
	};

/**
 * д����
 */
	class TC_DefaultWriteT
	{
	public:
		void operator()(ostream &of, const deque<pair<int, string> > &ds)
		{
			deque<pair<int, string> >::const_iterator it = ds.begin();
			while (it != ds.end())
			{
				of << it->second;
				++it;
			}
			of.flush();
		}
        
	};

	class TC_LoggerThreadGroup;

//////////////////////////////////////////////////////////////////////////////
    
#if __APPLE__
    static map<pthread_t, int> __tmpThreadID;
    static int __threadBase = 100;
    static list<int> __delPtds;
    int __getPthreadIntID(pthread_t ptd)
    {
        if (__tmpThreadID.find(ptd) != __tmpThreadID.end()) {
            return __tmpThreadID[ptd];
        }else{
            return 0;
        }
    }
    
    int __addPthreadForIntID(pthread_t ptd)
    {
        if (__tmpThreadID.find(ptd) != __tmpThreadID.end()) {
            return __tmpThreadID[ptd];
        }else{
            if (__delPtds.empty()) {
                __threadBase++;
                __tmpThreadID[ptd] = __threadBase;
                return __threadBase;
            }else{
                int tid = __delPtds.front();
                __delPtds.pop_front();
                __tmpThreadID[ptd] = tid;
                return tid;
            }
        }
    }
    
    int __delPthreadIntID(pthread_t ptd)
    {
        map<pthread_t, int>::iterator it = __tmpThreadID.find(ptd);
        if ( it != __tmpThreadID.end()) {
            int tid = __tmpThreadID[ptd];
            __tmpThreadID.erase(it);
            __delPtds.push_back(tid);
            return tid;
        }else{
            return 0;
        }
    }
    
#endif
    
/**
 * ����д��־����
 */
	class TC_LoggerRoll : public TC_HandleBase
	{
	public:
		/**
		 * ���캯��
		 */
		TC_LoggerRoll() : _pThreadGroup(NULL)
		{
		}

		/**
		 * ʵʱ����־, ���ҹ���
		 * ��ͬ�Ĳ�����־��,ʵ�ֲ�ͬ���߼�
		 * @param buffer
		 */
		virtual void roll(const deque<pair<int, string> > &ds) = 0;

		/**
		 * ��װ�߳�
		 * @param ltg
		 */
		void setupThread(TC_LoggerThreadGroup *pThreadGroup);

		/**
		 * ȡ���߳�
		 */
		void unSetupThread();

		/**
		 * д����־
		 * @param buffer
		 */
		void write(const pair<int, string> &buffer);

		/**
		 * ˢ�»��浽�ļ�
		 */
		void flush();

		/**
		 * ����Ⱦɫ�Ƿ���Ч
		 * @param bEnable
		 */
		void enableDyeing(bool bEnable) 
		{
			TC_LockT<TC_ThreadMutex> lock(_mutexDyeing);

			if (bEnable)
			{
				_setThreadID.insert(pthread_self());
#if __APPLE__
                __addPthreadForIntID(pthread_self());
#endif
			}
			else
			{
				_setThreadID.erase(pthread_self());
#if __APPLE__
                __delPthreadIntID(pthread_self());
#endif
			}

			_bDyeingFlag = (_setThreadID.size() > 0);
		}

	protected:

		/**
		 * buffer
		 */
		TC_ThreadQueue<pair<int, string> >  _buffer;

		/**
		 * ��
		 */
		TC_ThreadMutex          _mutex;

		/**
		 * �߳���
		 */
		TC_LoggerThreadGroup    *_pThreadGroup;

		/**
		 * �Ƿ��Ѿ�Ⱦɫ�ı�־
		 */
		static bool 			_bDyeingFlag;

		/**
		 * Ⱦɫ����
		 * 
		 * @author kevintian (2010-10-9)
		 */
		static TC_ThreadMutex	_mutexDyeing;

		/**
		 * Ⱦɫ���߳�ID����
		 * 
		 * @author kevintian (2010-10-9)
		 */
		static set<pthread_t> 	_setThreadID;
	};

	typedef TC_AutoPtr<TC_LoggerRoll> TC_LoggerRollPtr;

//////////////////////////////////////////////////////////////////////////////
//
/**
 * д��־�߳���
 * �ؼ���:ע����־��,�ᱣ��ְ��ָ��
 * ��֤��־����һֱ����
 */
	class TC_LoggerThreadGroup : public TC_ThreadLock
	{
	public:
		/**
		 * ���캯��
		 */
		TC_LoggerThreadGroup();

		/**
		 * ��������
		 */
		~TC_LoggerThreadGroup();

		/**
		 * �����߳�
		 * @param iThreadNum
		 */
		void start(size_t iThreadNum);

		/**
		 * ע��logger����
		 * @param l
		 */
		void registerLogger(TC_LoggerRollPtr &l);

		/**
		 * ж��logger����
		 * @param l
		 */
		void unRegisterLogger(TC_LoggerRollPtr &l);

		/**
		 * ˢ�����е�����
		 */
		void flush();

	protected:

		/**
		 * д��־
		 */
		void run();

		/**
		 * ָ��Ƚ�
		 */
		struct KeyComp
		{
			bool operator()(const TC_LoggerRollPtr& p1, const TC_LoggerRollPtr& p2) const
			{
				return p1.get() <  p2.get();
			}
		};

		typedef set<TC_LoggerRollPtr, KeyComp>  logger_set;

	protected:

		/**
		 * ����
		 */
		bool            _bTerminate;

		/**
		 * д�߳�
		 */
		TC_ThreadPool   _tpool;

		/**
		 * logger����
		 */
		logger_set      _logger;

	};

/**
 * �Զ���logger buffer
 */
	class LoggerBuffer : public std::basic_streambuf<char>
	{
	public:
		//��������buffer��С(10M)
		enum
		{
			MAX_BUFFER_LENGTH = 1024*1024*10,
		};

		/**
		 * 
		 */
		LoggerBuffer();

		/**
		 * ���캯��
		 * @param logger
		 */
		LoggerBuffer(TC_LoggerRollPtr roll, size_t buffer_len);

		/**
		 * ��������
		 */
		~LoggerBuffer();

	protected:

		/**
		 * ����ռ�
		 * @param n
		 */
		void reserve(std::streamsize n);

		/**
		 * ������
		 * @param s
		 * @param n
		 * 
		 * @return streamsize
		 */
		virtual streamsize xsputn(const char_type* s, streamsize n);

		/**
		 * buffer����, ����д����
		 * @param c
		 * 
		 * @return int_type
		 */
		virtual int_type overflow(int_type c);

		/**
		 * ���ռ�empty��(��ʵ��)
		 * 
		 * @return int_type
		 */
		virtual int_type underflow()    { return std::char_traits<char_type>::eof();}

		/**
		 * ����д�߼�
		 * 
		 * @return int
		 */
		virtual int sync();

	protected:
		LoggerBuffer(const LoggerBuffer&);
		LoggerBuffer& operator=(const LoggerBuffer&);

	protected:
		/**
		 * д��־
		 */ 
		TC_LoggerRollPtr    _roll;

		/**
		 * ������
		 */
		char*                       _buffer;

		/**
		 * ��������С
		 */
		std::streamsize     _buffer_len;
	};

/**
 * ��ʱ��, ������ʱ��д��־
 */
	class LoggerStream
	{
	public:
		/**
		 * ����
		 * @param stream
		 * @param mutex
		 */
		LoggerStream(ostream *stream, ostream *estream, TC_ThreadMutex &mutex) : _stream(stream), _estream(estream), _mutex(mutex)
		{
		}

		/**
		 * ����
		 */
		~LoggerStream()
		{
			if (_stream)
			{
				_stream->flush();
				_mutex.unlock();
			}
		}

		/**
		* ����<<
		*/
		template <typename P>
		LoggerStream& operator << (const P &t)  { if (_stream) *_stream << t;return *this;}

		/**
		 * endl,flush�Ⱥ���
		 */
		typedef ostream& (*F)(ostream& os);
		LoggerStream& operator << (F f)         { if (_stream) (f)(*_stream);return *this;}

		/**
		 * hex��ϵ�к���
		 */
		typedef ios_base& (*I)(ios_base& os);
		LoggerStream& operator << (I f)         { if (_stream) (f)(*_stream);return *this;}

		/**
		 * �ֶ�ת����ostream����
		 *
		 * @return ostream&
		 */
		operator ostream&()                     
		{ 
			if (_stream)
			{
				return *_stream; 
			}

			return *_estream; 
		}

		//��ʵ��
		LoggerStream(const LoggerStream& lt);
		LoggerStream& operator=(const LoggerStream& lt);

	protected:

		/**
		 * �����
		 */
		std::ostream    *_stream;

		/**
		 * 
		 */
		std::ostream    *_estream;

		/**
		 * ��
		 */
		TC_ThreadMutex  &_mutex;
	};

/**
 * ��־����
 */
	template<typename WriteT, template<class> class RollPolicy>
	class TC_Logger : public RollPolicy<WriteT>::RollWrapperI
	{
	public:

		/**
		 * ������ʾ����
		 */
		enum
		{
			HAS_TIME   = 0x01,	//�뼶��ʱ��
			HAS_PID    = 0x02,	//����ID
			HAS_LEVEL  = 0x04,	//��־�ȼ�
			HAS_MTIME  = 0x08	//���뼶��ʱ��(������ʹ��, ��Ӱ������)
		};

		/**
		* ö������,������־�����ֵȼ�
		*/
		enum
		{
			NONE_LOG    = 1,	//���е�log����д
			ERROR_LOG   = 2,	//д����log
			WARN_LOG    = 3,	//д����,����log
			DEBUG_LOG   = 4,	//д����,����,����log
			INFO_LOG    = 5		//д����,����,����,Info log
		};

		/**
		 * ��־��������
		 */
		static const string LN[6];

		/**
		 * ���캯��
		 */
		TC_Logger()
		: _flag(HAS_TIME)
		, _level(DEBUG_LOG)
		, _buffer(TC_LoggerRollPtr::dynamicCast(this->_roll), 1024)
		, _stream(&_buffer)
		, _ebuffer(NULL, 0)
		, _estream(&_ebuffer)
		{
		}

		/**
		 * ��������
		 */
		~TC_Logger()
		{
		}

		/**
		 * �޸ı���
		 * @param flag
		 * @param add
		 */
		void modFlag(int flag, bool add = true)
		{
			if (add)
			{
				_flag |= flag;
			}
			else
			{
				_flag &= ~flag;
			}
		}

		/**
		 * �Ƿ���ĳ��ʾ
		 *
		 * @return bool
		 */
		bool hasFlag(int flag) const    { return _flag & flag;}

		/**
		 * ��ȡflag
		 *
		 * @return int
		 */
		int getFlag() const             { return _flag;}

		/**
		* ��ȡ��־�ȼ�
		* @return int �ȼ�
		*/
		int getLogLevel() const         { return _level;}

		/**
		* ������־�ȼ�
		* @param level : �ȼ�
		* @return int
		*/
		int setLogLevel(int level)
		{
			if (!isLogLevelValid(level))
			{
				return -1;
			}

			_level = level;
			return 0;
		}

		/**
		 * ���õȼ�
		 * @param level
		 * @param int
		 */
		int setLogLevel(const string &level)
		{
			if (level == "ERROR")
			{
				return setLogLevel(ERROR_LOG);
			}
			else if (level == "WARN")
			{
				return setLogLevel(WARN_LOG);
			}
			else if (level == "DEBUG")
			{
				return setLogLevel(DEBUG_LOG);
			}
			else if (level == "NONE")
			{
				return setLogLevel(NONE_LOG);
			}
			else if (level == "INFO")
			{
				return setLogLevel(INFO_LOG);
			}
			return -1;
		}

		/**
		* DEBUG����־
		*/
		LoggerStream info()     { return stream(INFO_LOG);}

		/**
		* DEBUG����־
		*/
		LoggerStream debug()    { return stream(DEBUG_LOG);}

		/**
		* WARNING����־
		*/
		LoggerStream warn()     { return stream(WARN_LOG);}

		/**
		* ERROR����־
		*/
		LoggerStream error()    { return stream(ERROR_LOG);}

		/**
		* ��������־, ��ȼ��޹�
		*/
		LoggerStream any()      { return stream(-1);}

	protected:

		/**
		 * ͷ����Ϣ
		 * @param c
		 * @param len
		 * @param level
		 */
		void head(char *c, int len, int level)
		{
			size_t n = 0;

			if (hasFlag(TC_Logger::HAS_MTIME))
			{
				struct timeval t;
				TC_TimeProvider::getInstance()->getNow(&t);
				//gettimeofday(&t, NULL);

				tm tt;
				localtime_r(&t.tv_sec, &tt);
				n += snprintf(c + n, len-n, "%04d-%02d-%02d %02d:%02d:%02d.%03ld|", 
							  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, t.tv_usec/1000);
			}
			else if (hasFlag(TC_Logger::HAS_TIME))
			{
				time_t t = TC_TimeProvider::getInstance()->getNow();
				tm tt;
				localtime_r(&t, &tt);
				n += snprintf(c + n, len-n, "%04d-%02d-%02d %02d:%02d:%02d|", 
							  tt.tm_year + 1900, tt.tm_mon+1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec);
			}

			if (hasFlag(TC_Logger::HAS_PID))
			{
				n += snprintf(c + n, len - n, "%ld%s", syscall(SYS_gettid), "|");
			}

			if (hasFlag(TC_Logger::HAS_LEVEL))
			{
				n += snprintf(c + n, len - n, "%s%s", TC_Logger::LN[level].c_str(), "|");
			}
		}

		/**
		 * ��
		 * @param level
		 * 
		 * @return LoggerStream
		 */
		LoggerStream stream(int level)
		{
			ostream *ost = NULL;

			if (level <= _level)
			{
				char c[128] = "\0";
				head(c, sizeof(c) - 1, level);

				_mutex.lock(); 
				ost = &_stream;
				_stream.clear();
				_stream << c;
			}

			return LoggerStream(ost, &_estream, _mutex);
		}

		/**
		* ���̵ȼ��Ƿ���Ч
		* @param level : ���̵ȼ�
		* @return bool,�ȼ��Ƿ���Ч
		*/
		bool isLogLevelValid(int level)
		{
			switch (level)
			{
			case NONE_LOG:
			case ERROR_LOG:
			case WARN_LOG:
			case DEBUG_LOG:
			case INFO_LOG:
				return true;
				break;
			}

			return false;
		}

	protected:
		/**
		 * ��ʾͷ
		 */
		int             _flag;

		/**
		* ��־��ߵȼ�
		*/
		int             _level;

		/**
		 * buffer
		 */
		LoggerBuffer    _buffer;

		/**
		 * logger��ʱ��
		 */
		std::ostream    _stream;

		/**
		 * ��buffer
		 */
		LoggerBuffer   _ebuffer;

		/**
		 * ����
		 */
		std::ostream   _estream;

		/**
		 * ��
		 */
		TC_ThreadMutex  _mutex;
	};

	template<typename WriteT, template<class> class RollPolicy>
	const string TC_Logger<WriteT, RollPolicy>::LN[6] = {"", "", "ERROR", "WARN", "DEBUG", "INFO"};

////////////////////////////////////////////////////////////////////////////////

	class RollWrapperInterface
	{
	public:

		virtual ~RollWrapperInterface() {}

		/**
		* DEBUG����־
		*/
		virtual LoggerStream info() = 0;

		/**
		* DEBUG����־
		*/
		virtual LoggerStream debug() = 0;

		/**
		* WARNING����־
		*/
		virtual LoggerStream warn() = 0; 

		/**
		* ERROR����־
		*/
		virtual LoggerStream error() = 0;

		/**
		* ��������־, ��ȼ��޹�
		*/
		virtual LoggerStream any() = 0;

		/**
		 * ������첽���ã������Ͻ���ˢ��
		 */
		virtual void flush() = 0;
	};


	template<typename RollPolicyWriteT>
	class RollWrapperBase : public RollWrapperInterface
	{
	public:

		typedef TC_AutoPtr<RollPolicyWriteT>     RollPolicyWriteTPtr;

		/**
		 * ����
		 */
		RollWrapperBase()
		{
			_roll = new RollPolicyWriteT;
		}

		/**
		 * ��װ�߳�
		 * @param ltg
		 */
		void setupThread(TC_LoggerThreadGroup *ltg) { _roll->setupThread(ltg);}

		/**
		 * ȡ���߳�
		 */
		void unSetupThread()                        { _roll->unSetupThread();}

		/**
		 * ��ȡд����
		 *
		 * @return WriteT&
		 */
		typename RollPolicyWriteT::T &getWriteT()   { return _roll->getWriteT();}

		/**
		 * д��־
		 * @param buffer
		 */
		void roll(const pair<int, string> &buffer)  { _roll->write(buffer);}

		/**
		* ��ȡrollʵ��
		 * 
		 * @return RollPolicyWriteTPtr&
		 */
		RollPolicyWriteTPtr & getRoll()          	{return _roll;}

		/**
		 * �첽ˢ��
		 */
		void flush()								{ _roll->flush(); }

	protected:

		/**
		 * ����д��־������
		 */
		RollPolicyWriteTPtr      _roll;

	};

////////////////////////////////////////////////////////////////////////////////
/**
 * ��־��������, ������־��С����
 */
	template<typename WriteT>
	class TC_RollBySize : public TC_LoggerRoll, public TC_ThreadMutex
	{
	public:
		typedef WriteT T;

		/**
		 * ��װ��(�ṩ�ӿ�)
		 */
		class RollWrapperI : public RollWrapperBase<TC_RollBySize<WriteT> >
		{
		public:
			/**
			 * ��ʼ��
			 * @param path
			 * @param iMaxSize, �ֽ�
			 * @param iMaxNum
			 */
			void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
			{
				this->_roll->init(path, iMaxSize, iMaxNum);
			}

			/**
			 * ��־·��
			 *
			 * @return string
			 */
			string getPath()                    { return this->_roll->getPath();}

			/**
			 * �����ļ�·��
			 */
			void setPath(const string &path)    { this->_roll->setPath(path);}

			/**
			 * ��ȡ����С
			 *
			 * @return int
			 */
			int getMaxSize()                    { return this->_roll->getMaxSize();}

			/**
			 * ��������С
			 * @param maxSize
			 */
			void setMaxSize(int maxSize)        { this->_roll->setMaxSize(maxSize);}

			/**
			 * ������
			 *
			 * @return int
			 */
			int getMaxNum()                     { return this->_roll->getMaxNum();}

			/**
			 * �����ļ�����
			 * @param maxNum
			 */
			void setMaxNum(int maxNum)          { this->_roll->setMaxNum(maxNum);}
		};

		/**
		 * ���캯��
		 */
		TC_RollBySize() : _iUpdateCount(0), _lt(time(NULL))
		{
		}

		/**
		 * ����
		 */
		~TC_RollBySize()
		{
			if (_of.is_open())
			{
				_of.close();
			}
		}

		/**
		 * ��ʼ��
		 * @param path, �ļ���
		 * @param iMaxSize, ����С
		 * @param iMaxNum, ������
		 */
		void init(const string &path, int iMaxSize = 5000000, int iMaxNum = 10)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			_path   = path;
			_maxSize= iMaxSize;
			_maxNum = iMaxNum;
		}

		/**
		 * ��־·��
		 *
		 * @return string
		 */
		string getPath()                    { TC_LockT<TC_ThreadMutex> lock(*this); return _path;}

		/**
		 * ����·��
		 */
		void setPath(const string &path)    { TC_LockT<TC_ThreadMutex> lock(*this); _path = path;}

		/**
		 * ��ȡ����С
		 *
		 * @return int
		 */
		int getMaxSize()                    { TC_LockT<TC_ThreadMutex> lock(*this); return _maxSize;}

		/**
		 * ��������С
		 * @param maxSize
		 *
		 * @return void
		 */
		void setMaxSize(int maxSize)        { TC_LockT<TC_ThreadMutex> lock(*this); return _maxSize = maxSize;}

		/**
		 * ������
		 *
		 * @return int
		 */
		int getMaxNum()                     { TC_LockT<TC_ThreadMutex> lock(*this); return _maxNum;}

		/**
		 * ������
		 * @param maxNum
		 */
		void setMaxNum(int maxNum)          { TC_LockT<TC_ThreadMutex> lock(*this); return _maxNum = maxNum;}

		/**
		 * ��ȡдʾ��
		 *
		 * @return T&
		 */
		WriteT &getWriteT()                 { return _t;}

		/**
		 * ��������
		 *
		 * @param string
		 */
		void roll(const deque<pair<int, string> > &buffer)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			if (_path.empty())
			{
				_t(cout, buffer);
				return;
			}

			time_t t = TC_TimeProvider::getInstance()->getNow();
			time_t tt= t - _lt;
			//ÿ��5, ���´�һ���ļ�, �����ļ���ɾ���������ͷ�
			if (tt > 5 || tt < 0)
			{
				_lt = t;
				_of.close();
			}

			//�����־�ļ��Ƿ��
			if (!_of.is_open())
			{
				string sLogFileName = _path + ".log";
				_of.open(sLogFileName.c_str(), ios::app);

				string sLogFilePath = TC_File::extractFilePath(_path);

				if (!TC_File::isFileExist(sLogFilePath,S_IFDIR))
				{
					TC_File::makeDirRecursive(sLogFilePath);
				}

				if (!_of)
				{
					//���쳣ǰ��������_t �Ա��Զ����־
					_t(_of, buffer);

					throw TC_Logger_Exception("[TC_RollBySize::roll]:fopen fail: " + sLogFileName, errno);
				}

			}

			_t(_of, buffer);

			if (tt <= 5)
			{
				return;
			}

			//�ļ���СС������, ֱ�ӷ���
			if (_of.tellp() < _maxSize)
			{
				return;
			}

			//�ļ���С��������,ɾ�����һ���ļ�
			string sLogFileName = _path + TC_Common::tostr(_maxNum - 1) + ".log";
			if (access(sLogFileName.c_str(), F_OK) == 0)
			{
				if (remove(sLogFileName.c_str()) < 0 )
				{
					return;
				}
			}

			//��log�ļ�����shift, xxx1.log=>xxx2.log,��һ�ļ�����Ϊxxx.log
			for (int i = _maxNum-2; i >= 0; i--)
			{
				if (i == 0)
				{
					sLogFileName = _path + ".log";
				}
				else
				{
					sLogFileName = _path + TC_Common::tostr(i) + ".log";
				}

				if (access(sLogFileName.c_str(), F_OK) == 0)
				{
					string sNewLogFileName = _path + TC_Common::tostr(i + 1) + ".log";
					rename(sLogFileName.c_str(), sNewLogFileName.c_str());
				}
			}

			_of.close();
			_of.open(sLogFileName.c_str(), ios::app);
			if (!_of)
			{
				throw TC_Logger_Exception("[TC_RollBySize::roll]:fopen fail: " + sLogFileName, errno);
			}
		}

	protected:

		/**
		 * �ļ�·��
		 */
		string      _path;

		/**
		* ��־�ļ�������С
		*/
		int         _maxSize;

		/**
		* log�ļ������log�ļ�����Ϊ:xxx.log xxx1.log ...
		* xxx[_maxNum-1].log,_maxNum<=1 ?xxx.log
		*/
		int         _maxNum;

		/**
		 * ��־�ļ�
		 */
		ofstream    _of;

		/**
		 * ����д����
		 */
		WriteT      _t;

		/**
		 * �೤ʱ����һ���ļ���С
		 */
		short       _iUpdateCount;

		/**
		 * �೤ʱ����һ���ļ���С
		 */
		time_t      _lt;
	};

	typedef TC_Logger<TC_DefaultWriteT, TC_RollBySize> TC_RollLogger;

/**
 * ����ʱ�������־
 */
	template<typename WriteT>
	class TC_RollByTime : public TC_LoggerRoll, public TC_ThreadMutex
	{
	public:

		typedef WriteT T;

		/**
		 * ��װ��(�ӿ���)
		 */
		class RollWrapperI : public RollWrapperBase<TC_RollByTime<WriteT> >
		{
		public:

			/**
			 * ��ʼ��
			 * @param path
			 * @param iMaxSize
			 * @param iMaxNum
			 */
			void init(const string &path, const string &format = "%Y%m%d")
			{
				this->_roll->init(path, format);
			}

			/**
			 * ��־·��
			 *
			 * @return string
			 */
			string getPath()                        { return this->_roll->getPath();}

			/**
			 * �����ļ�·��
			 */
			void setPath(const string &path)        { this->_roll->setPath(path);}

			/**
			 * ��ȡ��ʽ
			 *
			 * @return string
			 */
			string getFormat()                      { return this->_roll->getFormat();}

			/**
			 * ���ø�ʽ
			 * @param format
			 */
			void setFormat(const string &format)    { this->_roll->setFormat(format);}
		};

		/**
		 * ����
		 */
		TC_RollByTime() : _lt(TC_TimeProvider::getInstance()->getNow())
		{
		}

		/**
		 * ����
		 */
		~TC_RollByTime()
		{
			if (_of.is_open())
			{
				_of.close();
			}
		}

		/**
		 * ��ʼ��
		 * @param path, �ļ�·��
		 * @param format, ��ʽ
		 */
		void init(const string &path, const string &format = "%Y%m%d")
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			_path       = path;
			_format     = format;
			_currentTime= TC_Common::tm2str(TC_TimeProvider::getInstance()->getNow(), _format);
		}

		/**
		 * ��־·��
		 *
		 * @return string
		 */
		string getPath()                    { TC_LockT<TC_ThreadMutex> lock(*this); return _path;}

		/**
		 * �����ļ�·��
		 */
		void setPath(const string &path)    { TC_LockT<TC_ThreadMutex> lock(*this); _path = path;}

		/**
		 * ��ȡ��ʽ
		 *
		 * @return string
		 */
		string getFormat()                  { TC_LockT<TC_ThreadMutex> lock(*this); return _format;}

		/**
		 * ���ø�ʽ
		 * @param format
		 */
		void setFormat(const string &format){ TC_LockT<TC_ThreadMutex> lock(*this); _format = format;}

		/**
		 * ��ȡдʾ��
		 *
		 * @return WriteT&
		 */
		WriteT &getWriteT()                 { return _t;}

		/**
		 * ��������
		 * @param path
		 * @param of
		 *
		 * @return string
		 */
		void roll(const deque<pair<int, string> > &buffer)
		{
			TC_LockT<TC_ThreadMutex> lock(*this);

			if (_path.empty())
			{
				_t(cout, buffer);
				return;
			}

			time_t t = TC_TimeProvider::getInstance()->getNow();

			string nowTime = TC_Common::tm2str(t, _format);

			//���ʱ���Ƿ��ʱ����, ��ʱ���ر��ļ�
			if (_currentTime != nowTime)
			{
				_currentTime = nowTime;
				_of.close();
			}

			//ÿ��10s, ���´�һ���ļ�, �����ļ���ɾ���������ͷ�
			if (t - _lt > 10 || t - _lt < 0)
			{
				_lt = t;
				_of.close();
			}

			if (!_of.is_open())
			{
				string sLogFileName = _path + "_" + nowTime + ".log";

				string sLogFilePath = TC_File::extractFilePath(_path);
				if (!TC_File::isFileExist(sLogFilePath,S_IFDIR))
				{
					TC_File::makeDirRecursive(sLogFilePath);
				}

				_of.open(sLogFileName.c_str(), ios::app);

				if (!_of)
				{
					//���쳣ǰ��������_t �Ա��Զ����־
					_t(_of, buffer);
					throw TC_Logger_Exception("[TC_RollByTime::roll]:fopen fail: " + sLogFileName, errno);
				}
			}

			//д
			_t(_of, buffer);
		}

	protected:
		/**
		 * �ļ�·��
		 */
		string      _path;

		/**
		 * ʱ���ʽ
		 */
		string      _format;

		/**
		 * �ϴ�rollʱ��ʱ��
		 */
		string      _currentTime;

		/**
		 * ��־�ļ�
		 */
		ofstream    _of;

		/**
		 * ����д����
		 */
		WriteT      _t;

		/**
		 * �೤ʱ����һ���ļ���С
		 */
		time_t      _lt;
	};

	typedef TC_Logger<TC_DefaultWriteT, TC_RollByTime> TC_DayLogger;

}

#endif

