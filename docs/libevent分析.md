# `libevent`分析
## `Reactor`模式
![reactor](images/普通调用与reactor模式.png)
`Reactor`模式的优点：
> 1. 响应快，不被单个同步事件阻塞
> 2. 编程简单，避免多线程/进程切换开销，避免复杂的多线程及同步问题
> 3. 可扩展性，通过扩展`Reactor`来充分利用`CPU`资源
> 4. 可复用性，框架与具体事件处理逻辑无关

`Reactor UML`:
![reactorUML](./images/reactorUML.png)
1. 事件源
程序在指定句柄上注册关系的事件；
2. `event demultiplexter---`事件多路分发机制
操作系统提供的多路`I/O`复用机制。
3. `Reactor--`反应器
事件管理的接口。负责注册，注销和调用回调函数。
常用声明方式：
```cpp
class Reactor{
public:
  int register_handle(Event_handler *pHandler, int event);
  int remove_handle(Event_handler *pHandler, int event);
  void handler_event(timeval *ptv);
};
```
4. `Event Handlers--`事件处理器
事件处理接口。
常用声明方式：
```cpp
class Event_Handler{
public:
  virtual void handle_read() = 0;
  virtual void handle_write() = 0;
  virtual void handle_timeout() = 0;
  virtual void handle_close() = 0;
  virtual HANDLE get_handle() = 0;
};

class Event_Handler{
public:
  virtual void handle_events(int events) = 0;
  virtual HANDLE get_handle() = 0;
};
```
![reactor时序](./images/reactor时序.png)

## 基本使用场景和事件流程
![libevent流程](./images/libevent流程.png)




