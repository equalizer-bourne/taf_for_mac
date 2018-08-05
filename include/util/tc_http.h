#ifndef __TC_HTTP_H_
#define __TC_HTTP_H_

#include "util/tc_ex.h"
#include "util/tc_common.h"
#include "util/tc_autoptr.h"
#include "util/tc_thread.h"
#include "util/tc_socket.h"
#include "util/tc_epoller.h"
#include "util/tc_timeout_queue.h"
#include <map>
#include <sstream>
#include <cassert>
#include <vector>

namespace taf
{
/////////////////////////////////////////////////
// ˵��: http��
// 1 ֧��httpЭ��Ľ���,֧��GET/POSTģʽ
// 2 ��Ƕ֧�ֶ�chunk����Ľ���
// 3 ֧��ͬ��http�������Ӧ(ֻ֧�ֶ�����,��֧��keep-alive)
// 4 ֧��http��Ӧ�������������
// 5 ����Cookie������
// Author : j@syswin.com              
// Modify: @syswin.com              
/////////////////////////////////////////////////

/**
* httpЭ������쳣��
*/
struct TC_Http_Exception : public TC_Exception
{
    TC_Http_Exception(const string &sBuffer) : TC_Exception(sBuffer){};
    ~TC_Http_Exception() throw(){};
};

/**
 * httpӦ��Э������쳣��
 */
struct TC_HttpResponse_Exception : public TC_Http_Exception
{
    TC_HttpResponse_Exception(const string &sBuffer) : TC_Http_Exception(sBuffer){};
    ~TC_HttpResponse_Exception() throw(){};
};

/**
 * http����Э������쳣��
 */
struct TC_HttpRequest_Exception : public TC_Http_Exception
{
    TC_HttpRequest_Exception(const string &sBuffer) : TC_Http_Exception(sBuffer){};
    ~TC_HttpRequest_Exception() throw(){};
};

/**
 * �򵥵�URL������
 * ftp://user:password@www.qq.com:8080/abc/dev/query?a=b&c=3#ref
 * scheme: ftp
 * user:user
 * pass:password
 * domain:www.qq.com
 * port:8080
 * path:/abc/dev/query
 * query:a=b&c=3
 * ref:ref
 * request:/abc/dev/query?a=b&c=3#ref
 * relativePath:/abc/dev/
 * rootPath:ftp://user:password@www.qq.com:8080/
 */

class TC_URL
{
public:
    enum URL_TYPE
    {
        HTTP,
        HTTPS,
        FTP
    };

    /**
     * ����
     */
    TC_URL() : _iURLType(HTTP)
    {
    }

    /**
     * ����URL, url�����Ǿ���·��
     * @throws TC_URL_Exception
     * @param sURL
     */
    bool parseURL(const string &sURL);

    /**
     * ���
     */
    void clear();

    /**
     * ��ȡscheme
     * 
     * @return const string&
     */
    string getScheme() const;

    /**
     * ��ȡ�û���
     * 
     * @return const string&
     */
    string getUserName() const;

    /**
     * ��ȡ����
     * 
     * @return const string&
     */
    string getPassword() const;

    /**
     * ��ȡ����
     * 
     * @return const string&
     */
    string getDomain() const;

    /**
     * ��ȡ�˿�
     * 
     * @return const string&
     */
    string getPort() const;

    /**
     * �Ƿ���ȱʡ�Ķ˿�
     * 
     * @return bool
     */
    bool isDefaultPort() const;

    /**
     * ��ȡ·��
     * 
     * @return const string&
     */
    string getPath() const;

    /**
     * ��ȡ��ѯ����
     * 
     * @return string
     */
    string getQuery() const;

    /**
     * ������Host
     * ����http://www.qq.com/abc?a=b#ref��Ϊ:/abc?a=b#ref
     *
     * @return string
     */
    string getRequest() const;

