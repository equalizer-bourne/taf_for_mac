#ifndef _JCE2CPP_H
#define _JCE2CPP_H

#include "parse/parse.h"
//#include "servant/BaseF.h"

#include <cassert>
#include <string>

using namespace taf;

/**
 * ����jce����c++�ļ�
 * �����ṹ�ı�����Լ�����Proxy��Servant
 */
class Jce2Cpp
{
public:
	Jce2Cpp();

    /**
     * ����
     * @param file
     */
    void createFile(const string &file, const vector<string> &vsCoder);


    /**
    * ���������ļ���Ŀ¼
    *
    */
    void setBaseDir(const std::string & sPath) { m_sBaseDir = sPath; }

	/**
    * �����Ƿ��׳��쳣
    *
    */
	void setPDU(bool bPDU) { m_bPDU = bPDU; }

	/**
	 * �����Ƿ���Ҫ���Ĭ��ֵ
	 */
	void setCheckDefault(bool bCheck) { m_bCheckDefault = bCheck; }

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
    string readFrom(const TypeIdPtr &pPtr, bool bIsRequire = true) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string display(const TypeIdPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string displaySimple(const TypeIdPtr &pPtr, bool bSep) const;

    //����������������Դ������
protected:
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
    string tostrMap(const MapPtr &pPtr) const;

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
     * ��ȡ�������������
     * @param pPtr
     *
     * @return string
     */
    
    string toStrSuffix(const TypeIdPtr &pPtr) const;

    /**
     * ��ȡ�������������
     * @param pPtr
     *
     * @return int
     */
    int getSuffix(const TypeIdPtr &pPtr) const;

    /**
     * ����unsignedת��Ϊsigned�ķ�������
     * @param pPtr
     *
     * @return string
     */
    //string unsigned2signedStr(const BuiltinPtr &pPtr) const;

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

    //������h��cpp�ļ��ľ�������
protected:
    /**
     * �ṹ��md5
     * @param pPtr
     *
     * @return string
     */
    string MD5(const StructPtr &pPtr) const;

    /**
     * ���ɽṹ��ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateH(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * ���ɽṹ��cppԴ�룬���ڻ���������
     * @param pPtr
     *
     * @return string
     */
	string generateCpp(const StructPtr &pPtr, const string& namespaceId) const;

    /**
     * ����������ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ContainerPtr &pPtr) const;

    /**
     * ����������cppԴ��
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const ContainerPtr &pPtr) const;

    /**
     * ���ɲ���������ͷ�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ParamDeclPtr &pPtr) const;

    /**
     *
     * @param pPtr
     *
     * @return string
     */
    string generateOutH(const ParamDeclPtr &pPtr) const;

    /**
     *
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateAsyncResponseCpp(const OperationPtr &pPtr, const string &cn) const;

    /**
     * ���ɲ���������cpp�ļ�����
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const ParamDeclPtr &pPtr) const;

    /**
     * ������������proxy��cpp�ļ�����
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateCpp(const OperationPtr &pPtr, const string &cn) const;

    /**
     * ������������proxy��cpp�ļ�����
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateCppAsync(const OperationPtr &pPtr, const string &cn) const;

    /**
     * ���ɲ���cpp�ļ��������÷ַ���Դ��
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateDispatchCpp(const OperationPtr &pPtr, const string &cn) const;
    /**
     * ���ɲ���cpp�ļ��������÷ַ���Դ��
     * @param pPtr
     * @param cn
     *
     * @return string
     */
    string generateDispatchAsyncCpp(const OperationPtr &pPtr, const string &cn) const;
    /**
     * ���ɲ�����servant��ͷ�ļ�Դ��
     * @param pPtr
     * @param bVirtual
     *
     * @return string
     */
    string generateHAsync(const OperationPtr &pPtr) const;
    /**
     * ���ɲ�����servant��ͷ�ļ�Դ��
     * @param pPtr
     * @param bVirtual
     *
     * @return string
     */
    string generateHAsync(const OperationPtr &pPtr, const string& interfaceId) const;
    /**
     * ���ɲ�����servant��ͷ�ļ�Դ��
     * @param pPtr
     * @param bVirtual
     *
     * @return string
     */
    string generateH(const OperationPtr &pPtr, bool bVirtual, const string& interfaceId) const;

    /**
     * ���ɽӿڵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const InterfacePtr &pPtr) const;

    /**
     * ���Ľӿڵ�cpp�ļ���Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const InterfacePtr &pPtr, const NamespacePtr &nPtr) const;

    /**
     * ����ö�ٵ�ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const EnumPtr &pPtr) const;

    /**
     * ���ɳ���ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const ConstPtr &pPtr) const;

    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateH(const NamespacePtr &pPtr) const;

    /**
     * �������ֿռ�cpp�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    string generateCpp(const NamespacePtr &pPtr) const;

    /**
     * �������ֿռ��ͷ�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateH(const ContextPtr &pPtr) const;

    /**
     * �������ֿռ�cpp�ļ�Դ��
     * @param pPtr
     *
     * @return string
     */
    void generateCpp(const ContextPtr &pPtr) const;

    /**
     *  
     * �������ֲ��ҽṹ 
     * @param pPtr 
     * @param id 
     * 
     * @return StructPtr 
     */
    StructPtr findStruct(const ContextPtr &pPtr,const string &id);

    /**
     * 
     * ���ɽӿڱ�������
     * @param pPtr 
     * @param interface 
     */
    void generateCoder(const ContextPtr &pPtr,const string &interface) const;

    string generateCoder(const NamespacePtr &pPtr,const string & sInterface) const;

    string generateCoder(const InterfacePtr &pPtr) const;

    string generateCoder(const OperationPtr &pPtr) const;

private:
    std::string m_sBaseDir;

	bool m_bPDU;
	bool m_bCheckDefault;
};

#endif


