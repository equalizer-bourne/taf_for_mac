#ifndef __OND_ADMIN_CLIENT_PARSE_H__ 
#define __OND_ADMIN_CLIENT_PARSE_H__ 

#include <iostream>
#include <vector>

#include "AdminReg.h"
#include "Node.h"

//
// Stuff for flex and bison
//

#define YYSTYPE std::vector<std::string>
#define YY_DECL int yylex(YYSTYPE* yylvalp)
YY_DECL;
int yyparse();

//
// I must set the initial stack depth to the maximum stack depth to
// disable bison stack resizing. The bison stack resizing routines use
// simple malloc/alloc/memcpy calls, which do not work for the
// YYSTYPE, since YYSTYPE is a C++ type, with constructor, destructor,
// assignment operator, etc.
//
#define YYMAXDEPTH  10000 // 10000 should suffice. Bison default is 10000 as maximum.
#define YYINITDEPTH YYMAXDEPTH // Initial depth is set to max depth, for the reasons described above.

//
// Newer bison versions allow to disable stack resizing by defining
// yyoverflow.
//
#define yyoverflow(a, b, c, d, e, f) yyerror(a)

using namespace taf;

class Parser;
typedef taf::TC_AutoPtr<Parser> ParserPtr;

class Parser : public TC_HandleBase
{
public:

    static ParserPtr createParser(AdminRegPrx, NodePrx);

    void usage();

    /******************
     * application ����
     */
    //application �б�
    void getAllApplicationNames();


    /******************
     * node ����
     */

    //node�б�
    void getAllNodeNames();

    //ping �ض�node
    void pingNode(const vector<string>& args);

    //ֹͣ�ض�node
    void shutdownNode(const vector<string>& args);

    //ͨ��node����server
    void startServerByNode(const vector<string>& args);

    //ͨ��nodeֹͣserver
    void stopServerByNode(const vector<string>& args);

    /******************
     * server ����
     */
    //server �б�
    void getAllServerIds(const vector<string>& args);;

    //��ѯ�ض�server״̬
    void stateServer(const vector<string>& args);

    //�����ض�server
    void startServer(const vector<string>& args);

    //ֹͣ�ض�server
    void stopServer(const vector<string>& args);

    //�����ض�server
    void restartServer(const vector<string>& args);

    /**
     * ֪ͨ����
     * @param args
     */
    void notifyServer(const vector<string>& args);

    //�����ض�server
    void patchServer(const vector<string>& args);

	//�����ض��ļ�
	void patchSubborn(const vector<string>& args);

    //��������
    void getPatchPercent(const vector<string>& args);

    //�����ض�server
    void loadServer(const vector<string>& args);

    /******************
     * registry ����
     */
    //ֹͣregistry
    void shutdown();
    //�汾��Ϣ
    void showBanner();

    //��ȡģ��
    void getProfileTemplate(const vector<string>& args);


    /******************
     * �ڲ���������
     */

    void getInput(char*, int&, int);
    void nextLine();
    void continueLine();
    const char* getPrompt();
    void scanPosition(const char*);

    void invalidCommand(const char*);
    void invalidCommand(const std::string&);
    void error(const char*);
    void error(const std::string&);

    void warning(const char*);
    void warning(const std::string&);

    int parse(FILE*, bool);
    int parse(const std::string&, bool);
    vector<string> serverIdParse(const string & sServerId);

private:

    Parser(const taf::AdminRegPrx &, const taf::NodePrx &);

    std::string _commands;
    AdminRegPrx _admin;
    NodePrx _node;
    bool _continue;
    int _errors;
    int _currentLine;
    std::string _currentFile;

    vector<vector<string> > _vServer; 
};

extern Parser* parser; // The current parser for bison/flex


#endif
