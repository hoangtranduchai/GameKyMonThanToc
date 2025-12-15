# GAME DESIGN DOCUMENT (GDD)
## Game Kỳ Môn Thần Tốc

**Version:** 1.0  
**Last Updated:** December 16, 2025  
**Project Status:** ✅ Production Complete  
**Document Type:** Comprehensive Game Design

---

## 📖 MỤC LỤC

1. [Game Overview](#1-game-overview)
2. [Cốt Truyện (Story)](#2-cốt-truyện-story)
3. [Nhân Vật (Characters)](#3-nhân-vật-characters)
4. [Cơ Chế Cốt Lõi (Core Mechanics)](#4-cơ-chế-cốt-lõi-core-mechanics)
5. [Vòng Lặp Game (Core Loop)](#5-vòng-lặp-game-core-loop)
6. [Điều Khiển (Controls)](#6-điều-khiển-controls)
7. [Level Design](#7-level-design)
8. [UI/UX Design](#8-uiux-design)
9. [Technical Specifications](#9-technical-specifications)

---

## 1. GAME OVERVIEW

### 1.1. High Concept
**Kỳ Môn Thần Tốc** là game puzzle strategy kết hợp AI, nơi người chơi phải thu thập các Trận Nhãn thần bí với đường đi tối ưu nhất. Game sử dụng thuật toán TSP (Traveling Salesman Problem) để đánh giá trí thông minh của người chơi.

### 1.2. Genre
- **Primary:** Puzzle Strategy
- **Secondary:** Logic, Pathfinding, Educational

### 1.3. Target Platform
- **PC (Windows)** - Primary
- SDL2 cross-platform framework

### 1.4. Target Audience
- Sinh viên CNTT (18-25 tuổi)
- Puzzle enthusiasts
- Algorithmic thinkers

### 1.5. Key Features
- ✅ AI-powered evaluation (BFS + TSP)
- ✅ Unlimited Undo system
- ✅ Rank system (S/A/B)
- ✅ Particle effects & animations
- ✅ Multiple levels

---

## 2. CỐT TRUYỆN (STORY)

### 2.1. Thế Giới Quan (World Building)

**Setting:** Vương quốc cổ đại phương Đông - Thời kỳ Tam Quốc huyền thoại

**Background:**
> Trong thời loạn lạc, các chiến lược gia đã sử dụng "Kỳ Môn Độn Giáp" - một môn thuật số bí ẩn dựa trên toán học và thiên văn học để dự đoán vận mệnh và hoạch định chiến lược. Những chiến lược gia vĩ đại như Gia Cát Lượng (Zhuge Liang) đã sử dụng nó để chiến thắng vô số trận đánh dù lực lượng yếu thế.

**Premise:**
> Bạn là một học trò của Thiên Cơ Viện - nơi đào tạo các chiến lược gia tinh nhuệ. Để trở thành Đại Chiến Lược Gia, bạn phải vượt qua "Thử Thách Thiên Mệnh" - một chuỗi bài toán tối ưu hóa đường đi được thiết kế bởi các bậc thầy cổ đại.

**Conflict:**
> Mỗi bản đồ chứa các "Trận Nhãn" (Formation Eyes) - những điểm năng lượng then chốt của進蝕 trận pháp. Bạn phải kích hoạt TẤT CẢ Trận Nhãn để phá vỡ trận pháp. Tuy nhiên, càng di chuyển nhiều, năng lượng của bạn càng cạn kiệt. Chỉ có con đường tối ưu nhất mới giúp bạn vượt qua thử thách.

### 2.2. Cốt Truyện Chi Tiết

#### **MỞ ĐẦU (Opening - Tutorial Level)**

**Scene 1: Thiên Cơ Viện**
```
[Nền: Học viện cổ kính với kiến trúc phương Đông]
[Nhạc: Nhẹ nhàng, bí ẩn]

Sư Phụ: "Ngươi đã học Kỳ Môn Độn Giáp được 3 năm.
         Giờ là lúc kiểm nghiệm tài năng."

Người Chơi: "Con sẵn sàng, Sư Phụ."

Sư Phụ: "Trước mặt ngươi là Trận Pháp Nhập Môn.
         Hãy tìm đường đi ngắn nhất qua tất cả Trận Nhãn.
         Nhớ rằng: Mỗi bước đi đều tiêu hao năng lượng.
         Chỉ kẻ thông minh mới có thể tối ưu hóa hành trình."

[Tutorial: Level 1 - 3x6 map, 3 shrines]
```

**Gameplay Moment:** Người chơi học cách di chuyển và thu thập Trận Nhãn đầu tiên.

**Kết quả:** 
- Nếu Rank S: "Tuyệt vời! Ngươi đã tìm ra Con Đường Thiên Mệnh!"
- Nếu Rank A/B: "Ngươi đã hoàn thành, nhưng vẫn chưa tối ưu. Hãy thử lại."

---

#### **THÂN BÀI (Mid-game - Intermediate Levels)**

**Scene 2: Thử Thách Tiến Giai**
```
[Level 2 - 5x8 map, 5 shrines]

Sư Phụ: "Ngươi đã vượt qua Nhập Môn.
         Giờ đây, trận pháp sẽ phức tạp hơn.
         Núi non hiểm trở, đường đi khó khăn.
         Hãy suy nghĩ kỹ lưỡng trước mỗi bước."

[Gameplay: Người chơi phải lập kế hoạch phức tạp hơn]
```

**Internal Monologue (Player's thoughts):**
> "Nếu ta đi qua Trận Nhãn này trước... không, như vậy sẽ bị chặn bởi núi.
>  Phải tìm thứ tự khác. Có lẽ AI đã tìm ra được..."

**Gameplay Moment:** Người chơi bắt đầu hiểu được tầm quan trọng của planning.

---

#### **KẾT THÚC (Climax - Final Level)**

**Scene 3: Thử Thách Chung Cực**
```
[Level 3 - 7x10 map, 7 shrines]

Sư Phụ: "Đây là thử thách cuối cùng.
         Trận Pháp Bát Quái - khó nhất trong tất cả.
         Nếu ngươi vượt qua được với Con Đường Thiên Mệnh,
         ngươi sẽ chính thức trở thành Đại Chiến Lược Gia."

[Epic music intensifies]

[Gameplay: Người chơi phải áp dụng tất cả kiến thức đã học]
```

---

**ENDING VARIANTS:**

**Ending 1: Perfect Victory (All Rank S)**
```
Sư Phụ: "Tuyệt vời! Ngươi không chỉ hoàn thành thử thách,
         mà còn tìm ra Con Đường Tối Ưu trong MỌI trường hợp.
         
         Từ nay, ngươi mang danh hiệu 'Thiên Cơ Sư'.
         Hãy sử dụng trí tuệ này để giúp đỡ người đời."

[Roll Credits: Player name + "Thiên Cơ Sư" title]
[Unlock: New game+ mode / Level Editor teaser]
```

**Ending 2: Normal Victory (Mixed Ranks)**
```
Sư Phụ: "Ngươi đã hoàn thành thử thách.
         Tuy nhiên, vẫn còn nhiều điều để học hỏi.
         
         Con đường tối ưu không phải lúc nào cũng dễ tìm,
         nhưng nó luôn tồn tại. Hãy tiếp tục tu luyện."

[Encourage player to retry for Rank S]
```

---

### 2.3. Lore & Mythology

#### **Trận Nhãn (Formation Eyes)**
> Trong học thuyết Kỳ Môn Độn Giáp, "Trận Nhãn" là những điểm then chốt của trận pháp, tương tự như huyệt đạo trên cơ thể con người. Kích hoạt Trận Nhãn theo đúng thứ tự sẽ tạo ra "Thiên Mệnh" (Destiny Path) - con đường tối ưu nhất.

#### **Núi Non Hiểm Trở**
> Những ngọn núi trên bản đồ đại diện cho "Môn Tử" (Death Gate) trong Kỳ Môn Độn Giáp - những vùng cấm địa không thể vượt qua. Chiến lược gia thông minh phải biết tránh né và tìm đường vòng.

#### **Số Bước (Steps)**
> Mỗi bước di chuyển tiêu tốn "Khí" (Energy). Chỉ có kẻ thông minh nhất mới có thể tối ưu hóa việc sử dụng Khí để hoàn thành sứ mệnh.

---

### 2.4. Tone & Atmosphere

**Overall Mood:** Strategic, Contemplative, Mystical

**Inspiration:**
- **Literary:** Romance of the Three Kingdoms (tactical battles)
- **Visual:** Ink wash painting aesthetic
- **Audio:** Traditional Chinese instruments (erhu, guzheng)

**Emotional Journey:**
- **Start:** Curiosity, Excitement (learning)
- **Middle:** Focus, Determination (mastery)
- **End:** Satisfaction, Achievement (victory)

---

## 3. NHÂN VẬT (CHARACTERS)

### 3.1. Nhân Vật Chính (Protagonist)

#### **Người Chơi / Player Character**

**Tên:** [Customizable - Player chooses]  
**Nickname:** "Học Trò Thiên Cơ" (Disciple of Heavenly Secrets)

**Tiểu Sử (Biography):**
- **Xuất thân:** Dân làng nghèo, có năng khiếu toán học bẩm sinh
- **Tuổi:** 18-20 (young adult)
- **Động lực:** Muốn trở thành chiến lược gia để bảo vệ quê hương
- **Điểm yếu:** Thiếu kinh nghiệm thực tế, đôi khi quá lý thuyết

**Tính Cách (Personality):**
- **Logical:** Tiếp cận mọi vấn đề bằng logic và toán học
- **Persistent:** Không bỏ cuộc, sẵn sàng thử lại nhiều lần
- **Humble:** Luôn học hỏi, thừa nhận khi mắc sai lầm
- **Curious:** Muốn hiểu rõ "tại sao" hơn là chỉ "thế nào"

**Visual Design (Concept):**
```
Appearance:
- Áo học trò đơn giản (simple scholar robe)
- Màu: Xanh lam nhạt (light blue) - symbolizing wisdom
- Phụ kiện: Sách cổ nhỏ trên lưng (book of strategies)
- Animation: 8-frame walk/idle cycles

Facial Features:
- Trẻ trung, mắt sáng (youthful, bright eyes)
- Expression: Tập trung, quyết đoán (focused, determined)
```

**Character Arc:**
```
Beginning: Naive student, learns basic movements
    ↓
Middle: Developing strategist, understands planning
    ↓
End: Master tactician, achieves optimal solutions
```

**Quotes:**
- *"Mỗi bước đi đều là một quyết định. Hãy chọn khôn ngoan."*
- *"Thất bại không đáng sợ. Quan trọng là học được gì từ nó."*
- *"Con đường tối ưu không phải may mắn. Đó là sự tính toán."*

---

### 3.2. Nhân Vật Phụ (Supporting Characters)

#### **Sư Phụ (Master / Mentor)**

**Tên:** Huyền Cơ Đạo Nhân (Master of Mysterious Secrets)  
**Vai trò:** Narrator, Teacher, Judge

**Tiểu Sử:**
- **Xuất thân:** Chiến lược gia huyền thoại đã nghỉ hưu
- **Tuổi:** ~60+ (elderly sage)
- **Thành tựu:** Đã giải 10,000 bài toán chiến lược trong đời
- **Triết lý:** "Trí tuệ không phải trời ban. Đó là kết quả của luyện tập."

**Tính Cách:**
- **Wise:** Hiểu sâu sắc về thuật toán và triết lý
- **Strict but Fair:** Khắt khe nhưng công bằng trong đánh giá
- **Patient:** Cho phép học trò thử nhiều lần (undo system)
- **Mysterious:** Không tiết lộ lời giải, chỉ gợi ý

**Visual Design:**
```
Appearance:
- Áo choàng dài màu xám trắng (long gray-white robe)
- Râu dài bạc phơ (long white beard)
- Cầm "La Bàn Thiên Cơ" (Compass of Destiny)
- Aura: Surrounded by floating mathematical symbols

Portrait:
- Drawn in ink wash style
- Appears in cutscenes and UI
```

**Dialog Style:**
- Speaks in classical Chinese proverbs
- Uses metaphors from nature
- Never directly gives answers
- Encourages self-discovery

**Key Lines:**
```
Tutorial: "Quan sát trước, hành động sau. Đó là chìa khóa."
Mid-game: "Núi cao có đường vòng. Đừng chỉ nhìn thẳng."
End-game: "Khi ngươi thấy toàn cục, Thiên Mệnh sẽ tự hiện."
Victory: "Thiên Cơ đã chọn ngươi. Hãy xứng đáng với nó."
```

---

#### **AI System (The Algorithm)**

**Tên:** "Thiên Cơ" (Heavenly Machine / Destiny Calculator)  
**Vai trò:** Silent Antagonist / Judge / Teacher

**Concept:**
> Thiên Cơ không phải kẻ thù, mà là một "thực thể" đại diện cho giải pháp tối ưu. Nó không có hình thể, nhưng luôn hiện diện qua "Con Đường Vàng" (Golden Path) và đánh giá Rank.

**Personality (If it had one):**
- **Perfect:** Never makes mistakes
- **Emotionless:** Pure mathematical logic
- **Challenging:** Sets the bar for excellence
- **Fair:** Same rules for everyone

**Visual Representation:**
```
In-game:
- Hiển thị qua "Thiên Mệnh: X bước" (Destiny: X steps)
- Golden light effect khi player match optimal
- Constellation patterns trên background (symbolizing math)

UI:
- Graph patterns
- Binary code aesthetic
- Chinese hexagram symbols (Ba Gua)
```

**Philosophical Role:**
> Thiên Cơ đại diện cho "Ideal Solution" trong mọi bài toán. Player's journey là hành trình từ "human intuition" đến "algorithmic perfection".

---

### 3.3. Character Relationships

```
         Sư Phụ (Teacher)
         ↓ teaches
    Người Chơi (Student)
         ↓ challenges
    Thiên Cơ (Algorithm)
         ↓ evaluates
       Kết Quả (Result)
```

**Dynamic:**
- Sư Phụ guides but doesn't solve
- Player learns through trial and error
- Thiên Cơ provides objective measurement
- Together: Perfect learning environment

---

### 3.4. Character Development

**Tutorial (Level 1):**
- Player: "Sư Phụ, con phải làm sao?"
- Master: "Hãy tin vào trực giác của ngươi."
- Outcome: Player learns by doing

**Mid-game (Level 2):**
- Player: "Con đã thử nhiều cách nhưng không tối ưu."
- Master: "Vội vàng chỉ tổn thất thời gian. Hãy suy nghĩ kỹ."
- Outcome: Player learns planning

**End-game (Level 3):**
- Player: "Con hiểu rồi. Đây là bài toán TSP!"
- Master: *smiles* "Ngươi đã lĩnh hội được tinh túy."
- Outcome: Player achieves mastery

---

## 4. CƠ CHẾ CỐT LÕI (CORE MECHANICS)

### 4.1. Gameplay Foundation

#### **Core Mechanic 1: Grid-based Movement**
```
Input: WASD / Arrow Keys
Output: Player moves one tile
Constraint: Cannot move through mountains (#)
```

**Rules:**
- One move = One tile in cardinal direction (no diagonals)
- Each move costs 1 step
- Movement is turn-based (wait for animation)

**Technical:**
- Grid size: Variable (3×6 to 7×10)
- Tile size: 640×640 pixels (auto-scaled)
- Collision: Checked before movement execution

---

#### **Core Mechanic 2: Shrine Collection**
```
Condition: Player tile == Shrine tile (2)
Action: Shrine collected, tile becomes ground (0)
Feedback: Particle effect + sound + counter update
```

**Rules:**
- Shrines can be collected in ANY order
- Once collected, cannot be recollected
- Must collect ALL shrines to win

**Progression Tracking:**
- Current: X shrines collected
- Total: N shrines on map
- Visual: Shrines disappear after collection

---

#### **Core Mechanic 3: Optimal Path Calculation**
```
Algorithm: BFS + TSP Dynamic Programming
Input: Map + Shrine positions
Output: Minimal steps required
Complexity: O(2^N × N²) for N shrines
```

**Process:**
1. **BFS Phase:** Calculate distances between all points
2. **TSP Phase:** Find shortest tour visiting all shrines
3. **Display:** Show optimal steps to player

**Player Comparison:**
```
if player_steps == optimal_steps: Rank S
else if player_steps <= optimal_steps + 3: Rank A
else: Rank B
```

---

#### **Core Mechanic 4: Undo System**
```
Trigger: Press 'U' key
Action: Restore previous game state
Limit: Unlimited (stack-based)
```

**What gets saved:**
- Player position (row, col)
- Current steps count
- Shrines collected list
- Map state (which shrines remain)

**Stack Structure:**
```cpp
struct GameStateMoment {
    int playerRow, playerCol;
    int currentSteps;
    int shrinesCollected;
    vector<pair<int,int>> visitedShrines;
};
stack<GameStateMoment> historyStack;
```

---

### 4.2. Win/Lose Conditions

#### **Win Condition:**
```
ALL shrines collected
→ Trigger victory sequence
→ Calculate rank (S/A/B)
→ Show win screen
→ Option to next level or retry
```

**Victory Flow:**
```
1. Collect last shrine
2. Particle explosion (2 seconds)
3. Music fade out
4. Win sound plays
5. Fade to win screen
6. Display rank + stats
```

#### **Lose Condition:**
```
NONE - Player cannot "lose" in traditional sense
```

**Design Philosophy:**
> Puzzle games should encourage experimentation. 
> "Failure" is simply "not optimal yet".
> Player can always Undo and try different approach.

---

### 4.3. Scoring & Ranking System

#### **Metric: Steps Taken**
```
Primary Score: Number of moves made
Lower = Better
```

#### **Rank Calculation:**
```
Rank S (Perfect):
    player_steps == optimal_steps
    Message: "THIÊN MỆNH HOÀN TẤT!"
    Color: Cyan glow

Rank A (Good):
    player_steps <= optimal_steps + 3
    Message: "HOÀN THÀNH TỐT"
    Color: Gold

Rank B (Complete):
    player_steps > optimal_steps + 3
    Message: "ĐÃ HOÀN THÀNH"
    Color: Silver
```

**Display:**
```
┌─────────────────────────┐
│  CHIẾN THẮNG!           │
│                         │
│  Số bước: 12            │
│  Tối ưu: 9              │
│  Đánh giá: RANK A       │
│                         │
│  [Enter] Màn tiếp theo  │
│  [R] Chơi lại           │
└─────────────────────────┘
```

---

### 4.4. Progression System

#### **Level Progression:**
```
Level 1 (Tutorial)
  ↓ Complete
Level 2 (Intermediate)
  ↓ Complete
Level 3 (Advanced)
  ↓ Complete
Victory Screen
  ↓ All Rank S?
[Secret Ending / Credits]
```

**Unlock System:**
- Linear progression (must complete previous level)
- Can replay any completed level
- Ranks are saved per level

---

### 4.5. Difficulty Scaling

| Level | Size  | Shrines | Obstacles | Optimal | Difficulty |
|-------|-------|---------|-----------|---------|------------|
| 1     | 3×6   | 3       | Low       | 9       | ⭐         |
| 2     | 5×8   | 5       | Medium    | ~15     | ⭐⭐       |
| 3     | 7×10  | 7       | High      | ~25     | ⭐⭐⭐     |

**Scaling Factors:**
- Map size increases
- More shrines to collect
- More mountains block paths
- Optimal path becomes harder to find mentally

---

## 5. VÒNG LẶP GAME (CORE LOOP)

### 5.1. Micro Loop (Single Level)

```
┌──────────────────────────────────────────┐
│                                          │
│  1. OBSERVE                              │
│     - Study map layout                   │
│     - Identify shrine positions          │
│     - Note mountain obstacles            │
│            ↓                             │
│  2. PLAN                                 │
│     - Mental pathfinding                 │
│     - Estimate optimal route             │
│     - Consider alternatives              │
│            ↓                             │
│  3. EXECUTE                              │
│     - Move step by step                  │
│     - Collect shrines                    │
│     - Monitor step count                 │
│            ↓                             │
│  4. EVALUATE                             │
│     - Compare with optimal               │
│     - Receive rank (S/A/B)               │
│     - Identify mistakes                  │
│            ↓                             │
│  5. DECIDE                               │
│     ├─ Satisfied? → Next level           │
│     └─ Not optimal? → Undo/Retry ←──────┤
│                                          │
└──────────────────────────────────────────┘
```

**Time per iteration:** 2-5 minutes  
**Replayability:** High (chase Rank S)

---

### 5.2. Macro Loop (Full Game)

```
Level 1 (Easy)
   ↓ Learn basics
   ↓
Level 2 (Medium)
   ↓ Develop strategy
   ↓
Level 3 (Hard)
   ↓ Master technique
   ↓
All Rank S?
   ├─ YES → Perfect Victory
   └─ NO → Improve and retry
```

**Progression Hooks:**
- **Curiosity:** "Can I do better?"
- **Mastery:** "I need Rank S!"
- **Completion:** "Must finish all levels!"

---

### 5.3. Engagement Loops

#### **Learning Loop:**
```
Try → Fail → Understand → Retry → Success
```

**Example:**
1. Player tries direct path → Too many steps (Rank B)
2. Player learns about planning → Better path (Rank A)
3. Player masters algorithm thinking → Optimal path (Rank S)

#### **Optimization Loop:**
```
Complete (Rank B) → Analyze → Optimize → Retry → Rank A → Perfect → Rank S
```

**Psychological Reward:**
- Initial completion: ✅ Satisfaction
- Rank improvement: 📈 Progress
- Rank S achievement: 🏆 Mastery

---

### 5.4. Session Structure

**Typical Play Session (20 minutes):**
```
0:00 - 0:02   Loading + Main menu
0:02 - 0:05   Level 1 (first attempt)
0:05 - 0:08   Level 1 (retry for Rank S)
0:08 - 0:14   Level 2 (multiple attempts)
0:14 - 0:20   Level 3 (challenge)
0:20+         Replay or quit
```

**Drop-off Points:**
- After Level 1: ~30% (casual players)
- After Level 2: ~20% (intermediate)
- After Level 3: ~10% (completionists)
- **Target retention:** 50% complete all levels

---

### 5.5. Motivation Drivers

#### **Intrinsic Motivation:**
- **Mastery:** "I want to understand the optimal solution"
- **Autonomy:** "I choose my own path"
- **Challenge:** "Can I beat the AI?"

#### **Extrinsic Motivation:**
- **Ranks:** Visible achievement (S/A/B)
- **Completion:** Finish all levels
- **Competition:** Compare with friends (future: leaderboard)

---

## 6. ĐIỀU KHIỂN (CONTROLS)

### 6.1. Keyboard Layout

#### **Movement (Core Gameplay)**
```
┌─────────┬─────────┬─────────┐
│         │    W    │         │
│         │    ↑    │         │
├─────────┼─────────┼─────────┤
│    A    │         │    D    │
│    ←    │  Player │    →    │
├─────────┼─────────┼─────────┤
│         │    S    │         │
│         │    ↓    │         │
└─────────┴─────────┴─────────┘
```

**Keys:**
- `W` or `↑` → Move Up
- `A` or `←` → Move Left
- `S` or `↓` → Move Down
- `D` or `→` → Move Right

**Behavior:**
- One press = One tile movement
- Hold key = Auto-repeat after delay (200ms)
- Blocked movement = No action + no step cost

---

#### **System Controls**
```
U         → Undo last move
ESC       → Pause / Menu
Q         → Quit (confirmation)
Enter     → Confirm / Select
Space     → Confirm / Select
R         → Retry level (from win screen)
M         → Return to Main Menu
```

---

#### **Menu Navigation**
```
↑ / ↓     → Select menu option
Enter     → Confirm selection
ESC       → Back / Cancel
```

---

### 6.2. Control Feel (Game Feel)

#### **Movement Responsiveness:**
```
Input Delay: 0ms (instant response)
Movement Cooldown: 130ms (prevents spam)
Animation Duration: ~300ms (smooth transition)
```

**Design Philosophy:**
> Controls should feel "deliberate" not "floaty".
> Each move is a conscious decision, not reflex.

#### **Visual Feedback:**
```
Key Press → Player animation starts (immediately)
          → Smooth lerp to destination (300ms)
          → Footstep sound plays
          → Step counter updates
```

---

### 6.3. Accessibility Considerations

#### **Key Rebinding (Future Feature):**
```
Currently: Fixed WASD/Arrows
Future: Customizable in settings
```

#### **Alternative Input (Future):**
- Mouse click to move (point-and-click)
- Gamepad support (D-pad/Analog stick)
- Touch controls (for mobile port)

#### **Input Forgiveness:**
- No "miss-input" penalty
- Blocked moves don't count as steps
- Undo system allows correction

---

### 6.4. Control Tutorial (In-game)

**Level 1 Introduction:**
```
┌─────────────────────────────────────┐
│  ĐIỀU KHIỂN                         │
│                                     │
│  WASD / Mũi tên - Di chuyển         │
│  U              - Quay lại          │
│  ESC            - Tạm dừng          │
│                                     │
│  [Nhấn phím bất kỳ để tiếp tục]    │
└─────────────────────────────────────┘
```

**On-screen Hints (First time only):**
- "Hãy thử di chuyển bằng WASD!"
- "Nhấn U để hoàn tác nếu muốn."

---

## 7. LEVEL DESIGN

### 7.1. Level 1: "Nhập Môn" (Beginner's Trial)

**Objective:** Introduce core mechanics

**Map Layout:**
```
Level 1 (3 rows × 6 columns):
. S . . # .
. . # . # .
S . # S . .

Legend:
. = Ground (walkable)
# = Mountain (blocked)
S = Shrine (collectible)
```

**Design Principles:**
- **Simple:** Small map, easy to visualize
- **Guided:** Obvious path exists
- **Forgiving:** Multiple solutions work
- **Teaching:** Shows importance of path order

**Optimal Solution:**
```
Start: (0, 0)
Path: (0,0) → (0,1) → (0,2) → (1,2) → (2,2) → (2,3) → (2,4) → (1,4) → (0,4) → (0,5)
Steps: 9
Shrines order: (0,1) → (2,4) → (0,5) [or other valid combinations]
```

**Learning Outcomes:**
- ✅ Understand movement
- ✅ Learn shrine collection
- ✅ See step counter
- ✅ Experience win condition

---

### 7.2. Level 2: "Tiến Giai" (Intermediate Challenge)

**Objective:** Test strategic thinking

**Estimated Layout:**
```
Level 2 (5 rows × 8 columns):
. . S # . S . .
. # . # . . # S
. . # S # . . .
S . . . . # S .
. # . . # . . .
```

**Design Principles:**
- **Complex:** More shrines, more obstacles
- **Non-obvious:** Optimal path requires planning
- **Multiple traps:** Direct routes are sub-optimal
- **Rewarding:** Clear difference between good/bad paths

**Difficulty Spike:**
- Shrines: 3 → 5 (+67%)
- Map area: 18 → 40 (+122%)
- Obstacles: ~4 → ~8 (+100%)

**Challenge:**
> Player must start thinking about "which shrine to collect first"
> rather than "how to reach each shrine".

---

### 7.3. Level 3: "Chung Cực" (Master's Test)

**Objective:** Prove mastery

**Estimated Layout:**
```
Level 3 (7 rows × 10 columns):
. S . . # . . S . .
. . # . # . # . # .
. # . S . . . . # S
# . # # . # S . . .
. . . . # . # . # .
S . # . . . . S . .
. . . # . S . . . .
```

**Design Principles:**
- **Challenging:** Requires multiple attempts
- **Non-linear:** Many possible routes
- **Puzzle-like:** Optimal solution is non-obvious
- **Fair:** Solvable with logical thinking

**Expected Behavior:**
- First attempt: Rank B (exploration)
- Second attempt: Rank A (refinement)
- Third+ attempt: Rank S (optimization)

---

### 7.4. Level Design Philosophy

#### **Core Pillars:**

1. **Readability**
   - Map should be understandable at a glance
   - Clear visual distinction between tile types
   - Grid lines visible

2. **Fairness**
   - All shrines must be reachable
   - Optimal solution must exist
   - No "trick" or "gotcha" mechanics

3. **Escalation**
   - Each level introduces new complexity
   - Difficulty curve is smooth
   - Player skills grow with challenges

4. **Replayability**
   - Multiple valid solutions exist
   - Optimal path is intellectually satisfying
   - Rank system encourages perfection

---

### 7.5. Future Levels (DLC/Updates)

**Ideas for Expansion:**

**Level 4: "Tứ Tượng" (Four Symbols)**
- 4 regions, 4 shrines each (16 total)
- Must collect all in one region before moving to next
- Tests sectioning strategy

**Level 5: "Ngũ Hành" (Five Elements)**
- Shrines have colors (Wood, Fire, Earth, Metal, Water)
- Must collect in elemental cycle order
- Tests sequence memory

**Level 6: "Lục Hợp" (Six Harmonies)**
- Time pressure (optional challenge)
- Race against AI to find path faster
- Tests speed + accuracy

---

## 8. UI/UX DESIGN

### 8.1. Main Menu

**Layout:**
```
┌──────────────────────────────────┐
│                                  │
│      KỲ MÔN THẦN TỐC             │
│   (Title with gold effect)       │
│                                  │
│    >> NHẬP THẾ (Play)            │
│       RỜI KHỎI (Quit)            │
│                                  │
│  [Animated character in BG]      │
└──────────────────────────────────┘
```

**Interaction:**
- ↑/↓ to select
- Enter to confirm
- Visual feedback: Selected item glows cyan
- Audio: Menu beep on selection change

---

### 8.2. HUD (Heads-Up Display)

**Top Bar:**
```
┌─────────────────────────────────────────────┐
│ THIÊN MỆNH: 12 / 9   TRẬN NHÃN: 2 / 3      │
│ [U] HỒI TƯỞNG   [ESC] MENU                  │
└─────────────────────────────────────────────┘
```

**Elements:**
- **Thiên Mệnh (Destiny):** Current steps / Optimal steps
  - White if ≤ optimal
  - Red if > optimal
  
- **Trận Nhãn (Shrines):** Collected / Total
  - Cyan color (mystical)
  
- **Hints:** Key reminders
  - Gold text, subtle

**Design Principles:**
- Minimalist: Doesn't obstruct view
- Informative: All critical info visible
- Color-coded: Instant understanding
- Always visible: No toggle needed

---

### 8.3. Pause Menu

**Overlay:**
```
┌──────────────────────────────────┐
│       --- TẠM DỪNG ---           │
│                                  │
│   [ESC]  TIẾP TỤC                │
│   [M]    VỀ MENU                 │
│   [Q]    THOÁT GAME              │
│                                  │
│   (Game frozen in background)    │
└──────────────────────────────────┘
```

**Features:**
- Dark overlay (80% opacity)
- Game state preserved
- Quick resume
- No accidental quits (Q requires confirmation)

---

### 8.4. Win Screen

**Layout:**
```
┌──────────────────────────────────┐
│    THIÊN MỆNH HOÀN TẤT!          │
│                                  │
│    ĐÁNH GIÁ: TUYỆT ĐỈNH (S)      │
│    (Cyan glow effect)            │
│                                  │
│    Số bước: 9                    │
│    Tối ưu:  9                    │
│                                  │
│  [Enter]  MÀN TIẾP THEO >>       │
│  [R] Chơi Lai    [M] Menu        │
└──────────────────────────────────┘
```

**Feedback:**
- Rank S: Cyan color, particle effects, epic sound
- Rank A: Gold color, moderate effects
- Rank B: Silver color, simple effects

**Psychology:**
> Victory screen should feel REWARDING.
> Player should want to share screenshot.

---

### 8.5. Typography

**Font Family:** Roboto (Clean, Modern, Readable)

**Font Sizes (Logical Scale):**
```
Title:     120px (Main menu)
Heading:   80px  (Win screen)
Body:      50px  (HUD)
Small:     40px  (Hints)
```

**Font Colors:**
- Primary: White (#FFFFFF)
- Accent: Gold (#FFD700)
- Highlight: Cyan (#00FFFF)
- Warning: Red (#FF5050)
- Neutral: Gray (#AAAAAA)

---

### 8.6. Visual Effects

#### **Particle System:**
```
Shrine Collection:
- Spawn: 500 particles
- Color: Cyan (#00FFFF) + Gold (#FFD700)
- Lifetime: 1-2 seconds
- Behavior: Explode outward, fade, fall

Victory:
- Spawn: 200 particles (continuous)
- Color: Gold (#FFD700)
- Pattern: Fireworks
- Duration: 2 seconds
```

#### **Transitions:**
```
Fade In/Out:
- Duration: 0.5 seconds
- Easing: Linear
- Purpose: Smooth state changes

Screen Shake: (Not implemented yet)
- Use case: Collecting shrine
- Intensity: Subtle
```

---

### 8.7. Audio Design

#### **Music:**
```
Main Menu:   Ambient, mysterious
Gameplay:    Calm, focus-enhancing
Win Screen:  Triumphant, celebratory
```

**Volume Balance:**
- Music: 50% (non-intrusive)
- SFX: 100% (clear feedback)

#### **Sound Effects:**
```
step.wav     - Footstep (130ms)
collect.wav  - Shrine pickup (200ms)
win.wav      - Victory fanfare (1000ms)
menu_beep    - UI navigation (Future)
```

**Audio Philosophy:**
> Sound should enhance, not distract.
> Every action has audio feedback.

---

### 8.8. Accessibility

#### **Current:**
- ✅ Clear visual feedback
- ✅ Color-blind friendly (shapes + colors)
- ✅ Audio redundancy (SFX + visual)
- ✅ No time pressure

#### **Future Improvements:**
- [ ] Colorblind modes
- [ ] Adjustable font sizes
- [ ] Screen reader support
- [ ] High contrast mode

---

## 9. TECHNICAL SPECIFICATIONS

### 9.1. Technology Stack

**Programming Language:** C++17

**Libraries:**
- **SDL2** - Core framework (window, rendering, input)
- **SDL2_image** - Image loading (PNG, JPG)
- **SDL2_ttf** - Font rendering
- **SDL2_mixer** - Audio playback

**Build Tools:**
- **CMake** - Build configuration
- **MinGW-w64 GCC 15.2.0** - Compiler
- **Batch/PowerShell scripts** - Quick build

---

### 9.2. Architecture

**Design Patterns:**
```
Singleton:
- GameEngine (core)
- TextureManager (assets)
- SoundManager (audio)
- ThienCoEngine (AI)
- ParticleSystem (VFX)

State Machine:
- Menu → Play → Pause → Win

Observer:
- OnPlayerMove()
- OnShrineVisited()

Component:
- GameObject (base)
  └── Player (derived)
```

---

### 9.3. File Structure

```
GameKyMonThanToc/
├── assets/           # All game resources
│   ├── audio/        # BGM + SFX
│   ├── fonts/        # TTF files
│   ├── images/       # Sprites, tiles, backgrounds
│   └── levels/       # Level data (txt files)
├── include/          # Header files (.h)
├── src/              # Source files (.cpp)
├── build/            # Compiled executable
├── docs/             # Documentation (this file)
└── scripts/          # Build scripts
```

---

### 9.4. Performance Targets

**Frame Rate:** 60 FPS (VSync enabled)

**Load Times:**
- Game startup: < 2 seconds
- Level load: < 0.5 seconds
- AI calculation: < 0.1 seconds (for N ≤ 10)

**Memory Usage:**
- Runtime: < 100MB
- Assets: ~50MB

---

### 9.5. Platform Requirements

**Minimum:**
- OS: Windows 10 64-bit
- RAM: 2GB
- Storage: 100MB
- Display: 1280×720

**Recommended:**
- OS: Windows 11
- RAM: 4GB
- Storage: 200MB (with space for saves)
- Display: 1920×1080

---

## 10. CONCLUSION

This Game Design Document serves as the complete blueprint for **Kỳ Môn Thần Tốc**. It covers:

- ✅ **Story & Lore** - Rich narrative context
- ✅ **Characters** - Deep character development
- ✅ **Mechanics** - Complete gameplay systems
- ✅ **Controls** - Intuitive input design
- ✅ **Level Design** - Balanced challenge progression
- ✅ **UI/UX** - Professional interface design

**Document Status:** ✅ COMPLETE  
**Implementation Status:** ✅ PRODUCTION COMPLETE  
**Next Phase:** Testing & Iteration

---

**End of Game Design Document**

*"Mọi bước đi đều là một quyết định. Hãy chọn khôn ngoan."*