    /**
     * ��ȡRef
     * 
     * @return const string&
     */
    string getRef() const;

    /**
     * �Ƿ���Ч
     * 
     * @return bool
     */
    bool isValid() const;

    /**
     * ��ȡ������URL
     * 
     * @return const string&
     */
    string getURL() const;

    /**
     * URL����
     * 
     * @return int
     */
    int getType() const { return _iURLType; }

    /**
     * ��ȡ���·��
     * 
     * @return string
     */
    string getRelativePath() const;

    /**
     * ��ȡ��·��
     * 
     * @return string
     */
    string getRootPath() const;

    /**
     * �Ե�ǰURL����·�� 
     * 1 http://www.qq.com/a/b?a=b, /test/abc => http://www.qq.com/test/abc
     * 2 http://www.qq.com/a/b?a=b, test/abc   => http://www.qq.com/a/b/test/abc
     * 3 http://www.qq.com/a/b?a=b, ../test/abc   => http://www.qq.com/a/test/abc
     * 
     * @param sRelativeURL: ��Ե�ǰURL�ĵ�ַ
     */
    TC_URL buildWithRelativePath(const string &sRelativeURL) const;

    /**
     * ������
     */
    void specialize();

protected:
    /**
     * ����URL
     * 
     * @return string
     */
    string toURL() const;

    /**
     * 
     * @param frag
     * 
     * @return string
     */
    string getFragment(const string& frag) const;

    /**
     * ���͵��ַ���
     * 
     * @return string
     */
    string type2String() const;

    /**
     * ��ȡЭ���ȱʡ�˿�
     * 
     * @return string
     */
    string getDefaultPort() const;

    /**
     * ��URL
     * @param sPath
     * 
     * @return string
     */
    string simplePath(const string &sPath) const;

protected:
    /**
     * �������URL
     */
    string              _sURL;

    /**
     * URL����
     */
    int                 _iURLType;

    /**
     * ÿһ��
     */
    map<string, string> _fragment;
};

/**
 * httpЭ�������, �������Ӧ���ڸ����н���
 */
class TC_Http
{
public:
    /**
     * ���캯��
     */
    TC_Http()
    {
        TC_Http::reset();
        setConnection("close");     //Ĭ�Ͼ��ö�����
    }

    //������ͷ����Сд
    struct CmpCase
    {
        bool operator()(const string &s1, const string &s2) const
        {
            return TC_Common::upper(s1) < TC_Common::upper(s2);
        }
    };

    typedef multimap<string, string, CmpCase> http_header_type;

    /**
     * ɾ��ͷ��
     * @param sHeader
     */
    void eraseHeader(const string &sHeader) { _headers.erase(sHeader); }

    /**
     * ���� Cache-Control
     * @param sCacheControl
     */
    void setCacheControl(const string &sCacheControl){setHeader("Cache-Control", sCacheControl);}

    /**
     * ���� Connection
     * @param sConnection
     */
    void setConnection(const string &sConnection){setHeader("Connection", sConnection);}

    /**
     * ���� Content-Type
     * @param sContentType
     */
    void setContentType(const string &sContentType){setHeader("Content-Type", sContentType);}

    /**
     * �������ݳ���
     * @param iContentLength
     */
    void setContentLength(size_t iContentLength)
    {
        setHeader("Content-Length", TC_Common::tostr(iContentLength));
    }

    /**
     * ���� Referer
     * @param sReferer
     */
    void setReferer(const string &sReferer){setHeader("Referer", sReferer);}

    /**
     * ���� Host, ���� www.qq.com:80
     * @param sHost
     */
    void setHost(const string &sHost){setHeader("Host", sHost);}

    /**
     * ���� Accept-Encoding
     * @param sAcceptEncoding, gzip, compress, deflate, identity
     */
    void setAcceptEncoding(const string &sAcceptEncoding){setHeader("Accept-Encoding", sAcceptEncoding);}

