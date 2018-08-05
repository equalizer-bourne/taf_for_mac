#ifndef __TC_CGI_H
#define __TC_CGI_H

#include <sstream>
#include <istream>
#include <map>
#include <vector>
#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: CGI������
// Author : j@syswin.com              
/////////////////////////////////////////////////
class TC_Cgi;
class TC_Cgi_Upload;
class TC_HttpRequest;

/**
* �����ļ��쳣��
*/
struct TC_Cgi_Exception : public TC_Exception
{
	TC_Cgi_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Cgi_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TC_Cgi_Exception() throw(){};
};

/**
* ȫ�ֵ���Ԫ����
* ����ú���,�������TC_Common::tostr�� vector<TC_Cgi_Upload>�����������
*/
ostream &operator<<(ostream &os, const TC_Cgi_Upload &tcCgiUpload);

/**
* cgi�ϴ��ļ�����
* ͨ�������ȡcgi�ϴ����ļ���Ϣ
*/
class TC_Cgi_Upload
{
public:
    friend ostream &operator<<(ostream &os, const TC_Cgi_Upload &tcCgiUpload);

    /**
    * contructor
    */
    TC_Cgi_Upload()
    :_sFileName("")
    , _sRealFileName("")
    , _sServerFileName("")
    , _iSize(0)
    , _bOverSize(false)
    {
    }

    /**
    * �������캯�� copy contructor
    * @param tcCgiUpload : �������캯��
    */

    TC_Cgi_Upload(const TC_Cgi_Upload &tcCgiUpload);

    /**
    * ��ֵ���캯��
    */
    TC_Cgi_Upload & operator=(const TC_Cgi_Upload &tcCgiUpload);

    /**
    * �����ϴ�����Ϣ
    * return string : �ϴ��ļ�����Ϣ
    */
    string tostr() const;

    /**
    * ���ؿͻ���IE INPUT�ϴ��ؼ�������
    * return string : INPUT�ϴ��ؼ�����
    */
    string getFormFileName() const
    {
        return _sFileName;
    }

    /**
    * ����INPUT�ؼ��û����������,���ͻ�����ʵ���ļ�����
    * return string : �ͻ�����ʵ���ļ�����
    */
    string retRealFileName() const
    {
        return _sRealFileName;
    }

    /**
    * �ϴ�����������,���������ļ�����
    * return string : ���������ļ�����
    */
    string getSeverFileName() const
    {
        return _sServerFileName;
    }

    /**
    * �ϴ����ļ���С
    * return size_t : �ϴ����ļ���С
    */
    size_t getFileSize() const
    {
        return _iSize;
    }

    /**
    * �ϴ����ļ��Ƿ񳬹���С
    * return bool : �Ƿ񳬹���С
    */
    bool isOverSize() const
    {
        return _bOverSize;
    }

protected:

    /**
    * �ϴ��ļ�,�����file�ؼ�����
    */
    string  _sFileName;

    /**
    * �ϴ��ļ���ʵ����,�ͻ��˵��ļ�����
    */
    string  _sRealFileName;

    /**
    * �ϴ��ļ�������������
    */
    string  _sServerFileName;

    /**
    * �ϴ��ļ���С,�ֽ���
     */
    size_t  _iSize;

    /**
    * �ϴ��ļ��Ƿ񳬹���С
    */
    bool    _bOverSize;

    friend class TC_Cgi;
};

/**
* cgi���������
* 1 ֧�ֲ�������
* 2 ֧��cookies����
* 3 ֧���ļ��ϴ�,�����ϴ��ļ���������,�ļ�������С
* 4 �ϴ��ļ�ʱ, ��Ҫ����ļ��Ƿ񳬹�����С
* 5 �ϴ��ļ�ʱ, ��Ҫ����ϴ��ļ������Ƿ�����
*
* ˵��:����ļ�ͬʱ�ϴ�ʱ,�������file�ؼ�����ȡ��ͬname,�����޷���ȷ���ϴ��ļ�
*
*
* ע��:
* ����parseCgi������׼����
* ������ļ��ϴ���Ҫ����setUpload, ������Ҫ��parseCgi֮ǰ����
*
*/
class TC_Cgi
{
public:

    /**
    * TC_Cgi contructor
    */
    TC_Cgi();

    /**
    * decontructor
    */
    virtual ~TC_Cgi();

