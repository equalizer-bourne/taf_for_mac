#ifndef __CONSISTENT_HASH__
#define __CONSISTENT_HASH__

#include "util/tc_md5.h"
/////////////////////////////////////////////////
// ˵��: һ����hash�㷨��
// Author : j@syswin.com              
/////////////////////////////////////////////////
/**
 * һ����hash�㷨��
 */

using namespace std;

namespace taf
{

struct node_T
{
    unsigned int iHashCode;  //�ڵ�hashֵ
    unsigned int iIndex;     //�ڵ��±�
};

class  TC_ConsistentHash
{
    public:
        TC_ConsistentHash()
        {
        }

        /**
         * �ڵ�Ƚ�
         *
         * @param m1
         * @param m2
         *
         * @return less or not
         */
        static bool less_hash(const node_T & m1, const node_T & m2)
        {
            return m1.iHashCode < m2.iHashCode;
        }

        /**
         * ���ӽڵ�
         *
         * @param node  �ڵ�����
         * @param index �ڵ���±�ֵ
         *
         * @return �ڵ��hashֵ
         */
        unsigned addNode(const string & node, unsigned int index)
        {
            node_T stItem;
            stItem.iHashCode = hash_md5(TC_MD5::md5bin(node));
            stItem.iIndex = index;
            vHashList.push_back(stItem);

            sort(vHashList.begin(), vHashList.end(), less_hash);

            return stItem.iHashCode;
        }

        /**
         * ɾ���ڵ�
         *
         * @param node  �ڵ�����
         *
         * @return  0 : ɾ���ɹ�  -1 : û�ж�Ӧ�ڵ�
         */
        int removeNode(const string & node)
        {
            unsigned iIndex = hash_md5(TC_MD5::md5bin(node));
            vector<node_T>::iterator it;
            for(it=vHashList.begin() ; it!=vHashList.end(); it++)
            {
                if(it->iIndex == iIndex)
                {
                    vHashList.erase(it);
                    return 0;
                }
            }
            return -1;
        }

        /**
         * ��ȡĳkey��Ӧ���Ľڵ�node���±�
         *
         * @param key         key����
         * @param out  iIndex ��Ӧ���Ľڵ��±�
         *
         * @return  0:��ȡ�ɹ�   -1:û�б���ӵĽڵ�
         */
        int getIndex(const string & key, unsigned int & iIndex)
        {
            unsigned iCode = hash_md5(TC_MD5::md5bin(key));
            if(vHashList.size() == 0)
            {
                iIndex = 0;
                return -1;
            }

            int low = 0;
            int high = vHashList.size();

            if(iCode <= vHashList[0].iHashCode || iCode > vHashList[high-1].iHashCode)
            {
                iIndex = vHashList[0].iIndex;
                return 0;
            }

            while (low < high - 1)
            {
                int mid = (low + high) / 2;
                if (vHashList[mid].iHashCode > iCode)
                {
                    high = mid;
                }
                else
                {
                    low = mid;
                }
            }
            iIndex = vHashList[low+1].iIndex;
            return 0;
        }

   protected:
        /*
         * ����md5ֵ��hash���ֲ���Χ�� 0 -- 2^32-1
         *
         * @param  sMd5 : md5 ֵ
         *
         * @return hashֵ
         */
        unsigned int hash_md5(const string & sMd5)
        {
            char *p = (char *) sMd5.c_str();
            return (*(int*)(p)) ^ (*(int*)(p+4)) ^ (*(int*)(p+8)) ^ (*(int*)(p+12));
        }

        vector<node_T> vHashList;

};

}
#endif