    /**
     * ���� Accept-Language
     * @param sAcceptLanguage
     */
    void setAcceptLanguage(const string &sAcceptLanguage){setHeader("Accept-Language", sAcceptLanguage);}

    /**
     * ���� Accept
     * @param sAccept
     */
    void setAccept(const string &sAccept){setHeader("Accept", sAccept);}

    /**
     * ���� Transfer-Encoding
     * @param sTransferEncoding
     */
    void setTransferEncoding(const string& sTransferEncoding) {setHeader("Transfer-Encoding", sTransferEncoding); }

    /**
     * ����header.���õ�ֵ��ʹ���Ѿ��е�get/set�������� 
     * (��Set-Cookie&Cookie��,ͷ���ֶβ����ظ�)
     * @param sHeadName 
     * @param sHeadValue 
     */
    void setHeader(const string &sHeadName, const string &sHeadValue) 
    {
        //Set-Cookie��Cookie�����ж��ͷ
        if(TC_Common::upper(sHeadName) != "SET-COOKIE" && TC_Common::upper(sHeadName) != "COOKIE")
        {
            _headers.erase(sHeadName);
        }
        _headers.insert(multimap<string, string>::value_type(sHeadName, sHeadValue)); 
    }

    /**
     * ����header.���õ�ֵ��ʹ���Ѿ��е�get/set������
     * (ͷ���ֶο����ظ�)
     * @param sHeadName
     * @param sHeadValue 
     */
    void setHeaderMulti(const string &sHeadName, const string &sHeadValue) 
    {
        _headers.insert(multimap<string, string>::value_type(sHeadName, sHeadValue)); 
    }

    /**
     * ��ȡͷ(�ظ���Ҳ��ȡ)
     * @param sHeadName
     * 
     * @return vector<string>
     */
    vector<string> getHeaderMulti(const string &sHeadName) const;

    /**
     * ��ȡhttp���ݳ���
     *
     * @return size_t
     */
    size_t getContentLength() const;

    /**
     * ��ȡhttp����Host
     *
     * @return string
     */
    string getHost() const;

    /**
     * ��ȡhttpͷ������
     *
     * @return size_t
     */
    size_t getHeadLength() const { return _headLength; }

    /**
     * ��ȡhttp����
     *
     * @return string
     */
    string getContent() const { return _content; }

    /**
     * ����http body(Ĭ�ϲ��޸�content-length)
     * @param content
     * @param bUpdateContentLength, �Ƿ����ContentLength
     */
    void setContent(const string &content, bool bUpdateContentLength = false) 
    {
        _content = content; 

        if(bUpdateContentLength)
        {
            eraseHeader("Content-Length");
            if(_content.length() > 0)
                setContentLength(_content.length());
        }
    }

    /**
     * ��ȡ��������
     *
     * @return string
     */
    string getContentType() const;

    /**
     * ��ȡhttpͷ������
     * (Set-Cookie��Cookie��Ҫ�������ȡ, �����к�����ȡ)
     * @param sHeader
     *
     * @return string
     */
    string getHeader(const string& sHeader) const;

    /**
     * ��ȡhttpͷ��map
     *
     * @return http_header_type&
     */
     const http_header_type& getHeaders() const{return _headers;}

     /**
      * ����
      */
     void reset();

    /**
     * ��ȡһ��
     * @param ppChar
     *
     * @return string
     */
    static string getLine(const char** ppChar);

	/**
     * ��ȡһ��
     * @param ppChar
     * @param iBufLen
     *
     * @return string
     */
    static string getLine(const char** ppChar, int iBufLen);

    /**
     * ����ͷ���ַ���(��������һ��)
     *
     * @return string
     */
    string genHeader() const;

    /**
     * ��������head(��������һ��,��һ�����������Ӧ����һ��)
     * ��������ݽ���Ϊmap��ʽ
     * @param szBuffer
     *
     * @return const char*, ƫ�Ƶ�ָ��
     */
    static const char* parseHeader(const char* szBuffer, http_header_type &sHeader);

protected:

