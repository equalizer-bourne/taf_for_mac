#ifndef __TC_CONFIG_H_
#define __TC_CONFIG_H_

#include <map>
#include <list>
#include <stack>
#include <vector>
#include "util/tc_ex.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �����ļ���ȡ��
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
* ��ָ���
*/
const char TC_CONFIG_DOMAIN_SEP = '/';

/**
* ������ʼ��
*/
const char TC_CONFIG_PARAM_BEGIN = '<';

/**
* ����������
*/
const char TC_CONFIG_PARAM_END = '>';

/**
* �����ļ��쳣��
*/
struct TC_Config_Exception : public TC_Exception
{
	TC_Config_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_Config_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TC_Config_Exception() throw(){};
};

/**
 * ����û���ļ�����
 */
struct TC_ConfigNoParam_Exception : public TC_Exception
{
	TC_ConfigNoParam_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_ConfigNoParam_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
	~TC_ConfigNoParam_Exception() throw(){};
};


/**
* ���������ļ��е���
*/
class TC_ConfigDomain
{
public:
    friend class TC_Config;

    /**
    * ���캯��
    * @param sLine : ��
    */
    TC_ConfigDomain(const string &sLine);

    /**
    * ��������
    */
    ~TC_ConfigDomain();

    /**
     * ��������
     * @param tcd
     */
    TC_ConfigDomain(const TC_ConfigDomain &tcd);

    /**
     * ��ֵ
     * @param tcd
     * 
     * @return TC_ConfigDomain&
     */
    TC_ConfigDomain& operator=(const TC_ConfigDomain &tcd);

    struct DomainPath
    {
        vector<string>  _domains;
        string          _param;
    };

    /**
     * ����domain
     * @param domain
     *
     * @return bool
     */
    static DomainPath parseDomainName(const string& path, bool bWithParam);

    /**
    * ������������
    * @param pTcConfigDomain: pointer to sub domain
    * @throws TC_Config_Exception
    * @return TC_ConfigDomain*
    */
    TC_ConfigDomain* addSubDomain(const string& name);

    /**
    * Search Sub Domain �ݹ�����
    * @param sDomainName : /domain1/domain2
    * @return pointer to sub domain
    */
    TC_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd);
    const TC_ConfigDomain *getSubTcConfigDomain(vector<string>::const_iterator itBegin, vector<string>::const_iterator itEnd) const;

    /**
    * Get Param Value ��ȡ����/ֵ��
    * @param sName : ����
    * @return ֵ
    */
    string getParamValue(const string &sName) const;

    /**
    * Get Param Map ��ȡmap
    * @return map
    */
    const map<string, string>& getParamMap() const { return _param; }

    /**
    * Get Domain Map ��ȡmap
    * @return map
    */
    const map<string, TC_ConfigDomain*>& getDomainMap() const { return _subdomain; }

    /**
    * Set Param Value ���ò���/ֵ��
    * @param sLine : ��
    * @return ��
    */
    void setParamValue(const string &sLine);

    /**
     * �������
     * @param m
     */
    void insertParamValue(const map<string, string> &m);

    /**
    * Destroy �ͷ�
    * @return ��
    */
    void destroy();

    /**
    * Get Name ��ȡ������
    * @return ������
    */
    string getName() const;

    /**
     * ����������
     * @param name
     */
    void setName(const string& name);

    /**
     * �����ļ��е�˳���ȡkey
     * 
     * @return vector<string>
     */
    vector<string> getKey() const;

    /**
     * �����ļ��е�˳���ȡ��Domain
     * 
     * @return vector<string>
     */
    vector<string> getSubDomain() const;

    /**
    * tostr ת���������ļ����ַ�����ʽ
    * @param i : tab�Ĳ���
    * @return ������
    */
    string tostr(int i) const;

    /**
     * ��¡
     * 
     * @return TC_ConfigDomain*
     */
    TC_ConfigDomain* clone() const 
    { 
        return new TC_ConfigDomain(*this); 
    }

protected:
    /**
     * ת��
     * @param name
     *
     * @return string
     */
    static string parse(const string& s);

    /**
     * ����ת��
     * @param s
     *
     * @return string
     */
    static string reverse_parse(const string &s);

    /**
    * Set Param Value ���ò���/ֵ��
    * @param sName : ����
    * @param sValue : ֵ
    * @return ��
    */
    void setParamValue(const string &sName, const string &sValue);

protected:

    /**
    * ������
    */
    string                  _name;

    /**
    * name value��
    */
    map<string, string>     _param;

    /**
     * key�Ĳ���˳��
     */
    vector<string>          _key;

    /**
    * ����
    */
    map<string, TC_ConfigDomain*>	    _subdomain;

    /**
     * ��Ĳ���˳��
     */
    vector<string>          _domain;
};

