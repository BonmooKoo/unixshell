# unixshell
요구사항 : https://ee209.kaist.ac.kr/assignments/assignment5/shellsupplementary.html 
# TODO : 
 시작시, 프로그램은 유저의 home directory의 .ishrc파일을 읽는다.
 홈 디렉토리는 variable HOME 으로 명명된다.


 프로그램은 .ishrc의 각줄을 읽고 이를 즉시 출력한다.
 출력시, 프로그램은 % 기호와 스페이스 즉(% )각 줄의 맨 처음에 출력해야한다.

 프로그램은 사용자가 Ctrl+d 를 입력하면 종료해야한다.
 프로그램은 .ishrc 파일이 존재하지 않거나 읽을 수 없더라도 제대로 작동해야한다.

# 프로그램이 시작되고 아래 과정을 계속 진행해야한다.
1. 출력 : 매 줄을 읽고선, "% "을 std output stream에 출력한다.
2. std input stream에서 매 줄을 읽는다.
3. 어휘 분석을 통해 토큰으로 나누고, 토큰들을 배열 형태로 정리한다.
ex : 
input
x = 42
결과
[
  {type: 'identifier', value: 'x'},
  {type: 'operator', value: '='},
  {type: 'literal', value: '42'}
]
4. 토큰으로 나눈 내용을 통해 command를 생성한다.
5. command를 실행한다.

# 어휘분석
1. "" 또는 '' 안의 스트링은 하나의 토큰으로 구성된다.
2. '>' 와 '<' 는 각각의 토큰을 구성한다. ex. a>b >> "a" ">" "b"
3. 각 토큰은 단어다. 각 토큰은 공백이 아닌 문자들로 구성된다.
 
만약 한줄의 문장이 1023개 문자를 넘기게 되면 프로그램은 그걸 읽고 처리할 필요는 없지만, 그렇다고 메모리를 누수해서는 안된다.

# 토큰 분석
command 는 token들의 연속이다. 첫번째 토큰은 command의 이름을 나타낸다.

# 실행
프로그램은 4가지 shell build-in command를 수행한다.
## setenv var [value] :   
1. 새로운 변수 var이 존재하지 않으면 하나 새로 생성 / 이미 존재하면 그 값을 새로운 값 value로 수정
2. 변수의 값은 value로 설정하되, 값이 없으면 빈 문자열 ("")로 설정
부모 프로세스의 환경변수 var을 상속받는다. 새로 생성하거나 수정한 값은 현재 프로세스의 환경 변수에만 적용됨
특수 변수 HOME 프로그램은 HOME을 명시적으로 사용함.
## unsetenv var
환경 변수 var을 삭제함. 만약에 환경 변수 var이 존재 안하면 그냥 삭제함
## cd [dir]
현재 경로를 dir로 옮기거나 만약 dir이 없으면 HOME으로 변경함
## exit
exit status 를 0으로 설정하고 프로그램을 종료

주의
1. build -in command 들은 std input/output stream에서 읽거나 쓰기를 해서는 안된다. (std io stream은 오로지 main 프로세스만 가능)
2. 만약 내장명령과함꼐 파일 리다이랙션(> <) 이 존재한다면, 프로그램은 에러 메시지를 출력함.
3. build-in command 4가지가 아닌경우, 프로그램은 자식프로세스를 fork하고, 파일과 그 인자를 execvp 시스템call로 전달함.
   실행이 실패하면 실패 원인을 나타내는 에러 메시지를 출력함

# 프로세스 실행
프로그램이 내장명령이 아닌 명령어를 실행하면서 fork한 자식 프로세스는 전부 fore ground에서 실행이됨.
모든 자식 프로세스에 대해 wait를 수행함.

# Signal Handling
Ctrl-d 는 EOF를 의미함
만약 사용자가 ctrl-c를 입력하면 리눅스에서 SIGINT를 부모와 자식 프로세스에 전달함. 
부모는 이걸 무시함
자식은 무시 안함. 자식 프로세스가 직접 부모로 인해 SIGINT 핸들러를 받는게 아닌이상 종료해야됨.
만약 사용자가 ctrl-\를 입력하면 리눅스에서 SIGQUIT를 부모와 자식 프로세스에 전달함.
부모는 "Type Ctrl-\ again within 5 seconds to exit." 를 std output stream에 전달하고,
만약 wall-clock time으로 5초 이내에 명령이 들어오면 부모 프로세스는 종료함
자식 프로세스는 마찬가지로 핸들러가 따로 존재하지 않는 이상 이걸 받아야하고, 만약 받는다면 종료함.

# Redirection
"<" 토큰에 대해서, 프로그램에서는 std input을 해당 파일로 리다이랙트 해야함. 만약 여러번 수행하면 오류를 반환
그리고 해당 파일이 없으면, 역시나 오류를 반환
">" 토큰에 대해서, ㅣ프로그램은 std output을 해당 파일로 리다이랙트 해야함.
output file이 없으면, 새로 하나 만듬. 만약 이미 있는 파일이면, 안에 있는 내용을 싹 날리고 다시씀. 파일의 권한은 0600으로 설정

# 오류 핸들링
만약에 이상한 줄을 읽게되면, 프로그램은 실행을 안하고 오류 메시지를 std error stream에 출력함.
출력 메시지는 다음과 같음 "<프로그램 이름>:" 프로그램 이름은 argv[0]임
프로그램에서 작성한 에러 메시지와 sampleish 프로그램에서 작성한 에러 메시지와 구분이 되어야함
사용자의 input이 프로그램을 crash하지 않도록 모든 user error 에 대해 대응을 할 수 있어야함.

# 메모리 관리
프로그램은 메모리 누수 없이 작동해야함. malloc과 calloc에 대해서 free를 호출해야함.

# 추가점수
추후