    /**
     * httpͷ�����б���
     */
    http_header_type    _headers;

    /**
     * httpͷ������
     */
    size_t              _headLength;

    /**
     * httpͷ������
     */
    string              _content;
};

/********************* TC_HttpCookie***********************/

/**
 * �򵥵�Cookie������
 */
class TC_HttpCookie
{
public:
    typedef map<string, string, TC_Http::CmpCase> http_cookie_data;

    struct Cookie
    {
        http_cookie_data    _data;
        string              _domain;
        string              _path;
        time_t              _expires;       //����0��ʾֻ�ڵ�ǰ�ػ���Ч
        bool                _isSecure;
    };

    /**
     * �������
     */
    void clear();

    /**
     * ����Cookie����������
     * @param sCookieRspURL: ������Cookie HTTP��Ӧ��URL
     * @param vCookies: Set-Cookie�ַ���
     */
    void addCookie(const string &sCookieRspURL, const vector<string> &vCookies);

    /**
     * ����Cookie��������
     * @param cookie
     */
    void addCookie(const Cookie &cookie);

    /**
     * ����Cookie��������
     * @param cookie
     */
    void addCookie(const list<Cookie> &cookie);

    /**
     * ��ȡĳ��url��cookie��ֵ��
     * ȥ����Domain,Path���ֶ�
     * 
     * @param sReqURL: ��Ҫƥ���URL
     * @param cookies
     */
    void getCookieForURL(const string &sReqURL, list<Cookie> &cookies);

    /**
     * ��ȡĳ��url��cookie��ֵ��
     * ȥ����Domain,Path���ֶ�
     * 
     * @param sReqURL: ��Ҫƥ���URL
     * @param http_cookie_type
     * @param string
     */
    void getCookieForURL(const string &sReqURL, string &sCookie);

    /**
     * ƥ������
     * sCookieDomain���ж����
     * sCookieDomainδ��.��ͷ, ��sCookieDomain=sDomain
     * sCookieDomain��.��ͷ,��sDomain�ұ�ƥ��sDomain,sDomain��sCookieDomain������һ��.
     * (x.y.z.com ƥ�� .y.z.com, x.y.z.com ��ƥ��.z.com )
     * 
     * @param sCookieDomain
     * @param sDomain
     * 
     * @return bool
     */
    static bool matchDomain(const string &sCookieDomain, const string &sDomain);

    /**
     * ƥ��·��
     * sCookiePath��sPath��ǰ׺
     * 
     * @param sCookiePath
     * @param sPath
     * 
     * @return size_t, ƥ��·���ĳ���
     */
    static size_t matchPath(const string &sCookiePath, const string &sPath);

    /**
     * ��ȡ���е�Cookie
     * 
     * @return list<Cookie>&
     */
    list<Cookie> getAllCookie();

    /**
     * ɾ�����ڵ�Cookie
     * ���������뵱ǰ�ػ���Cookie��ɾ��
     * @param t:������ǰʱ���cookie��ɾ��
     * @param bErase: true:�����ڵ�ǰ�ػ���CookieҲɾ��
     *                false:�����ڵ�ǰ�ػ���Cookie��ɾ��
     */
    void deleteExpires(time_t t, bool bErase = false);

    /**
     * ��������û�й��ڵ�Cookie����Expires�ֶε�Cookie
     * �����������뵱ǰ�ػ���Cookie�ǲ����ص�
     * ͨ�������������л�
     * 
     * @param t:������ǰʱ���cookie��ɾ��
     * 
     * @return list<Cookie>
     */
    list<Cookie> getSerializeCookie(time_t t);

protected:
    /**
     * �ж�Cookie����Ч
     * @param tURL
     * 
     * @return size_t
     */
    size_t isCookieMatch(const Cookie &cookie, const TC_URL &tURL) const;

