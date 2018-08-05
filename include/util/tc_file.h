#ifndef __TC_FILE_H_
#define __TC_FILE_H_

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <fnmatch.h>
#include "util/tc_ex.h"
#include "util/tc_common.h"

namespace taf
{
/////////////////////////////////////////////////
// ˵��: �ļ�������
// Author : j@syswin.com              
// Modify: j@syswin.com              
/////////////////////////////////////////////////
/**
* �ļ��쳣��
*/
struct TC_File_Exception : public TC_Exception
{
    TC_File_Exception(const string &buffer) : TC_Exception(buffer){};
    TC_File_Exception(const string &buffer, int err) : TC_Exception(buffer, err){};
    ~TC_File_Exception() throw(){};
};

/**
* �����ļ���������
*/
class TC_File
{
public:

    /**
    * ��ȡ�ļ���С, ����ļ�������, �򷵻�0
    * @param    sFullFileName: �ļ�ȫ·��(����Ŀ¼���ļ���)
    * @return   ofstream::pos_type: �ļ���С
    */
    static ifstream::pos_type getFileSize(const string &sFullFileName);

    /**
     * �Ƿ��Ǿ���·��, ���Կո���'/'��ͷ
     * @param sFullFileName
     *
     * @return bool
     */
    static bool isAbsolute(const string &sFullFileName);

    /**
    * �жϸ���·�����ļ��Ƿ����
    * ע��: ����ļ��Ƿ�������,���Է��������ж�
    *       �������Է�������ָ����ļ��ж�
    * @param sFullFileName: �ļ�ȫ·��
    * @param iFileType: �ļ�����, ȱʡS_IFREG
    * @return bool: ����, ����true; ���򷵻� false.
    */
    static bool isFileExist(const string &sFullFileName, mode_t iFileType = S_IFREG);

    /**
    * �жϸ���·�����ļ��Ƿ����
    * ע��: ����ļ��Ƿ�������,���Է�������ָ����ļ��ж�
    * @param sFullFileName: �ļ�ȫ·��
    * @param iFileType: �ļ�����, ȱʡS_IFREG
    * @return bool: ����, ����true; ���򷵻� false.
    */
    static bool isFileExistEx(const string &sFullFileName, mode_t iFileType = S_IFREG);

    /**
     * ����Ŀ¼����, ��һЩ���õ�ȥ��, ����./��
     * @param path
     *
     * @return string
     */
    static string simplifyDirectory(const string& path);

    /**
    * ����Ŀ¼, ���Ŀ¼�Ѿ�����, ��Ҳ���سɹ�
    * @param sFullPath: Ŀ¼
    * @param iFlag: Ȩ��, Ĭ�� S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP
    *             | S_IXGRP | S_IROTH |  S_IXOTH
    * @return bool: �����ɹ�, ����true; ���򷵻� false.
    */
    static bool makeDir(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);

     /**
    * ѭ������Ŀ¼, ���Ŀ¼�Ѿ�����, ��Ҳ���سɹ�
    * @param sFullPath: Ŀ¼
     *@param iFlag: Ȩ��, Ĭ�� S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP
     *            | S_IXGRP | S_IROTH |  S_IXOTH
    * @return bool: �����ɹ�, ����true; ���򷵻� false.
    */

    static bool makeDirRecursive(const string &sDirectoryPath, mode_t iFlag = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH |  S_IXOTH);

    /**
     * �����ļ��Ƿ��ִ��
     * @param sFullFileName
     * @param canExecutable, true: ��ִ��, false:����֮��
     *
     * @return int,�ɹ�����0, ����ʧ��
     */
    static int setExecutable(const string &sFullFileName, bool canExecutable);

    /**
     * �Ƿ��ִ��
     * @param sFullFileName
     *
     * @return bool
     */
    static bool canExecutable(const string &sFullFileName);

    /**
     * ɾ��һ���ļ���Ŀ¼
     * @param sFullFileName
     * @param bRecursive, �����Ŀ¼�Ƿ�ݹ�ɾ��
     * @return int, 0:�ɹ� <0, ʧ��, ����ͨ��errno�鿴ʧ�ܵ�ԭ��
     */
    static int removeFile(const string &sFullFileName, bool bRecursive);

    /**
    * ��ȡ�ļ���string, �ļ������ڻ��߶�ȡ�ļ������ʱ��, ����Ϊ��
    * @param sFullFileName: �ļ�����
    * @return string : �ļ�����
    */
    static string load2str(const string &sFullFileName);

