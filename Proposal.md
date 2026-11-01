# Proposal — AI Plush Manta: 智慧鬼蝠魟陪伴與專注計時器

**Project type:** AIoT / Embedded AI / Interactive Plush Companion  
**Platform:** AMB82-Mini + Android AI Edge Gallery / Gemma agent  
**Status:** Design proposal and reference prototype

## 1. Abstract

本專題提出一款以鬼蝠魟（Manta Ray）為造型的 AI 互動玩偶。系統結合 AMB82-Mini 嵌入式控制板與 Android 裝置上的 AI 模型，讓使用者能以自然語言與玩偶互動，並透過燈光、聲音及受限的動作回饋情緒。

本專題的主要創新是 **Manta Focus Timer**。使用者可以用語音或手機介面設定倒數時間，Manta 會進入專注狀態，在時間結束時提供聲音與表情提醒。計時器由 AMB82-Mini 本地執行，不依賴 AI 持續運算，因此即使 AI 回應延遲，倒數仍可持續。

## 2. Motivation

一般 AI 聊天機器人主要透過螢幕與使用者互動，缺少實體存在感；一般計時器則只提供數字倒數，互動較為單一。本專題希望結合柔軟玩偶的陪伴感、AI 的自然語言理解，以及嵌入式系統的即時控制，打造兼具互動性與實用性的桌上型智慧夥伴。

鬼蝠魟具有寬大的胸鰭與流線外型，適合發展成療癒、友善的玩偶角色，也能將「展翼」的視覺概念延伸到專注模式與情緒燈效。

## 3. Objectives

1. 建立 Android AI host 與 AMB82-Mini 之間的 Wi-Fi 通訊。
2. 設計安全、精簡的 HTTP/JSON 裝置 API。
3. 提供視覺觀察、狀態查詢、聲音播放及情緒表達等互動能力。
4. 開發本地倒數計時器，支援開始、暫停、繼續、停止與剩餘時間查詢。
5. 讓使用者可透過自然語言或手機介面操作計時器。
6. 將隱私、安全、可測試性納入系統設計。

## 4. Proposed innovations

### 4.1 Manta Focus Timer — 核心創新

- 語音設定：「Manta，幫我計時 25 分鐘。」
- 手機介面設定時間與控制倒數。
- 支援暫停、繼續、取消及查詢剩餘時間。
- 計時期間切換成 focus 表情／燈效。
- 時間結束時播放提醒音並切換成 happy 表情。
- 倒數由 AMB82-Mini 本地時鐘執行，不因 AI 推理延遲而中斷。

### 4.2 Emotion-aware Companion

Manta 可依互動情境切換 happy、curious、thinking、sleepy、focus 等狀態，透過 LED 和預先定義的安全動作呈現回饋。

### 4.3 On-demand Vision

使用者詢問周遭物品時，AI 才請求一次影像觀察；不預設持續拍攝或上傳影像，以降低隱私風險與網路負載。

### 4.4 Dual interaction

同一組語意 API 同時支援 AI 語音互動與手機控制介面，避免兩套控制邏輯不一致。

### 4.5 Focus Flow — 專注與休息循環\n\n除了單次倒數，Manta 可提供可自訂的專注／休息循環。預設示範為 25 分鐘專注、5 分鐘休息，但使用者可自行調整。每個階段以不同燈效與提示音呈現；是否開始下一輪由使用者確認，避免玩偶自行連續打擾。\n\n### 4.6 Gentle Break Coach — 溫和休息提醒\n\n計時結束後，Manta 提醒使用者起身、喝水或短暫休息。這是一般生活提醒，不進行健康診斷，也不推測使用者的心理或身體狀態。\n\n### 4.7 Opt-in Focus Journal — 專注紀錄\n\n手機端可選擇記錄每日完成次數、專注總分鐘數與自訂目標。預設只保存計時統計，不保存語音、照片或對話內容；使用者可關閉或清除紀錄。\n\n### 4.8 Context-aware Interaction — 情境互動\n\n以使用者明確指令、按鍵或可選的 IMU／接近感測器事件切換 curious、happy、focus、resting 等狀態。狀態是互動模式，不宣稱能準確判斷人的真實情緒。\n\n## 5. User scenarios

**Scenario A — 專注計時**

1. 使用者說：「Manta，幫我計時 25 分鐘。」
2. Android AI 解析時間並呼叫 `timer_start(25)`。
3. AMB82-Mini 開始本地倒數，Manta 進入 focus 狀態。
4. 使用者可問：「還剩多久？」AI 呼叫 `timer_status()`。
5. 倒數歸零，Manta 播放提醒音並顯示完成表情。

**Scenario B — 暫停與繼續**