    /**
     * ���Cookie�Ƿ����
     * ��ǰ�ػ��Ĳ������(expires=0)
     * @param cookie
     * 
     * @return bool
     */
    bool isCookieExpires(const Cookie &cookie) const;

    /**
     * ���Cookie
     * @param cookie
     * @param cookies
     */
    void addCookie(const Cookie &cookie, list<Cookie> &cookies);

    /**
     * ����Domain
     * @param sDomain
     * 
     * @return string
     */
    bool fixDomain(const string &sDomain, string &sFixDomain);

protected:

    /**
     * ����Cookie
     * key:domain+path
     */
    list<Cookie> _cookies;
};

/********************* TC_HttpResponse ***********************/

class TC_HttpResponse : public TC_Http
{
public:

    /**
     * ����
     */
    TC_HttpResponse()
    {
        TC_HttpResponse::reset();
    }

    /**
     * reset��Ӧ��
     */
    void reset();

    /**
     * ����decode,�����buffer���Զ��ڽ��������б������
     * ����decode֮ǰ����reset
     * (������յ�bufferֱ����ӵ�sBuffer���漴��, Ȼ����������)
     * (�ܹ�����������TC_HttpResponse���Զ���sBuffer����������ֱ�����������ϻ��߽�������true)
     * @param buffer
     * @throws TC_HttpResponse_Exception, ��֧�ֵ�httpЭ��, �׳��쳣
     * @return bool,
     *      true:������һ��������buffer
     *      false:����Ҫ��������(��������������ر����ӵ�ģʽ��, ��������������ر����ӵ�ģʽ��, Ҳ���ܲ���Ҫ�ٽ�����)
     *      )
     * 
     */
    bool incrementDecode(string &sBuffer);

    /**
     * ����httpӦ��(����string��ʽ)
     * ע��:���httpͷ��û��Content-Length�ҷ�chunkģʽ, �򷵻�true
     * ��Ҫ������ж�(http����������ر�����Ҳ���Ƿ���http��Ӧ������)
     * @param sBuffer
     * @return bool, sBuffer�Ƿ���������http����
     */
    bool decode(const string &sBuffer);

    /**
     * ����httpӦ��
     * ע��:���httpͷ��û��Content-Length�ҷ�chunkģʽ, �򷵻�true
     * ��Ҫ������ж�(http����������ر�����Ҳ���Ƿ���http��Ӧ������)
     * @param sBuffer
     * @return bool, sBuffer�Ƿ���������http����
     */
    bool decode(const char *sBuffer, size_t iLength);

    /**
     * ����Ӧ���(����string��ʽ)
     * @param sBuffer
     *
     * @return string
     */
    string encode() const;

    /**
     * ����Ӧ���(����vector<char>��ʽ)
     * @param sBuffer
     *
     * @return string
     */
    void encode(vector<char> &sBuffer) const;

    /**
     * ��ȡ��һ��(HTTP/1.1 200 OK)
     * 
     * @return string
     */
    string getResponseHeaderLine() const { return _headerLine; }

    /**
     * ��ȡHTTP��Ӧ״̬(����200)
     *
     * @return int
     */
    int  getStatus() const {return _status; }

    /**
     * ����״̬
     * @param status
     */
    void setStatus(int status) { _status = status; }

    /**
     * ����(����OK)
     * 
     * @return string
     */
    string getAbout() const { return _about; }

    /**
     * ��������
     * @param about
     */
    void setAbout(const string &about) { _about = about; }

    /**
     * ��ȡ�汾, ����HTTP/1.1
     * 
     * @return string
     */
    string getVersion() const { return _version; }

    /**
     * ����ģʽ:HTTP/1.1(Ĭ��)����HTTP/1.0
     * @param sVersion
     */
    void setVersion(const string &sVersion) { _version = sVersion; }

