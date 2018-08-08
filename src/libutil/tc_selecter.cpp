#if __APPLE__

#include "util/tc_selecter.h"
#include <unistd.h>
#include <stdio.h>

namespace taf
{

TC_Selecter::TC_Selecter()
{
    _iEpollfd    = -1;
    _pevs.clear();
    _et         = false;
    _emptyEvent.events = 0;
    _emptyEvent.data.u64 = 0;
}
    
TC_Selecter::TC_Selecter(bool isET)
{
	_iEpollfd    = -1;
    _pevs.clear();
    _et         = false;
    _emptyEvent.events = 0;
    _emptyEvent.data.u64 = 0;
}

TC_Selecter::~TC_Selecter()
{
//    if(_pevs != NULL)
//    {
//        delete[] _pevs;
//        _pevs = NULL;
//    }
    if(!_pevs.empty())
    {
        _pevs.clear();
    }

	if(_iEpollfd > 0)
	{
        close(_iEpollfd);
        _iEpollfd = -1;
	}
}

void TC_Selecter::ctrl(int fd, long long data, __uint32_t events, int op)
{
	struct epoll_event ev;
	ev.data.u64 = data;
    if(_et)
    {
        ev.events   = events | EPOLLET;
    }
    else
    {
        ev.events   = events;
    }
    
    switch (op) {
        case EPOLL_CTL_ADD:
        {
            _pevs[fd] = ev;
            if(events & EPOLLIN){
                FD_SET(fd,&_rcve_set);
            }
            if(events & EPOLLOUT){
                FD_SET(fd,&_send_set);
            }
            if((events & EPOLLERR) || (events & EPOLLHUP)){
                FD_SET(fd,&_erro_set);
            }
            break;
        }
        case EPOLL_CTL_MOD:
        {
            _pevs[fd] = ev;
            if(events & EPOLLIN){
                FD_SET(fd,&_rcve_set);
            }else{
                FD_CLR(fd,&_rcve_set);
            }
            if(events & EPOLLOUT){
                FD_SET(fd,&_send_set);
            }else{
                FD_CLR(fd,&_send_set);
            }
            if((events & EPOLLERR) || (events & EPOLLHUP)){
                FD_SET(fd,&_erro_set);
            }else{
                FD_CLR(fd,&_erro_set);
            }
            break;
        }
        case EPOLL_CTL_DEL:
        {
            _pevs.erase(fd);
            FD_CLR(fd,&_rcve_set);
            FD_CLR(fd,&_send_set);
            FD_CLR(fd,&_erro_set);
            break;
        }
//        default:
//            break;
    }
    FD_SET(fd, &_all_set);  // 添加监听fd
    if(fd > _iEpollfd)
    {
        _iEpollfd = fd;
    }
//    epoll_ctl(_iEpollfd, op, fd, &ev);
}

void TC_Selecter::create(int max_connections)
{
	_max_connections = max_connections;

//    _iEpollfd = epoll_create(_max_connections + 1);

	if(!_pevs.empty())
	{
		_pevs.clear();
	}

//    _pevs = new epoll_event[_max_connections + 1];
}

void TC_Selecter::add(int fd, long long data, __uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_ADD);
}

void TC_Selecter::mod(int fd, long long data, __uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_MOD);
}

void TC_Selecter::del(int fd, long long data, __uint32_t event)
{
	ctrl(fd, data, event, EPOLL_CTL_DEL);
}

void TC_Selecter::selectPerRun()
{
    // _iEpollfd = -1;   // 初始化max_fd
    FD_ZERO(&_all_set);
    // FD_SET(listen_fd, &master_set);  // 添加监听fd
}

int TC_Selecter::wait(int millsecond)
{
//    return epoll_wait(_iEpollfd, _pevs, _max_connections + 1, millsecond);
    struct timeval dtime;
    dtime.tv_sec = int(millsecond/1000);
    dtime.tv_usec = int(millsecond%1000)*1000;


    FD_ZERO(&_rcve_set);
    memcpy(&_rcve_set, &_all_set, sizeof(_all_set));
    FD_ZERO(&_send_set);
    memcpy(&_send_set, &_all_set, sizeof(_all_set));
    FD_ZERO(&_erro_set);
    memcpy(&_erro_set, &_all_set, sizeof(_all_set));

    _nums = select(_iEpollfd+1, &_rcve_set, &_send_set, &_erro_set, &dtime);
    return _nums;
}


struct epoll_event& TC_Selecter::get(int i)
{
    assert(_pevs.size() != 0);
    if(_pevs.find(i) != _pevs.end())
    {
        return _pevs[i];
    }else{
        printf("get empty event");
        return _emptyEvent;
    }
}
    
bool TC_Selecter::isReadyRcve(int fd)
{
    return FD_ISSET(fd,&_rcve_set);
}

bool TC_Selecter::isReadySend(int fd)
{
    return FD_ISSET(fd,&_send_set);
}

bool TC_Selecter::isErro(int fd)
{
    return FD_ISSET(fd,&_erro_set);
}

}

#endif
