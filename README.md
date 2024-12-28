# Touch-Screen-MP3
한국공학대 EH교과 (ARM Cortex M3 시스템 프로그래밍) 기말 과제로 개발된 터치스크린 MP3 프로그램을 기록한 공간입니다.   
STM32F407 개발보드 위에서 작업이 이루어졌습니다.   
<br>

![image](https://github.com/user-attachments/assets/549fc051-dff9-409e-8420-d8d9bc256a4f)

* 목차
  1. 프로젝트 개요
  2. 주요 개발 내용
  3. 시스템 구성도
  4. 구현 내용
  5. 소프트웨어 이론 설명
  6. 결론

<br>

## 1. 프로젝트 개요
실습보드를 기반으로 MP3 Player를 구현하고자 한다.   
SD 메모리에 저장되어 있는 MP3 파일을 재생을 하고, 재생에 필요한 기능(Play/Pause/Next/Previous) 등의 기능들을 구현한다.   
   
LCD에 재생되는 음악 리스트 표시, 음악의 선택기능. 곡의 시간 표시와 재생되는 곡명을 표시한다.   
   
재생 음량을 조절할 수 있도록 한다.   
UI는 터치기능을 사용하여 구현한다.   

<br>

## 2. 주요 개발 내용
**MP3 Player 구현**   
* 재생 제어 기능 구현 : Play/Pause/Next/Previous
* 재생 방법 구현 : 단순재생
* 음악 리스트 표시, 음악 선택, 재생 시간 표시
* 음량 조절
* 터치 기능 구현
* 리모컨 기능 구현

<br>

## 3. 시스템 구성도
시스템 Block Diagram   
<br>
![image](https://github.com/user-attachments/assets/1ad8b167-71c6-411b-a97a-01b62d759110)

<br>

## 4. 구현 내용
* 음악 재생 제어 기능
  ```
  Previous 버튼 터치	: 이전곡 재생
  Play / Pause 버튼 터치	: 현재 재생중인 곡 정지, 재생
  Next 버튼 터치		: 다음곡 재생
  Volume Up, Down 버튼  	: (+) 버튼을 누르면 음량을 증가시키고 (-) 버튼을 누르면 음량을 감소시킨다.
  ```
  ![image](https://github.com/user-attachments/assets/d81f62f3-bb03-4fab-9ff5-e3f2a639e8b7)
<br>

* 음악의 재생 시간 및 이름 표시 기능
  ```
  현재 재생한 시간과 총 재생 시간을 숫자로 표시하며, 현재 재생 중인 음악의 이름을 파일 이름을 통해 보여준다.
  Artist 항목의 경우 음악 파일 내에  작성자 이름이 존재하면 나타난다.
  Time 항목은 Time: 전체시간 현재부분  형식으로 출력된다.
  Volume 항목은 볼륨을 높이거나 내릴때 마다 해당 볼륨 수치로 갱신되어 출력된다.
  ```
  ![image](https://github.com/user-attachments/assets/4288a89c-372c-4ce7-872c-5b44f4cd3f13)
<br>

* 리스트 표시 및 음악 선택 기능
  ```
  현재 메모리에 저장되어 있는 음악들이 15개의 음악 이름으로 나열됨
  15개보다 더 많으면 CH-, CH+ 리모컨 버튼을 사용해서 위아래로 리스트를 이동할 수 있다.
   원하는 음악을 선택해서 재생할 수 있도록 한다.
  ```
![image](https://github.com/user-attachments/assets/92ebc433-0e87-44a0-a5bc-ef6cbd5ee401)

<br>
<br>

## 5. 소프트웨어 이론 설명
이 소프트웨어는 stm32f4 와 보드에서 동작하는 mp3 player로, SD 카드에 저장되어 있는 .mp3파일을 로딩해 목록화, 재생하는 프로그램이다.
   
main.c 파일에 프로그램의 전체적인 흐름이 전부 담겨있다.   
main.c 는 전원이 공급되면 부팅을 시작하며, 부팅과정에서 모든 초기화를 진행하고 난 후 while (1) 을 통해 터치, 리모컨, 키 등 각종 입력장치로 들어오는 입력 값을 반복 검사한다.   

반복 내용은 main.c의 tick_process () 함수에 있으며, 스크린의 터치영역에 대한 동작과 각 리모컨, 키에 해당하는 동작이 조건문을 통해 설정되어 있다.   
.mp3 파일은 /source/audio_codec/mp3/ 에 위치한 코덱 파일과 audio.c 파일을 통해 재생된다. 

<br>

## 6. 첨부 소스파일
소스파일은 다음 디렉토리에서 확인하실 수 있습니다.
[stm32f4x_test_audio](./stm32fx_test_audio)
