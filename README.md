## ping 프로그램 만들기

### 목적
- ping과 동일하게 동작하는 프로그램 작성

### 과정
- tcpdump를 이용한 패킷 분석
- ICMP 프로토콜의 이해 (Echo Reply / Echo Response)

### 구현
- ICMP socket으로 sendto 함수를 사용하여 데이터 전송
- ICMP socket으로 recvfrom 함수를 사용하여 데이터 파싱
- -v 옵션 적용 시 에러 dump 출력
