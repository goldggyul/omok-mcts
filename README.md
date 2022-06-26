# omok-mcts
- `오목 AI 만들기`
- MCTS 알고리즘을 이용했다.
- [Google C++ Syle Guide](https://google.github.io/styleguide/cppguide.html)를 참고하며 최대한 일관된 스타일의 코드를 작성했다.

## 개발 과정
- 우선 여러 Player(게임을 실행한 플레이어, 테스트를 위한 랜덤 플레이어, AI 플레이어)를 활용할 수 있도록 추상 클래스를 활용하여 여러 플레이어 전환이 쉽게 했다.
- AiPlayer는 Game Tree Search 알고리즘 중 Monte Carlo Tree Search를 이용해서 구현했다.
- 구현 후에 한 수를 둘 때마다 10~12초가 걸려서 시간을 줄여야 했다.
- 따라서 논문을 참고하여 다양한 방법들을 이용하여 대략 4초로 시간을 줄였다.
- 그 후엔 성능 개선을 위해 AiPlayer들끼리 게임을 해보면서 가장 적합한 상수값을 찾아 구현했다. 

## Monte Carlo Tree Search
### 단계
![image](https://user-images.githubusercontent.com/101321313/175829596-aa5a7134-2090-4bf2-9ef0-2da8023120df.png)
1. partial search tree 만들기
- partial tree?
    - partial tree가 없을 경우: 각 노드 모두 leaf node이므로 모두 Rollout(Simulation)이 일어나므로 연산 시간을 더 차지하게 됨
    - 따라서 어느 정도의 partial tree는 있어야 함
2. 아래 과정 반복 (위 이미지)
- Selection: good child node를 선택해서(`Tree policy`대로) root node로 start
    - expandable node에 도착할 때까지 tree policy대로 child 계속 선택
    - `expandable`: node가 not-terminal이고 unexplored child가 있음
- Expansion: 1개 이상의 child node를 add
    - 확장할 child node의 개수는 Tree policy에서 결정
- Simulation (rollout): 선택한 노드에서 게임 결과가 나올 때까지 시뮬레이션
    - 확장한 것에서 선택하여 시뮬레이션 한다.
    - 시뮬레이션의 outcome이 결국 value가 된다.
- Backpropagation: simulation 결과 나온 후 점수를 위로 올라가면서 reward에 반영해줌
    - 포인터를 따라 뒤로 가면서 value 값을 부모에게 반영
    - 시뮬레이션에 따른 게임 결과에 기반하여 업데이트
    - 좋은 수라면 여러 시뮬레이션에도 계속 좋은 value가 나와서 점점 그 수가 또 선택될 확률 증가
    
### Flowchart
<img src="https://user-images.githubusercontent.com/101321313/175829769-a02a96b4-00c6-4185-9b65-bd85ae4ede0f.png" width="60%" height="60%"/>

1. Current = root node (처음 시작)
2. Is current a leaf node?: child가 없는 노드인가?
    - Yes: 최초 방문인가?
        - Yes: Rollout → Backpropagation
        - No: current node에서 가능한 action대로 새로운 노드 추가, 거기서 또 Rollout → Backpropagation
    - No:  tree policy에 의해 child 선택 → UCB값에 의해 child 선택.
        - UCB: exploration과 eploitation을 잘 고려해서 만든 measure
        - UCB값은 방문횟수 0일 때 max값으로 시뮬레이션 한 번도 안한 게 더 큰 UCB값을 가짐. 따라서 맨 처음 partial tree의 노드들은 일단 모두 방문하여 simulation되고 UCB값에 따라 계속 선택됨

### UCT
- child 노드 선택 방법
- exploration과 exploitation과 적절한 balance가 있도록 값 구함
    
    ![image](https://user-images.githubusercontent.com/101321313/175829902-817407b0-790c-4381-ae1b-260f85bbb9d8.png)
    
    `exploration`
    - np: 현재 노드(parent) 방문 횟수
    - nj: child j의 방문 횟수: 방문 횟수 0이면 무한대로 먼저 선택됨
    - c: 0 이상의 어떤 상수 → balance 맞추기 용 상수
        - parent의 방문 횟수가 child의 방문 횟수보다 클 수록 값이 커진다. 즉, 선택이 덜 된 child일 수록 커지므로 탐색의 기회가 더 커진다. → child 방문을 덜 했다고 그게 좋은 수라는 것이 아니라, 더 탐색할 기회를 더 주는 것
        - 이 값을 여러번 바꿔보며 가장 적합한 값을 선택했다.
    
    `exploitation` 개발, 이용: 좋을 것 같은 것 계속 방문하여 개발
    
    - xj: 이 position을 선택하면서 발생한 reward의 평균 [0,1]
        - reward가 좋을 수록 커진다.
        
### 장단점
  - Aheuristic
      - 휴리스틱을 사용하지 않으므로 domain-specific한 지식이 필요 없다. 장점이자 단점으로, 뚜렷한 휴리스틱이 존재하는 경우라면(chess) min-max가 좋다. 휴리스틱을 이용해 min-max tree를 줄일 수 있기 때문. min-max의 단점인 depth가 증가할 때마다 폭이 매우 커지는 문제가 발생하지 않는다.
  - Anytime
      - 언제든 MCTS를 멈출 수 있다. 따라서 멈춘 상태에서의 best action을 리턴함.
  - Asymmetric
      - 더 promising node가 계속해서 선호된다.
        
## 실행
- Windows 환경에서 Visual Studio 이용하여 실행

### 입력
- 게임이 진행될 오목판의 크기, AI의 색깔을 입력한다.
- 예: `10 white`
- AI의 색을 'white'로 할 경우 게임을 실행한 플레이어가 'black'이 되므로 선공이다. 따라서 먼저 둘 곳을 입력한다.
- 두고 싶은 곳을 열/행 순으로 입력할 수 있다.
- 예: `5 5`