使用者說：「先暫停」或「繼續計時」，AI 分別呼叫 `timer_pause()` 或 `timer_resume()`。裝置回傳最新狀態供 AI 確認。

**Scenario C — 視覺互動**

使用者問：「我桌上有什麼？」AI 呼叫 `look()`，取得一次感測結果後再以自然語言回答。

**Scenario D — 專注與休息循環**\n\n使用者選擇 25/5 模式。專注結束時 Manta 提醒休息，詢問是否開始下一輪；只有使用者同意後才啟動下一次倒數。\n\n**Scenario E — 每日專注紀錄**\n\n使用者可在手機查看今日完成幾輪與累積專注分鐘數。此功能只需保存計時統計，無需保存媒體。\n\n## 6. System architecture

請參閱 [Architecture.md](Architecture.md)。整體採雙層架構：

- **Embedded layer:** AMB82-Mini、TimerEngine、Wi-Fi API、camera/peripheral adapters、LED/audio/motion control。
- **AI layer:** Android AI Edge Gallery、Gemma、Manta Skill、JavaScript bridge。

## 7. Hardware and software

| Item | Purpose |
|---|---|
| AMB82-Mini | 主控制器、Wi-Fi、相機／麥克風能力（依板型確認） |
| Camera | 按需影像擷取 |
| Speaker + compatible amplifier | 語音或提醒音效 |
| RGB LED | 情緒與專注狀態顯示 |
| Optional guarded actuator | 鰭部或身體的有限度動作 |
| Android phone/tablet | AI 推理與語音介面 |
| Arduino-compatible firmware | 裝置 API 與本地計時器 |
| JavaScript Skill bridge | Agent 指令驗證與 HTTP 呼叫 |

實際接線需依 AMB82-Mini 板型、電壓、電流與周邊模組規格確認；目前韌體中的相機、LED、伺服與音訊部分屬於硬體介接預留區。

## 8. Timer API

| Function | Input | Description |
|---|---|---|
| `timer_start(minutes)` | 1–180 | 開始倒數 |
| `timer_pause()` | — | 暫停 |
| `timer_resume()` | — | 繼續 |
| `timer_stop()` | — | 停止並清除 |
| `timer_status()` | — | 查詢狀態與剩餘秒數 |

Timer state: `idle → running ↔ paused → completed`.

本階段使用 AMB82 的單調計時來源，裝置斷電或重開機後不保留倒數；若未來需要跨斷電恢復，可再加入 RTC 或持久化截止時間。

## 9. Evaluation plan

- API 正確性：五種計時操作是否回傳正確狀態。
- 計時準確度：比較裝置倒數與標準時鐘的誤差。
- AI 操作成功率：測試不同自然語言說法能否正確轉成工具呼叫。
- 韌體可靠度：測試 Wi-Fi 中斷、重連及無效參數。
- 使用體驗：觀察使用者是否能理解專注中、暫停、休息及完成狀態。\n- 休息循環：確認專注結束後只提示，不會未經確認自動開始下一輪。\n- 紀錄隱私：確認只記錄選擇保存的計時統計，且可關閉／清除。
- 隱私與安全：確認沒有未經請求的持續影像／音訊擷取，並驗證動作限制。

## 10. Risks and mitigation

| Risk | Mitigation |
|---|---|
| AI 誤判時間 | 參數限制 1–180 分鐘；不清楚時先詢問 |
| AI 或 Wi-Fi 暫時離線 | 已啟動的倒數由本地 TimerEngine 繼續 |
| 計時器因阻塞程式碼停頓 | 使用非阻塞時間差計算，不使用長時間 delay |
| 周邊硬體尚未確定 | 先完成 API 與模擬測試，再依板型接入 |
| 相機／麥克風隱私 | 按需啟用、狀態指示、不預設儲存 |
| 動作造成機構風險 | 限制動作範圍、速度與時間，並提供停止機制 |

## 11. Development milestones

1. **M1 — Device foundation:** Wi-Fi、HTTP API、status。
2. **M2 — Timer engine:** start/pause/resume/stop/status，完成邊界測試。
3. **M3 — Agent integration:** Manta Skill 與 JavaScript bridge。
4. **M4 — Feedback:** LED、聲音與完成提醒硬體介接。
5. **M5 — Perception:** 相機按需觀察與感測器整合。
6. **M6 — Demonstration:** 完整語音計時流程、測試紀錄與實體成品展示。

## 12. Expected outcome

預期完成一款具備 AI 對話、基本環境觀察、情緒回饋及智慧計時功能的 Manta 互動玩偶原型。其差異化不只在於鬼蝠魟造型，而是新增一套由嵌入式裝置獨立執行、可由 AI 自然語言控制的專注計時服務。