    /**
    * ���廷������
    */
    enum
    {
        ENM_SERVER_SOFTWARE,
        ENM_SERVER_NAME,
        ENM_GATEWAY_INTERFACE,
        ENM_SERVER_PROTOCOL,
        ENM_SERVER_PORT,
        ENM_REQUEST_METHOD,
        ENM_PATH_INFO,
        ENM_PATH_TRANSLATED,
        ENM_SCRIPT_NAME,
        ENM_HTTP_COOKIE,
        ENM_QUERY_STRING,
        ENM_REMOTE_HOST,
        ENM_REMOTE_ADDR,
        ENM_AUTH_TYPE,
        ENM_REMOTE_USER,
        ENM_REMOTE_IDENT,
        ENM_CONTENT_TYPE,
        ENM_CONTENT_LENGTH,
        ENM_HTTP_USER_AGENT
    };

    /**
     * �����ϴ��ļ�
     * @param sUploadFilePrefix, �ļ�ǰ׺(����·��), ������ļ��ϴ�,���ļ��������Ը�ǰ׺Ϊ���Ƶ�·����
     *                           ����ж���ļ��ϴ�,���ļ������Դ��ں����"_���"
     * @param iMaxUploadFiles, ����ϴ��ļ�����,<0:û������
     * @param iUploadMaxSize, ÿ���ļ��ϴ�������С(�ֽ�)
     */
    void setUpload(const string &sUploadFilePrefix, int iMaxUploadFiles = 5, size_t iUploadMaxSize = 1024*1024*10, size_t iMaxContentLength = 1024*1024*10);

    /**
     * ����cgi
     * @param is: �ӱ�׼�������
     */
    void parseCgi();

    /**
     * ֱ�Ӵ�http�������
     * @param request
     */
    void parseCgi(TC_HttpRequest &request);

    /**
    * ��ȡcgi��url����multimap
    * @return multimap<string, string>
    */
    const multimap<string, string> &getParamMap() const;

    /**
     * ��ȡcgi��������map
     *
     * @return map<string,string>
     */
    map<string, string> getEnvMap() const { return _env; }

    /**
    * ��ȡcgi�Ĳ���map, ��multimapת����map����, ����һ���������ƶ�Ӧ�������ֵ�����, ֻȡ����һ��ֵ
    * @return map<string, string>
    */
    map<string, string> getParamMapEx() const;

    /**
    * ��ȡcookies�Ĳ���map
    * @return map<string, string>
    */
    const map<string, string> &getCookiesMap() const;

    /**
    * ��ȡcgi��ĳ������
    * @param sName : ��������
    * @return string
    */
    string &operator[](const string &sName);

    /**
    * ��ȡcgi��ĳ������
    * @param sName : ��������
    * @return string
    */
    string getValue(const string& sName) const;

    /**
    * ��ȡĳһ���ƵĲ����Ķ��ֵ,���ڽ���checkbox����ؼ���ֵ(һ��������,�������ֵ)
    * @param sName : ��������
    * @param vtValue : �����ƵĲ���ֵ��ɵ�vector
    * @return vector<string>, �����ƵĲ���ֵ��ɵ�vector
    */
    const vector<string> &getMultiValue(const string& sName, vector<string> &vtValue) const;

    /**
    * ��ȡcookieֵ
    * @param sName : cookie����
    * @return string
    */
    string getCookie(const string& sName) const;

    /**
    * ����cookieֵ
    * @param sName : cookie����
    * @param sValue : cookieֵ
    * @param sExpires : ��������
    * @param sPath : cookie��Ч·��
    * @param sDomain : cookie��Ч��
    * @param bSecure : �Ƿ�ȫ(sslʱ��Ч)
    * @return string, �����ַ���
    */
    string setCookie(const string &sName, const string &sValue, const string &sExpires="", const string &sPath="/", const string &sDomain = "", bool bSecure = false);

    /**
    * ���������Ƿ�Ϊ��
    * @return bool
    */
    bool isParamEmpty() const;

    /**
    * �����Ƿ����
    * @param sName, ��������
    * @return bool
    */
    bool isParamExist(const string& sName) const;

    /**
    * �ϴ��ļ��Ƿ񳬹���С,����ļ��ϴ�ʱ,ֻҪ��һ���ļ�������С,�򳬹�
    * @return bool
    */
    bool  isUploadOverSize() const;

    /**
    * �ϴ��ļ��Ƿ񳬹���С,����ļ��ϴ�ʱ,ֻҪ��һ���ļ�������С,�򳬹�
    * @param vtUploads : ���س�����С���ļ�����(�����file�ؼ�������)
    * return bool
    */
    bool  isUploadOverSize(vector<TC_Cgi_Upload> &vtUploads) const;

    /**
     * �Ƿ񳬹��ϴ��ļ�����
     *
     * @return bool
     */
    bool isOverUploadFiles() const { return _bOverUploadFiles; }

    /**
    * �ϴ��ļ�����
    * @return size_t
    */
    size_t getUploadFilesCount() const;

