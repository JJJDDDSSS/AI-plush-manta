# AI Plush Manta｜智慧鬼蝠魟玩偶與專注計時夥伴

> A soft manta-ray AI companion with on-device interaction, expressive feedback, and a local focus timer.

**AI Plush Manta** 是一款以鬼蝠魟（Manta Ray）為造型的 AI 毛絨玩偶。實體端以 AMB82-Mini 作為控制核心，Android 手機／平板執行 AI Edge Gallery 與 Gemma agent。使用者可透過語音或手機與 Manta 互動，設定專注倒數、暫停／繼續、查詢剩餘時間，並透過燈光與聲音獲得回饋。

## Manta 玩偶產品概念

![AI Plush Manta 毛絨玩偶、AMB82-Mini 系統、Android AI 與 Focus Timer 產品概念圖](https://github.com/user-attachments/assets/3bfc3444-8beb-4a65-b691-d10612705661)

*產品概念圖：以藍白配色的鬼蝠魟毛絨玩偶為主角，整合 AMB82-Mini、攝影機、麥克風、Android AI 與 Focus Timer。*

## 完整系統總覽

![AI Manta 系統總覽圖](assets/manta_system_overview.svg)

[直接開啟系統總覽圖](assets/manta_system_overview.svg) · [硬體與軟體架構](Architecture.md) · [專題企劃書](Proposal.md)

## 專題特色

- **Manta 實體角色：** 鬼蝠魟寬大的胸鰭、深藍背部與白色腹部，作為玩偶造型及互動識別。
- **AI 對話：** 透過 Android AI host 與 Gemma agent 理解自然語言。
- **按需視覺：** 只有在使用者提出需求或明確觸發時才進行單次影像觀察。
- **角色狀態：** happy、curious、thinking、focus、resting、sleepy 等狀態可映射到 LED、音效與可選的安全動作。
- **Manta Focus Timer：** 本地倒數、暫停、繼續、停止與剩餘時間查詢。
- **Focus Flow：** 可調整專注／休息循環；每輪結束先詢問使用者，不會自動連續啟動。
- **隱私優先：** 不預設持續錄音、拍攝或上傳媒體；專注紀錄採選擇加入。

## 創新功能

### 1. Manta Focus Timer
語音或手機設定 1–180 分鐘倒數。AMB82-Mini 使用本地單調計時來源執行倒數，AI 推理延遲或 Wi-Fi 暫時中斷不會停止已啟動的計時。

### 2. Focus Flow
提供可自訂的專注／休息循環（示範預設 25 分鐘專注＋5 分鐘休息）。專注結束後 Manta 提示休息，是否進入下一輪由使用者確認。

### 3. Manta Companion States
以有限且明確的角色狀態控制表情、燈效與聲音。這些狀態是互動設計，不代表系統能準確判讀人的真實情緒。

### 4. On-demand Vision
使用者提出視覺問題時才擷取單次影像；不預設持續監看或自動保存媒體。

### 5. Gentle Break Coach & Focus Journal
計時結束時提供溫和休息提醒。手機端可選擇保存完成次數與專注總分鐘數，不需要保存照片、錄音或完整對話。

## 系統分層

| 層級 | 職責 |
|---|---|
| Manta Plush | 毛絨外殼、角色外觀、感測器與回饋模組的實體整合 |
| AMB82-Mini | Wi-Fi、HTTP API、本地 TimerEngine、感測器／周邊介接 |
| Android AI Host | Gemma 推理、語音互動、意圖理解與 agent orchestration |
| Manta Skill／JS Bridge | 驗證語意指令並呼叫裝置 API |
| Phone UI | 計時控制、狀態顯示與選擇性專注統計 |

## Timer API

| Agent action | Device endpoint | 說明 |
|---|---|---|
| `timer_start(minutes)` | `/timer/start?minutes=N` | 開始 1–180 分鐘倒數 |
| `timer_pause()` | `/timer/pause` | 暫停 |
| `timer_resume()` | `/timer/resume` | 繼續 |
| `timer_stop()` | `/timer/stop` | 停止並清除 |
| `timer_status()` | `/timer/status` | 查詢狀態與剩餘秒數 |

計時狀態：`idle → running ↔ paused → completed`。目前參考韌體在裝置重新啟動或斷電後不保留倒數。

## 硬體與軟體

**參考硬體**
- AMB82-Mini（RTL8735B；需確認實際板型與周邊支援）
- Camera、Microphone（依實際模組與音訊路徑整合）
- Speaker／amplifier、RGB LED
- 可選的安全限位動作機構
- Android 手機／平板作為 AI host

**軟體**
- `MantaAgent.ino`：裝置 API 與本地計時器參考韌體
- `SKILL.md`：Manta agent 操作規則
- `scripts/index.html`：JavaScript Skill bridge
- `Architecture.md`：架構、API 與安全規格
- `Proposal.md`：完整專題企劃書

> **實作狀態說明：** 本 Repository 包含參考韌體與介接規格。相機辨識、LED、伺服動作、音訊播放仍需依實際 AMB82-Mini 板型及周邊完成整合與實機驗證；目前不可視為所有硬體功能都已測試完成。

## Quick start

1. 閱讀 [Architecture.md](Architecture.md)，確認 AMB82-Mini 板型與周邊規格。
2. 在相容的 AMB82 Arduino 開發環境開啟 `MantaAgent.ino`。
3. 在本機設定 Wi-Fi 資訊，**不要把真實密碼提交到 GitHub**。
4. 燒錄後確認裝置 LAN IP，並先測試 `/status`。
5. 在 `scripts/index.html` 設定 `MANTA_BASE_URL`，再測試 Timer API。
6. 依實際 AI Edge Gallery 版本匯入 [SKILL.md](SKILL.md) 的工具規格。

## 專案結構

```text
.
├── README.md
├── Architecture.md
├── Proposal.md
├── PROPOSAL.md
├── MantaAgent.ino
├── SKILL.md
├── assets/
│   ├── manta_architecture.svg
│   ├── manta_product_concept.svg
│   ├── manta_system_overview.svg
│   └── manta_timer_flow.svg
└── scripts/
    └── index.html
```

## 開發狀態

- [x] Manta 角色設定與產品概念
- [x] Manta 系統架構、Timer 流程與 API 規格
- [x] Focus Timer 參考韌體狀態機
- [x] Manta Skill 與 JavaScript bridge
- [x] Focus Flow、角色狀態、按需視覺與休息提醒提案
- [ ] 依實際板型完成 camera／LED／servo／audio 介接
- [ ] 在指定 AI Edge Gallery 版本驗證 Skill 呼叫
- [ ] 完成硬體計時誤差、Wi-Fi 中斷與功耗測試
- [ ] 製作並拍攝實體 Manta 玩偶原型

## 安全與隱私

使用安全合規的供電模組與固定外殼；可動部件必須有機構限位、逾時停止及實體斷電方式。相機與麥克風應有明確啟用指示與停用方式。未經使用者要求，不持續擷取、儲存或傳輸影像與音訊。
