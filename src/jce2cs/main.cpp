#include "util/tc_option.h"
#include "util/tc_file.h"
#include "jce2cs.h"

void usage()
{
    cout << "Usage : jce2cs [OPTION] jcefile" << endl;
    cout << "  jce2cs support type: bool byte short int long float double vector map" << endl;
    cout << "supported [OPTION]:" << endl;
    cout << "  --help                help,print this(����)" << endl;
    cout << "  --dir=DIRECTORY       generate java file to DIRECTORY(�����ļ���Ŀ¼DIRECTORY,Ĭ��Ϊ��ǰĿ¼)" << endl;
    cout << "  --base-package=NAME   package prefix, default 'com.qq.'(packageǰ׺��δָ����Ĭ��Ϊcom.qq.)" << endl;
    cout << "  --with-servant        also generate servant class(һ�����ɷ���˴��룬δָ����Ĭ�ϲ�����)" << endl;
    cout << endl;
    exit(0);
}

void check(vector<string> &vJce)
{
    bool b = true;

    for(size_t i  = 0; i < vJce.size(); i++)
    {
        string ext  = taf::TC_File::extractFileExt(vJce[i]);
        if(ext == "jce")
        {
            if(!b)
            {
                usage();
            }
            if(!taf::TC_File::isFileExist(vJce[i]))
            {
                cerr << "file '" << vJce[i] << "' not exists" << endl;
                exit(0);
            }
        }
        else
        {
            cerr << "only support jce file." << endl;
            exit(0);
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        usage();
    }

    taf::TC_Option option;
    option.decode(argc, argv);
    vector<string> vJce = option.getSingle();

    check(vJce);

    if(option.hasParam("help"))
    {
        usage();
    }

    Jce2Cs j2cs;

    //�Ƿ������taf��ͷ
    g_parse->setTaf(option.hasParam("with-taf"));

    //���������ļ��ĸ�Ŀ¼
    if(option.getValue("dir") != "")
    {
        j2cs.setBaseDir(option.getValue("dir"));
    }
    else
    {
        j2cs.setBaseDir(".");
    }

    //����ǰ׺
    if(option.hasParam("base-package"))
    {
        j2cs.setBasePackage(option.getValue("base-package"));
    }
    else
    {
        j2cs.setBasePackage("Com.QQ.");
    }

    //�Ƿ����ɷ������,Ĭ�ϲ�����
    if(option.hasParam("with-servant"))
    {
        j2cs.setWithServant(true);
    }
    else
    {
        j2cs.setWithServant(false);
    }

    for(size_t i = 0; i < vJce.size(); i++)
    {
        g_parse->parse(vJce[i]);
        j2cs.createFile(vJce[i]);
    }

    return 0;
}