    /**
    * ��ȡ�ϴ��ļ���map
    * @return map<string, TC_Cgi_Upload>
    */
    const map<string, TC_Cgi_Upload> &getUploadFilesMap() const;

    /**
    * ��ȡ��������
    * @param iEnv : ö�ٱ���
    * @return string
    */
    string getCgiEnv(int iEnv);

    /**
    * ��ȡ��������
    * @param sEnv : ������������
    * @return string
    */
    string getCgiEnv(const string& sEnv);

    /**
     * ���û�������
     * @param sName
     * @param sValue
     */
    void setCgiEnv(const string &sName, const string &sValue);

    /**
    * ����htmlͷ:content-type
    * @param sHeader : ��������
    * @return ��
    */
    static string htmlHeader(const string &sHeader = "text/html");

    /**
    * http�����url����, %����Ļ����ַ�
    * @param sUrl : http����url
    * @return string : ���ؽ������ַ���
    */
    static string decodeURL(const string &sUrl);

    /**
     * ��url���б���, �����ֺ���ĸ��%XX����
     * @param sUrl
     *
     * @return string
     */
    static string encodeURL(const string &sUrl);

    /**
     * ��Դ�ַ�������HTML����(<>"&)
     * @param src
     * @param blankEncode, �Ƿ�Կո�Ҳ����(�ո�, \t, \r\n, \n)
     *
     * @return string
     */
    static string encodeHTML(const string &src, bool blankEncode = false);

    /**
     * ��Դ�ַ�������XML����(<>"&')
     * @param src
     *
     * @return string
     */
    static string encodeXML(const string &src);

protected:

    /**
    * ����,���ǲ�����,��֤����������ᱻʹ��
    */
    TC_Cgi &operator=(const TC_Cgi &tcCgi);

    /**
    * GET method
    * @param sBuffer : GET��QueryString
    * return ��
    */
    void getGET(string &sBuffer);

    /**
    * POST method
    * @param sBuffer : POST��QueryString
    * return ��
    */
    void getPOST(string &sBuffer);

    /**
    * �����ļ��ϴ�
    * @param mmpParams : [out]�������multimap
    * return ��
    */
    void parseUpload(multimap<string, string> &mmpParams);

    /**
     * ����form����
     */
    void parseFormData(multimap<string, string> &mmpParams, const string &sBoundary);

    /**
     * ���Կ���
     */
    void ignoreLine();

    /**
     * д�ļ�
     * @param sFileName
     * @param sBuffer
     */
    bool writeFile(FILE*fp, const string &sFileName, const string &sBuffer, size_t &iTotalWrite);

    /**
    * ���ϴ�ģʽ�½���
    * @param mmpParams : [out]�������multimap
    * @param sBuffer : [in]����QueryString
    * return ��
    */
    void parseNormal(multimap<string, string> &mmpParams, const string& sBuffer);

    /**
    * ����cookies
    * @param mpCooies : [out]���cookiesmap
    * @param sBuffer : [in]����Cookies�ַ���
    * return ��
    */
    void parseCookies(map<string, string> &mpCooies, const string& sBuffer);

    /**
    * ���ƽ���cgi input�Ļ�������
    * @param mmpParams : [out]������� multimap
    * @param mpCooies : [out]���cookies
    * return ��
    */
    void readCgiInput(multimap<string, string> &mmpParams, map<string, string> &mpCooies);

protected:

    /**
     * buffer
     */
    string                      _buffer;

    /**
     * ��
     */
    istringstream               _iss;

    /**
     * ����
     */
    istream                     *_is;

    /**
     * ��������
     */
    map<string, string>         _env;

    /**
    * cgi����
    */
    multimap<string, string>    _mmpParams;

    /**
    * cookies
    */
    map<string, string>         _mpCookies;

    /**
    * �ϴ��ļ�����ǰ׺
    */
    string                      _sUploadFilePrefix;

    /**
    * �ϴ��ļ���������,<0:������
    */
    int                         _iMaxUploadFiles;

    /**
    * �ϴ��ļ�������С
    */
    size_t                      _iUploadMaxSize;

    /**
     * �Ƿ񳬹��ϴ��ļ�����
     */
    bool                        _bOverUploadFiles;

    /**
     * ����content-length
     */
    size_t                      _iMaxContentLength;

    /**
    * �Ƿ񳬹���С,��һ���ļ������򳬹�
    */
    bool                        _bUploadFileOverSize;

    /**
    * �ϴ��ļ������Ϣ�����ڸ�map��
    */
    map<string, TC_Cgi_Upload>  _mpUpload;
};

}

#endif