    /**
    * д�ļ�
    * @param sFullFileName: �ļ�����
    * @param sFileData: �ļ�����
    * @return ��
    */
    static void save2file(const string &sFullFileName, const string &sFileData);

    /**
     * д�ļ�
     * @param sFullFileName: �ļ���
     * @param sFileData:����ָ��
     * @param length:����
     * 
     * @return int, 0:�ɹ�,-1:ʧ��
     */
    static int save2file(const string &sFullFileName, const char *sFileData, size_t length);

    /**
     * ��ȡǰ����ִ���ļ�·��
     *
     * @return string
     */
    static string getExePath();

    /**
    * ��һ����ȫ�ļ�����ȥ��·��:����:/usr/local/temp.gif ��ȡtemp.gif
    *@param sFullFileName: �ļ�����ȫ����
    *@return string �ļ�����
    */
    static string extractFileName(const string &sFullFileName);

    /**
    * ��һ����ȫ�ļ�������ȡ�ļ���·��
    * ����1: "/usr/local/temp.gif" ��ȡ"/usr/local/"
    * ����2: "temp.gif" ��ȡ "./"
    * @param sFullFileName: �ļ�����ȫ����
    * @return string �ļ�����
    */
    static string extractFilePath(const string &sFullFileName);

    /**
    * ��ȡ�ļ���չ��
    * ����1: "/usr/local/temp.gif" ��ȡ"gif"
    * ����2: "temp.gif" ��ȡ"gif"
    *@param sFullFileName: �ļ�����
    *@return string �ļ���չ��
    */
    static string extractFileExt(const string &sFullFileName);

    /**
    * ��ȡ�ļ�����,ȥ����չ��
    * ����1: "/usr/local/temp.gif" ��ȡ"/usr/local/temp"
    * ����2: "temp.gif" ��ȡ"temp"
    * @param sFullFileName: �ļ�����
    * @return string �ļ�����,ȥ����չ��
    */
    static string excludeFileExt(const string &sFullFileName);

    /**
    * �滻�ļ���չ��, �������չ��,�������չ�� =?1:
    * "/usr/local/temp.gif" �滻 "jpg" �õ�"/usr/local/temp.jpg"
    * ����1: "/usr/local/temp" �滻 "jpg" �õ�"/usr/local/temp.jpg"
    * @param sFullFileName: �ļ�����
    * @param sExt : ��չ��
    * @return string �����չ������ļ���
    */
    static string replaceFileExt(const string &sFullFileName, const string &sExt);

    /**
    * ��һ��url�л�ȡ��ȫ�ļ���
    * ��ȡ��http://��,��һ��'/'����������ַ�
    * ����1:http://www.qq.com/tmp/temp.gif ��ȡtmp/temp.gif
    * ����2:www.qq.com/tmp/temp.gif ��ȡtmp/temp.gif
    * ����3:/tmp/temp.gif ��ȡtmp/temp.gif
    * @param sUrl: �ļ�����ȫ����
    * @return string �ļ�����
    */
    static string extractUrlFilePath(const string &sUrl);

    /**
    * �����ļ�ʱȷ���Ƿ�ѡ��
    * ���� 1 ��ѡ��, ����0��ѡ��
    */
    typedef int (*FILE_SELECT)(const dirent *);

    /**
    * ɨ��һ��Ŀ¼
    * @param sFilePath : ������·��
    * @param vtMatchFiles : ���ص��ļ���ʸ����
    * @param f : ƥ�亯��,ΪNULL��ʾ�����ļ�����ȡ
    * @param iMaxSize : ����ļ�����,iMaxSize <=0ʱ,��������ƥ���ļ�
    * @return size_t : �ļ�����
    */
    static size_t scanDir(const string &sFilePath, vector<string> &vtMatchFiles, FILE_SELECT f = NULL, int iMaxSize = 0);

    /**
     * ����Ŀ¼, ��ȡĿ¼����������ļ�����Ŀ¼
     * @param path, ·��
     * @param files, ����·�����������ļ�
     * @param bRecursive, �Ƿ�ݹ���Ŀ¼
     *
     **/
    static void listDirectory(const string &path, vector<string> &files, bool bRecursive);
    
     /**
     * �����ļ���Ŀ¼sExistFile��sNewFile
     * @param sExistFile
     * @param sNewFile
     * @param bRemove �Ƿ���ɾ��sNewFile��copy ��ֹTextfile busy���¸���ʧ��
     * 
     * @return void
     */
    static void copyFile(const string &sExistFile, const string &sNewFile,bool bRemove = false);
};

}
#endif // TC_FILE_H