    /**
     * ����Ӧ��״̬(����string��ʽ)
     * @param status
     * @param about
     * @param sBody, postЭ��body������
     */
    void setResponse(int status = 200, const string& about = "OK", const string& sBody = "");

    /**
     * ����Ӧ��״̬
     * @param status
     * @param about
     * @param sBuffer, postЭ��body������
     * @param iLength, sBuffer����
     */
    void setResponse(int status, const string& about, const char *sBuffer, size_t iLength);

    /**
     * ����Ӧ��, ȱʡstatus=200, about=OK
     * @param sBuffer
     * @param iLength
     */
    void setResponse(const char *sBuffer, size_t iLength);

    /**
     * ���÷���
     * @param sServer
     */
    void setServer(const string &sServer){setHeader("Server", sServer);}

    /**
     * ����Set-Cookie
     * @param sCookie
     */
    void setSetCookie(const string &sCookie){setHeader("Set-Cookie", sCookie);}

    /**
     * ��ȡSet-Cookie
     * 
     * @return vector<string>
     */
    vector<string> getSetCookie() const;

    /**
     * ����Ӧ��ͷ
     * @param szBuffer
     *
     * @return
     */
    void parseResponseHeader(const char* szBuffer);

protected:

    /**
     * Ӧ��״̬
     */
    int     _status;

    /**
     * Ӧ������
     */
    string  _about;

    /**
     * ��ȡ�汾
     */
    string  _version;

    /**
     * ��ȡ��һ��
     */
    string  _headerLine;

    /**
     * ��ʱ��content length
     */
    size_t  _iTmpContentLength;
};

/********************* TC_HttpRequest ***********************/

class TC_HttpRequest : public TC_Http
{
public:

    ///////////////////////////////////////////////////////////////////
    TC_HttpRequest()
    {
        TC_HttpRequest::reset();
        setUserAgent("TC_Http");
    }

    /**
     * ����Э������
     */
    enum
    {
        REQUEST_GET,
        REQUEST_POST
    };

    /**
     * ���http�����Ƿ���ȫ
     * @param sBuffer
     * @throws TC_HttpRequest_Exception, ��֧�ֵ�httpЭ��, �׳��쳣
     * @return bool, true: ��ȫ, false:��ȫ
     */
    static bool checkRequest(const char* sBuffer, size_t len);

    /**
     * ����
     */
    void reset();

    /**
     * ���� User-Agent
     * @param sUserAgent
     */
    void setUserAgent(const string &sUserAgent){setHeader("User-Agent", sUserAgent);}

    /**
     * ���� Cookie
     * @param sCookie
     */
    void setCookie(const string &sCookie){setHeader("Cookie", sCookie);}

    /**
     * ��ȡԭʼCookie��
     * 
     * @return vector<string>
     */
    vector<string> getCookie();

    /**
     * ����http����, �������HTTP�������׳��쳣
     * @param sBuffer
     * @throw TC_HttpRequest_Exception
     * @return bool, sBuffer�Ƿ���������http����
     */
    bool decode(const string &sBuffer);

    /**
     * ����http����, �������HTTP�������׳��쳣(����vector<char>��ʽ)
     * @param sBuffer
     * @param iLength: ����
     * @throw TC_HttpRequest_Exception
     * @return bool, sBuffer�Ƿ���������http����
     */
    bool decode(const char *sBuffer, size_t iLength);

    /**
     * ��������(����string��ʽ)
     * @param sUrl
     *
     * @return string
     */
    string encode();

    /**
     * ��������(����vector<char>��ʽ)
     * @param buffer
     */
    void encode(vector<char> &buffer);

    /**
     * ����Get�����
     * @param sUrl, ����:http://www.qq.com/query?a=b&c=d
     * @param bCreateHost, �Ƿ��´���ͷ����Host��Ϣ
     *                     (Ĭ��, �����Host��Ϣ��, �Ͳ�����)
     *                     (ע��, ����encode��ʱ�򴴽���)
     */
    void setGetRequest(const string &sUrl, bool bNewCreateHost = false);

