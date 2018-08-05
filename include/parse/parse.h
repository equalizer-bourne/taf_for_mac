#ifndef JCE_DRIVER_H
#define JCE_DRIVER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <map>
#include <stack>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include "element.h"

using namespace std;

/**
 * Jce�ļ�������
 *
 */
class JceParse : public taf::TC_HandleBase
{
public:
    /**
     * ���캯��
     */
    JceParse();

    /**
     * �Ƿ�֧��taf��ͷ�ı�ʶ
     * @param bWithTaf
     */
    void setTaf(bool bWithTaf);

    /**
     * ͷ�ļ�·��
     * @param sHeader
     */
    void setHeader(const string &sHeader);

    /**
     * ��ȡͷ�ļ�·��
     */
    string getHeader();

    /**
     * ����ĳһ���ļ�
     * @param sFileName
     */
    void parse(const string &sFileName);

    /**
     * ������ʾ
     * @param msg
     */
    void error(const string &msg);

    /**
     * ���ؼ���
     * @param s
     *
     * @return int
     */
    int  checkKeyword(const string &s);

    /**
     * ��һ��
     */
    void nextLine();

    /**
     * Ŀǰ�������ļ�����
     *
     * @return string
     */
    string getCurrFileName();

    /**
     * tab
     *
     * @return string
     */
    string getTab();

    /**
     * ����tab��
     */
    void incTab() { _itab++; }

    /**
     * ����tab��
     */
    void delTab() { _itab--; }

    /**
     * �����ļ�
     * @param file
     */
    void pushFile(const string &file);

    /**
     * ���������ļ�
     */
    ContextPtr popFile();

    /**
     * ��ȡ���е�������
     *
     * @return std::vector<ContextPtr>
     */
    std::vector<ContextPtr> getContexts() { return _vcontexts; }

    /**
     * ��ȡĿǰ������
     *
     * @return ContainerPtr
     */
    ContainerPtr currentContainer();

    /**
     * push����
     * @param c
     */
    void pushContainer(const ContainerPtr &c);

    /**
     * Ŀǰ��������
     *
     * @return ContextPtr
     */
    ContextPtr currentContextPtr();

    /**
     * ��������
     *
     * @return ContainerPtr
     */
    ContainerPtr popContainer();

    /**
     * ����BuiltinԪ��
     * @param kind
     *
     * @return BuiltinPtr
     */
    BuiltinPtr createBuiltin(Builtin::Kind kind,bool isUnsigned = false);

    /**
     * ����VectorԪ��
     * @param ptr
     *
     * @return VectorPtr
     */
    VectorPtr createVector(const TypePtr &ptr);

    /**
     * ����MapԪ��
     * @param pleft
     * @param pright
     *
     * @return MapPtr
     */
    MapPtr createMap(const TypePtr &pleft, const TypePtr &pright);

    /**
     * ��ӽṹԪ��
     * @param sPtr
     */
    void addStructPtr(const StructPtr &sPtr);

    /**
     * ���ҽṹ
     * @param id
     *
     * @return StructPtr
     */
    StructPtr findStruct(const string &sid);

    /**
     * ���ö��Ԫ��
     * @param ePtr
     */
    void addEnumPtr(const EnumPtr &ePtr);

    /**
     * ���ҽṹ
     * @param id
     *
     * @return EnumPtr
     */
    EnumPtr findEnum(const string &sid);

    /**
     * ����ͻ
     * @param id
     */
    void checkConflict(const string &sid);

    /**
     * �����Զ�������
     * @param sid
     *
     * @return TypePtr
     */
    TypePtr findUserType(const string &sid);

    /**
     * �������ֿռ�
     * @param id
     *
     * @return NamespacePtr
     */
    NamespacePtr findNamespace(const string &id);

    /**
     * Ŀǰ�����ֿռ�
     *
     * @return NamespacePtr
     */
    NamespacePtr currentNamespace();

    /**
     * ���tag�ĺϷ���
     * @param i
     */
    void checkTag(int i);

    
    /**
     * ���szie�ĺϷ���
     * @param i
     */
    void checkSize(int i);

    /**
     * ���array�ĺϷ���
     * @param i
     */
    void checkArrayVaid(TypePtr &tPtr,int size);

    /**
     * ���pointer�ĺϷ���
     * @param i
     */
    void checkPointerVaid(TypePtr &tPtr);

    /**
     * ��鳣�����ͺ�ֵ�Ƿ�һ��
     * @param c
     * @param b
     */
    void checkConstValue(TypeIdPtr &tPtr, int b);

    /**
     * ��ȡ�ļ���
     * @param s
     *
     * @return string
     */
    bool getFilePath(const string &s, string &file);

    void setKeyStruct(const StructPtr& key)
    {
        _key = key;
    }

    StructPtr getKeyStruct()
    {
        return _key;
    }

    /**
     * ��ӡ�ļ���ͷע��
     */
    string printHeaderRemark();

protected:
    /**
     * ������ֿռ�
     * @param nPtr
     */
    void addNamespacePtr(const NamespacePtr &nPtr);

    /**
     * ��ʼ��
     */
    void initScanner();

    /**
     * ���
     */
    void clear();

protected:
    bool                            _bWithTaf;
    std::map<std::string, int>      _keywordMap;
    int                             _itab;
    StructPtr                       _key;
    std::stack<ContextPtr>          _contexts;
    std::stack<ContainerPtr>        _contains;
    std::vector<ContextPtr>         _vcontexts;
    std::vector<StructPtr>          _structs;
    std::vector<EnumPtr>            _enums;
    std::vector<NamespacePtr>       _namespaces;
    string                          _sHeader;
};

extern int yyparse();
extern int yylex();
extern FILE *yyin, *yyout;

typedef taf::TC_AutoPtr<JceParse> JceParsePtr;

extern JceParsePtr g_parse;

#endif


