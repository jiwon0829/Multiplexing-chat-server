## epoll이란?
- 리눅스에서만 호환성을 갖는다.
- select(), poll() 함수에 비해서 파일 디스크립터의 준비 상태를 알리는 방식에서 더 유연성을 갖는다.

<br><br>

## 작동 방식
  LT(Level triggered) | ET(Edge triggered)
  | ---------- | ---------- |
  상태변화를 감지하고자 하는 파일 디스크립터가 준비되면 무조건 알리는 방식 | 항상 새로 준비된 파일 디스크립터에 대해서만 알리는 방식

<br><br>

##  함수
###  epoll_create()
 ```cpp
 #include <sys/epoll.h>
 int epoll_create(int size);
 ```

<br>

성공하면 파일 디스크립터를 리턴하고, 에러가 발생하면 -1을 리턴<br>
매개변수 size에는 감시할 파일 디스크립터의 수를 명시한다.
 
<br>

### epoll_ctl()
epoll에 fd들을 등록/수정/삭제하는 함수
<br>

```cpp
#include <sys/epoll.h>
      
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *ev);
```

<br>

성공하면 0을 리턴하고, 에러가 발생하면 -1을 리턴<br>
<br>
**op 매개변수**
op | 설명 | 에러
| ------- | ---------- | ----- |
EPOLL_CTL_ADD | 관심있는 파일디스크립터를 추가 | EEXIST : 이미 존재하는 fd를 epfd 관심목록에 추가할때 발생
EPOLL_CTL_MOD | 기존 파일 디스크립터를 수정 | ENOENT : 관심 목록에 없는 fd를 수정하려할때 발생
EPOLL_CTL_DEL | 기존 파일 디스크립터를 관심 목록에서 삭제 | ENOENT : epfd 관심목록에 없는 fd를 제거하려할 때 발생
      
<br>
epoll_event 구조체<br>
      
```cpp
struct epoll_event{
	uint32_t events;  //epoll 이벤트(비트 마스트)
	epoll_data_t data;   //사용자 데이터
};
        
typedef union epoll_data{
	void *ptr;   //사용자 정의 데이터 포인터
	int fd;   //파일 디스크립터
	uint32_t u32;
	uint64_t u64;
}epoll_data_t;
```

<br>

**epoll event**
<br>

이벤트 | 설명
| ----- | ---------- |
EPOLLIN  | 수신할 데이터가 있다.
EPOLLPRI  | 중요 데이터 발생
EPOLLRDHUP | 연결 종료 또눈 Half-close 발생
EPOLLOUT  | 송신 가능
EPOLLET  | 에지 트리거를 알림 방식으로 선택 (기본은 레벨트리거)
EPOLLONESHOT  | 한번만 이벤트를 
EPOLLERR  | 에러가 발생함  

<br>

### epoll_wait()
관심있는 fd들에 무슨 일이 일어났는지 조사하여 발생한 사건들의 개수를 리턴하는 함수<br>

```cpp
#include <sys/epoll.h>
      
int epoll_wait(int edfd, struct epoll_event *evlist, int maxevents, int timeout);
```

성공하면 준비된 파일 디스크립터 수 리턴, 타임아웃인 경우 0을 리턴, 에러가 발생하면 -1을 리턴<br>
준비된 파일 디스크립터를 배열로 리턴한다. maxevents는 최대 몇개까지의 event만 처리할지 지정하는 수이다.<br>
timeout은 epoll_wait의 동작 특성을 지정하는 중요한 요소이다. 이 시간만큼 사건발생을 기다리라는 의미이며 기다리는 도중 사건이 발생하면 즉시 리턴한다. <br>
**timeout 값**
timout | 설명
| --- | ---------- |
-1 | 영원히 사건을 기다리는 blocking
0 | 사건이 있건 없건 조사만하고 즉시 리턴
      
  
<br><br>
