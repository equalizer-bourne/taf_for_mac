#if __APPLE__

#ifndef	__TC_SELECTER_H_
#define __TC_SELECTER_H_

#include<sys/select.h>
#include <cassert>
#include <map>

namespace taf
{
    
/////////////////////////////////////////////////
// 说明: epoll操作封装类
// Author : j@syswin.com              
/////////////////////////////////////////////////
// 

    typedef union epoll_data{
        void *ptr;
        int fd;
        __uint32_t u32;
        __uint64_t u64;
    }epoll_data_t;
    
    struct epoll_event {
        __uint32_t events;
        epoll_data_t data;
    };
    
#define EPOLL_CTL_ADD 0x01  //select增加事件
#define EPOLL_CTL_MOD 0x02  //修改select事件
#define EPOLL_CTL_DEL 0x04  //删除事件
    
#define EPOLLERR 0x01       //fd出错
#define EPOLLHUP (0x01<<1)  //fd挂起
#define EPOLLET  (0x01<<2)  //epoll的边沿触发方式
#define EPOLLIN  (0x01<<3)  //epoll的数据读取事件
#define EPOLLOUT (0x01<<4)  //epoll的数据发送事件
    
    /**
     * selecter操作类
     */
class TC_Selecter
{
public:

	/**
     * 构造函数
     * @param max_fd：使用最大的socket初始化selecter
	 */
    TC_Selecter();
	TC_Selecter(bool isET);

	/**
     * 析够函数
	 */
	~TC_Selecter();

	/**
     * 生成epoll句柄
     * @param max_connections : epoll服务需要支持的最大连接数
	 */
	void create(int max_connections);

	/**
     * 添加监听句柄
     * @param fd, 句柄
     * @param data, 辅助的数据, 可以后续在epoll_event中获取到
     * @param event, 需要监听的事件EPOLLIN|EPOLLOUT
     *              
	 */
	void add(int fd, long long data, __uint32_t event);

	/**
     * 修改句柄事件
     * @param fd, 句柄
     * @param data, 辅助的数据, 可以后续在epoll_event中获取到
     * @param event, 需要监听的事件EPOLLIN|EPOLLOUT
	 */
	void mod(int fd, long long data, __uint32_t event);

	/**
     * 删除句柄事件
     * @param fd, 句柄
     * @param data, 辅助的数据, 可以后续在epoll_event中获取到
     * @param event, 需要监听的事件EPOLLIN|EPOLLOUT
	 */
	void del(int fd, long long data, __uint32_t event);

	/**
     * 等待时间
     * @param millsecond, 毫秒
	 *
     * @return int, 返回多少个句柄有事件触发
	 */
	int wait(int millsecond);


    void selectPerRun();

	/**
     * 获取被触发的事件
	 *
	 * @return struct epoll_event&
	 */
    struct epoll_event& get(int i);
    //{ assert(_pevs != 0); return _pevs[i]; }
    
    bool isReadyRcve(int fd);
    bool isReadySend(int fd);
    bool isErro(int fd);
    
    int getiIEpollfd(){return _iEpollfd;}
    
protected:

	/**
     * 控制epoll
     * @param fd, 句柄
     * @param data, 辅助的数据, 可以后续在epoll_event中获取到
     * @param event, 需要监听的事件
     * @param op, 操作
	 */
	void ctrl(int fd, long long data, __uint32_t events, int op);

protected:

    /**
     * 	select的最大fd值
     */
    int _iEpollfd;

    
    
	/**
     * 最大连接数
	 */
	int	_max_connections;

	/**
     * 事件集
	 */
    std::map<int,struct epoll_event>  _pevs;
    
    fd_set _all_set;  //所有事件表
    fd_set _rcve_set; //可读事件表
    fd_set _send_set; //可写事件表
    fd_set _erro_set; //错误或者挂起事件
    
    //空事件
    struct epoll_event _emptyEvent;
    /**
     * 是否是ET模式
     */
    bool _et;
    
    int _nums;
};

}
#endif
#endif