/**
* ���������ļ�
* <Main>
*   <Domain>
*       Name = Value
*   </Domain>
*   <Domain1>
*       Name = Value
*   </Domain1>
* </Main>
*
* ��ȡ����:conf["/Main/Domain<Name>"]
* ��ȡ��Map: getDomainMap("/Main/Domain", m); m�õ�NameValue��
* ��ȡ��Vector: getDomainVector("/Main", v); v�õ�Domain�б�
* ������������������ֵ��
*/
class TC_Config
{
public:

    /**
    * contructor
    */
    TC_Config();

    /**
     * ��������
     * @param tc
     */
    TC_Config(const TC_Config &tc);

    /**
     * ��ֵ
     * @param tcd
     * 
     * @return TC_Config&
     */
    TC_Config& operator=(const TC_Config &tc);

    /**
    * Parse File �����ļ�
    * @param sFileName : �ļ�����
    * @throws TC_Config_Exception
    * @return ��
    */
    void parseFile(const string& sFileName);

    /**
    * Parse �ַ���
    * @throws TC_Config_Exception
    * @return void
    */
    void parseString(const string& buffer);

    /**
    * Get Value ��ȡֵ, ���û�����׳��쳣
    * @param sName : ��������,  ����: /Main/Domain<Param>
    * @throws TC_Config_Exception
    * @return string
    */
    string operator[](const string &sName);

    /**
     * Get Value ��ȡֵ, ע�����û�в��׳��쳣,���ؿ��ַ���
     * @param sName
     *
     * @return string
     */
    string get(const string &sName, const string &sDefault="") const;

    /**
    * GetDomainParamMap ��ȡ������Ĳ���ֵ��
    * @param path : ������, ���ʶ, ����: /Main/Domain
    * @param m, ���ڵ�map�б�
    * @param map<string, string>
    * @return bool, ����������Ĳ���ֵ��
    */
    bool getDomainMap(const string &path, map<string, string> &m) const;

    /**
     * �����򷵻ؿ�map
     * @param path: ������, ���ʶ, ����: /Main/Domain
     * 
     * @return map<string,string>
     */
    map<string, string> getDomainMap(const string &path) const;

    /**
     * ��ȡ�����������key, �����ļ�����˳�򷵻�
     * @param path: ������, ���ʶ, ����: /Main/Domain
     * 
     * @return vector<string>
     */
    vector<string> getDomainKey(const string &path) const;

    /**
    * getDomainMap ��ȡ�����������
    * @param path : ������, ���ʶ, ����: /Main/Domain
    * @param v : �������������,
    * @param vector<string>, �����������������
    * @return bool
    */
    bool getDomainVector(const string &path, vector<string> &v) const;

    /**
     * �������򷵻ؿ�vector
     * �������ļ��е�˳��
     * @param path: ������, ���ʶ, ����: /Main/Domain
     * 
     * @return vector<string>
     */
    vector<string> getDomainVector(const string &path) const;

    /**
     * �Ƿ������
     * @param path: ������, ���ʶ, ����: /Main/Domain
     *
     * @return bool
     */
    bool hasDomainVector(const string &path) const;

    /**
     * ��ǰ������������, ����Ѿ�����sAddDomain��, ����Ϊ�ɹ�
     * @param sCurDomain, ���ʶ��, ����:/Main/Domain
     * @param sAddDomain, ��Ҫ���ӵ�������: ����: Domain1
     * @param bCreate, sCurDomain�򲻴��ڵ������, �Ƿ��Զ�����
     * @return int, 0:�ɹ�
     *              -1:sCurDomain������
     */
    int insertDomain(const string &sCurDomain, const string &sAddDomain, bool bCreate);

    /**
     * ��ǰ���������Ӳ���, ����Ѿ�����ز���, �����(���滻)
     * @param sCurDomain, ���ʶ��, ����:/Main/Domain
     * @param m,����ֵ��
     * @param bCreate, sCurDomain�򲻴��ڵ������, �Ƿ��Զ�����
     * @return int, 0: �ɹ�
     *              -1:sCurDomain������
     */
    int insertDomainParam(const string &sCurDomain, const map<string, string> &m, bool bCreate);

    /**
     * �ϲ������ļ�����ǰ�����ļ�
     * @param cf
     * @param bUpdate, true: ��ͻ����±�����, false: ��ͻ�����
     */
    void joinConfig(const TC_Config &cf, bool bUpdate);

    /**
    * ת���������ļ����ַ�����ʽ
    * @return �����ַ���
    */
	string tostr() const;

protected:
    /**
    * Parse �ַ���
    * @throws TC_Config_Exception
    * @return void
    */
    void parse(istream &is);

    /**
    * create New Domain ��������
    * @param sName : ������
    * @throws TC_Config_Exception
    * @return pointer to new domain
    */
    TC_ConfigDomain *newTcConfigDomain(const string& sName);

    /**
    * Search Domain ������
    * @param sDomainName ��������,֧����������
    * @return value
    */
	TC_ConfigDomain *searchTcConfigDomain(const vector<string>& domains);
	const TC_ConfigDomain *searchTcConfigDomain(const vector<string>& domains) const;

protected:

    /**
    * ��domain
    */
	TC_ConfigDomain _root;
};

}
#endif //_TC_CONFIG_H_
