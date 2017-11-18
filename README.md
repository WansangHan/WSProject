# WSProject
Simple IOCP / EPoll Server

Simeple Windows API Game

1. 버전 정보

~~~
* 운영체제
 + CentOS 7 (7.3.1611)
 + Windows 7
~~~
~~~
* 컴파일러
 + Linux Cmake Version : 3.6.1
 + Visual Studio 2017
~~~
~~~
* 라이브러리, 프로그램
 + Mysql Client : 5.7.20
 + protobuf : 3.3.0
 + jsoncpp : 0.5.0
 + curl : 7.54.0
 + boost c++ : 1.65.1
 * TBB : 2018_20170919oss_win
~~~

2. 빌드
~~~
 * Windows
  + Visual Studio에서 Debug 빌드 (Release는 아직 해보지 않음)
~~~
~~~
 * 리눅스
  + cd /mnt/hgfs/WSProject/WSSockServer/WSSockServer/linuxBuild/
  + cmake ..
  + make
  + ./WSSockServer
~~~

3. 데이터 베이스(Mysql)
~~~
 + 계정명 : WS
 + 비밀번호 : @013meadmin
 + 데이터 베이스 명 : WSDB
 
 + 자세한 구조는 /SQL_File/needed.sql 파일 참조
~~~