    /**
     * ����POST�����(����string��ʽ)
     * @param sUrl, ����:http://www.qq.com/query
     * @param sPostBody, ���requestType��GET, ��sPostBody��Ч
     * @param bCreateHost, �Ƿ��´���ͷ����Host��Ϣ
     *                     (Ĭ��, �����Host��Ϣ��, �Ͳ�����)
     *                     (ע��, ����encode��ʱ�򴴽���)
     */
    void setPostRequest(const string &sUrl, const string &sPostBody, bool bNewCreateHost = false);

    /**
     * ����POST�����(����vector<char>��ʽ)
     * @param sUrl, ����:http://www.qq.com/query
     * @param sPostBody, ���requestType��GET, ��sPostBody��Ч
     * @param bCreateHost, �Ƿ��´���ͷ����Host��Ϣ
     *                     (Ĭ��, �����Host��Ϣ��, �Ͳ�����)
     *                     (ע��, ����encode��ʱ�򴴽���)
     */
    void setPostRequest(const string &sUrl, const char *sBuffer, size_t iLength, bool bNewCreateHost = false);

    /**
     * ��ȡurl����ĵ�ַ�Ͷ˿�
     * @param sHost
     * @param iPort
     */
    void getHostPort(string &sDomain, uint32_t &iPort);

    /**
     * ����HTTP����
     * @param iTimeout, ����
     *
     * @return int 0 �ɹ�, <0ʧ��, ����ֵ�μ�TC_ClientSocket����
     */
    int doRequest(TC_HttpResponse &stHttpRep, int iTimeout = 3000);

    /**
     * �Ƿ���GET����
     *
     * @return bool
     */
    bool isGET() const { return _requestType == REQUEST_GET; }

    /**
     * �Ƿ���POST����
     *
     * @return bool
     */
    bool isPOST() const { return _requestType == REQUEST_POST; }

    /**
     * ��ȡ�����URL
     * 
     * @return const TC_URL&
     */
    const TC_URL &getURL() const { return _httpURL; }

    /**
     * ��ȡ������http����
     * 
     * @return string
     */
    string getOriginRequest() const { return _httpURL.getURL(); }

    /**
     * ��ȡhttp�����url
     * ������Host, ����http://www.qq.com/abc?a=b, ��Ϊ:/abc?a=b
     *
     * @return string
     */
    string getRequest() const { return _httpURL.getRequest(); }

    /**
     * ��ȡhttp�����url����, ��?ǰ��
     * 
     * ������Host, ����http://www.qq.com/abc?a=b#def, ��Ϊ:/abc
     *
     * @return string
     */
    string getRequestUrl() const { return _httpURL.getPath(); }

    /**
     * ��ȡhttp����url��Ĳ�������, ��?����#ǰ�沿��
     * 
     * ������Host, ����http://www.qq.com/abc?a=b#def, ��Ϊ:a=b
     *
     * @return string
     */
    string getRequestParam() const { return _httpURL.getQuery(); }

    /**
     * ��������ͷ��
     * @param szBuffer
     *
     * @return size_t
     */
    size_t parseRequestHeader(const char* szBuffer);

    /**
     * �������͵��ַ���
     * @param iRequestType
     *
     * @return string
     */
    string requestType2str(int iRequestType) const;

protected:

    /**
     * ��http�������
     * @param sUrl
     * @param iRequestType
     *
     * @return void
     */
    void encode(int iRequestType, ostream &os);

    /**
     * ����URL
     */
    void parseURL(const string& sUrl);

protected:

    /**
     * ����URL
     */
    TC_URL             _httpURL;

    /**
     * ��������
     */
    int                _requestType;
};


}
#endif

