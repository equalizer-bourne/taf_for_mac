#ifndef __TC_MD5_H
#define __TC_MD5_H

#include <string>
#include "util/tc_common.h"

using namespace std;

namespace taf
{
/////////////////////////////////////////////////
// ˵��: md5��(�޸Ĳ�������md5��c�汾)
// Author : j@syswin.com              
// Modify: j@syswin.com              
/////////////////////////////////////////////////
/**
* �����ṩMD5���㷨
* �ṩ���������ʽ:�ַ���(32���ַ�)�������(16���ֽ�)
*/
class TC_MD5
{
    typedef unsigned char *POINTER;
    typedef unsigned short int UINT2;
    //typedef unsigned long int UINT4;
    typedef uint32_t UINT4;

    typedef struct 
    {
        /**
        * state (ABCD)
        */
        //unsigned long state[4];        
        UINT4 state[4];        

        /**
        * number of bits, modulo 2^64 (lsb first)
        */
        //unsigned long count[2];        
        UINT4 count[2];        

        /**
        * input buffer
        */
        unsigned char buffer[64];      
    }MD5_CTX;

public:

    /**
    * ���ַ�������md5����,����16�ֽڶ���������
    * @param sString : �����ַ���
    * @return string ,���,16���ֽڵĶ���������
    */
    static string md5bin(const string &sString);

    /**
    * ���ַ�������md5����,��md5�Ķ���������ת����HEX��32���ֽڵ��ַ���
    * @param sString : �����ַ���
    * @return string ,���,32���ַ�
    */
    static string md5str (const string &sString);

    /**
     * 
     * @param fileName
     * 
     * @return string
     */
    static string md5file(const string& fileName);

protected:

	static string bin2str(const void *buf, size_t len, const string &sSep);

    /**
    * MD5 init
    * @param context : ��������Ϣ
    * @return void
    */ 
    static void md5init(MD5_CTX *context);
    
    /**
    * MD5 block update operation. Continues an MD5 message-digest
    * operation, processing another message block, and updating the
    * context.
    * @param context : ��������Ϣ
    * @param input : ����
    * @param inputLen : ���볤��
    * @return void
    */    
    static void md5update (MD5_CTX *context, unsigned char *input,unsigned int inputLen);
    
    /**
    * MD5 finalization. Ends an MD5 message-digest operation,
    * writing the message digest and zeroizing the context.
    * @param digest : ժҪ
    * @param context : ��������Ϣ
    */
    static void md5final (unsigned char digest[16], MD5_CTX *context);
    
    /**
    * MD5 basic transformation. Transforms state based on block.
    * @param state : ״̬
    * @param block : ...
    */
    static void md5transform (UINT4 state[4],unsigned char block[64]);
    
    /**
    * Encodes input (UINT4) into output (unsigned char). Assumes len is a multiple of 4.
    * @param output : ���
    * @param input : ����
    * @param len : ���볤��
    */    
    static void encode (unsigned char *output,UINT4 *input,unsigned int len);
    
    /**
    * Decodes input (unsigned char) into output (UINT4). Assumes len is a multiple of 4.
    * @param output : ���
    * @param input : ����
    * @param len : ���볤��
    */    
    static void decode (UINT4 *output,unsigned char *input,unsigned int len);
    
    /**
    * replace "for loop" with standard memcpy if possible.
    * @param output : ���
    * @param input : ����
    * @param len : ���볤��
    */    
    static void md5_memcpy (POINTER output,POINTER input,unsigned int len);
    
    /**
    * replace "for loop" with standard memset if possible.
    * @param output : ���
    * @param value : ֵ
    * @param len : ���볤��
    */ 
    static void md5_memset (POINTER output,int value,unsigned int len);
    
    /**
    * ���ֵ
    */
    static unsigned char PADDING[64];
};

}
#endif
