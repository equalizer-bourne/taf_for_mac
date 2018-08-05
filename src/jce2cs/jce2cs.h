#ifndef _JCE2JAVA_H
#define _JCE2JAVA_H

#include "parse/parse.h"

#include <cassert>
#include <string>

/**
 * ����jce����java�ļ�
 * �����ṹ�ı�����Լ�����Proxy��Servant
 */
class Jce2Cs
{
public:

    /**
     * ���ô������ɵĸ�Ŀ¼
     * @param dir
     */
    void setBaseDir(const string &dir);

    /**
     * ���ð�ǰ׺
     * @param prefix
     */
    void setBasePackage(const string &prefix);

    /**
     * �����Ƿ���Ҫ����˴���
     */
    void setWithServant(bool bWithServant) { _bWithServant = bWithServant;}

    /**
     * ����
     * @param file
     * @param isFramework �Ƿ��ǿ��
     */
    void createFile(const string &file);

protected:
    /**
     * ���������ռ��ȡ�ļ�·��
     * @param ns �����ռ�
     *
     * @return string
     */
    string getFilePath(const string &ns) const;

    string _packagePrefix;
    string _baseDir;
    bool   _bWithServant;

    //�����Ǳ�����Դ������
protected:

    /**
     * ����ĳ���͵Ľ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string writeTo(const TypeIdPtr &pPtr) const;

    /**
     * ����ĳ���͵ı���Դ��
     * @param pPtr
     *
     * @return string
     */
    string readFrom(const TypeIdPtr &pPtr) const;

    /**
     * 
     * @param pPtr
     * 
     * @return string
     */
    string display(const TypeIdPtr &pPtr) const;

    //����������������Դ������
protected:

    /*
     * ����ĳ���͵ĳ�ʼ���ַ���
     * @param pPtr
     *
     * @return string
     */
    string toTypeInit(const TypePtr &pPtr) const;

    /**
     * ����ĳ���͵Ķ�Ӧ������ַ�������Դ��
     * @param pPtr
     *
     * @return string
     */
    string toObjStr(const TypePtr &pPtr) const;

    /**
     * �ж��Ƿ��Ƕ�������
     */
    bool isObjType(const TypePtr &pPtr) const;

    /**
     * ����ĳ���͵��ַ�������Դ��
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr &pPtr) const;

    /**
     * �����ڽ����͵��ַ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr &pPtr) const;
    /**
     * ����vector���ַ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr &pPtr) const;

    /**
     * ����map���ַ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr &pPtr, bool bNew = false) const;

    /**
     * ����ĳ�ֽṹ�ķ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;

    /**
     * ����ĳ��ö�ٵķ�������
     * @param pPtr
     *
     * @return string
     */
    string tostrEnum(const EnumPtr &pPtr) const;

    /**
     * �������ͱ����Ľ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string decode(const TypeIdPtr &pPtr) const;

    /**
     * �������ͱ����ı���Դ��
     * @param pPtr
     *
     * @return string
     */
    string encode(const TypeIdPtr &pPtr) const;

    //������h��java�ļ��ľ�������
protected:
    /**
     * �ṹ��md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * ���ɽṹ��Holder�࣬�������ô���
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateHolder(const StructPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Ĭ��Ԫ������ʶ��map/list����
     * @param pPtr
     * @param sElemName Ԫ������
     *
     * @return string
     */
    string generateDefautElem(const TypePtr &pPtr, const string & sElemName) const;

    /**
     * ���ɽṹ��java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateCs(const StructPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����������javaԴ��
     * @param pPtr
     *
     * @return string
     */
    string generateCs(const ContainerPtr &pPtr) const;

    /**
     * ���ɲ���������java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateCs(const ParamDeclPtr &pPtr) const;

    /**
     * ���ɲ���holder��java�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateHolder(const ParamDeclPtr &pPtr) const;

    /**
     * ������������proxy��java�ļ�����
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateCs(const OperationPtr &pPtr, const string &cn) const;

    /**
     * ���ɲ���java�ļ��������÷ַ���Դ��
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateDispatchCs(const OperationPtr &pPtr, const string &cn) const;


    /**
     * ���ɽӿڵ�java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateCs(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Proxy�ӿڵ�java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrx(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Proxy�������java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxHelper(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����Proxy�ص����java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generatePrxCallback(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ���ɷ��������java�ļ���Դ��
     * @param pPtr
     * @param nPtr
     *
     * @return string
     */
    string generateServant(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����ö�ٵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateCs(const EnumPtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ���ɳ���javaԴ��
     * @param pPtr
     * 
     * @return string
     */
    void generateCs(const ConstPtr &pPtr, const NamespacePtr &nPtr) const;

    void generateCs(const vector<EnumPtr> &es,const vector<ConstPtr> &cs,const NamespacePtr &nPtr) const;
    /**
     * �������ֿռ�java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateCs(const NamespacePtr &pPtr) const;

    /**
     * ����ÿ��jce�ļ���java�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateCs(const ContextPtr &pPtr) const;

};

#endif


