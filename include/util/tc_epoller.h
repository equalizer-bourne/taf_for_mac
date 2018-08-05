#ifndef	__TC_EPOLLER_H_
#define __TC_EPOLLER_H_

#include <sys/epoll.h>
#include <cassert>

namespace taf
{
/////////////////////////////////////////////////
// ˵��: epoll������װ��
// Author : j@syswin.com              
/////////////////////////////////////////////////
// 
/**
 * epoller������
 * �Ѿ�Ĭ�ϲ�����EPOLLET��ʽ������
 */
class TC_Epoller
{
public:

	/**
     * ���캯��
     * @param bEt: Ĭ����ETģʽ
	 */
	TC_Epoller(bool bEt = true);

	/**
     * ��������
	 */
	~TC_Epoller();

	/**
     * ����epoll���
     * @param max_connections : epoll������Ҫ֧�ֵ����������
	 */
	void create(int max_connections);

	/**
     * ��Ӽ������
     * @param fd, ���
     * @param data, ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event, ��Ҫ�������¼�EPOLLIN|EPOLLOUT
     *              
	 */
	void add(int fd, long long data, __uint32_t event);

	/**
     * �޸ľ���¼�
     * @param fd, ���
     * @param data, ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event, ��Ҫ�������¼�EPOLLIN|EPOLLOUT
	 */
	void mod(int fd, long long data, __uint32_t event);

	/**
     * ɾ������¼�
     * @param fd, ���
     * @param data, ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event, ��Ҫ�������¼�EPOLLIN|EPOLLOUT
	 */
	void del(int fd, long long data, __uint32_t event);

	/**
     * �ȴ�ʱ��
     * @param millsecond, ����
	 *
     * @return int, ���ض��ٸ�������¼�����
	 */
	int wait(int millsecond);

	/**
     * ��ȡ���������¼�
	 *
	 * @return struct epoll_event&
	 */
	struct epoll_event& get(int i) { assert(_pevs != 0); return _pevs[i]; }

protected:

	/**
     * ����epoll
     * @param fd, ���
     * @param data, ����������, ���Ժ�����epoll_event�л�ȡ��
     * @param event, ��Ҫ�������¼�
     * @param op, ����
	 */
	void ctrl(int fd, long long data, __uint32_t events, int op);

protected:

    /**
     * 	epoll
     */
    int _iEpollfd;

	/**
     * ���������
	 */
	int	_max_connections;

	/**
     * �¼���
	 */
	struct epoll_event *_pevs;

    /**
     * �Ƿ���ETģʽ
     */
    bool _et;
};

}
#endif

