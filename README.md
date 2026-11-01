# AI Plush Manta — 智慧鬼蝠魟陪伴與專注計時器

> A gentle AI manta-ray companion with vision, expressive feedback, and a built-in focus timer.

AI Plush Manta 將 **AMB82-Mini 實體控制器**與 Android 裝置上的 **Google AI Edge Gallery / Gemma agent**結合。Manta 不只可以透過 AI 互動，也能協助使用者安排專注時間：設定倒數、暫停、繼續、查詢剩餘時間，並在結束時以燈光與聲音提醒。

## Product concept

![AI Plush Manta product concept](assets/manta_product_concept.png)

*AI-generated concept rendering. The image illustrates the intended product direction; it is not a photograph of a completed physical prototype.*

## Highlights

- **AI companion:** natural-language conversation through the Android AI host.
- **Visual perception:** request a camera observation only when it is useful.
- **Manta expressions:** bounded LED patterns and optional gentle fin/body gestures.
- **Focus timer:** start, pause, resume, stop, and query a countdown.
- **Completion notification:** a sound and happy/celebration expression when the timer reaches zero.
- **Privacy-first behavior:** no continuous image/audio upload by default; camera and microphone activity should be visibly indicated.

## System overview

![Manta system architecture](assets/manta_architecture.svg)

| Layer | Responsibility |
|---|---|
| AMB82-Mini | Wi-Fi, device API, timer state, camera/peripheral adapters, LEDs/audio and bounded motion |
| Android AI host | Gemma inference, speech interaction, intent understanding and agent orchestration |
| Skill bridge | Validates semantic commands and translates them into HTTP requests |
| User | Speaks naturally or uses the phone interface |

See [Architecture.md](Architecture.md) for component details and API contracts.

## Timer feature

The timer is designed as a deterministic device feature rather than an AI-generated delay. The AI interprets a request such as “Manta，幫我計時 25 分鐘” and calls a semantic timer action. The AMB82-Mini owns the countdown using a monotonic clock, so model latency does not change the remaining time.

| Agent action | Device endpoint | Description |
|---|---|---|
| `timer_start` | `GET /timer/start?minutes=25` | Start a 1–180 minute countdown |
| `timer_pause` | `GET /timer/pause` | Pause the countdown |
| `timer_resume` | `GET /timer/resume` | Continue a paused countdown |
| `timer_stop` | `GET /timer/stop` | Clear the current timer |
| `timer_status` | `GET /timer/status` | Read remaining seconds and state |

The phone UI can call the same bridge actions. This keeps voice and touch control consistent.

## Hardware and software

### Reference hardware
- Realtek AMB82-Mini development board (camera and microphone capability; confirm board revision and peripheral support).
- Speaker and compatible audio amplifier (the original reference lists PAM8403 + speaker).
- Optional RGB LED and mechanically guarded, low-range actuator for expressive feedback.
- Android phone/tablet as the AI inference host.

### Software
- Arduino-style AMB82 firmware (hardware adapters may need board-specific implementation).
- Google AI Edge Gallery / compatible Android agent host.
- JavaScript Skill bridge in `scripts/index.html`.

> This repository contains a reference implementation. Camera inference, servo movement, LED patterns, and audio playback include hardware-adapter points that must be connected to the exact modules and board package used in the prototype.

## Quick start

1. Review [Architecture.md](Architecture.md) and confirm the board revision and peripherals.
2. Open `MantaAgent.ino` in the supported AMB82 Arduino environment.
3. Set Wi-Fi credentials locally. Do not commit real credentials.
4. Flash the board and note its LAN IP address.
5. Set `MANTA_BASE_URL` in `scripts/index.html` to that address.
6. Test `/status`, then test the timer endpoints.
7. Import the Manta skill instructions from [SKILL.md](SKILL.md) into the supported AI Edge Gallery skill/tool workflow.

## Example conversation

- User:「Manta，幫我計時 25 分鐘。」
- Agent: calls `timer_start(25)`.
- Manta: enters focus expression; the local timer starts.
- User:「還剩多久？」
- Agent: calls `timer_status()` and reports the returned remaining time.
- Timer completes: firmware triggers the configured completion sound and happy expression.

## Repository map

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
│   └── manta_timer_flow.svg
└── scripts/
    └── index.html
```

## Development status

- [x] Manta concept, system architecture, and timer API specification
- [x] Reference firmware timer state machine
- [x] Skill/bridge timer commands
- [ ] Verify board-specific camera, LED, servo, and audio adapters on hardware
- [ ] Validate AI Edge Gallery skill invocation on the selected app version
- [ ] Photograph the assembled prototype and replace the concept rendering with a real build photo

## Safety and privacy

Use protected power hardware and a physically secure enclosure. Keep moving parts inaccessible, constrain actuator travel in firmware, and provide a physical power switch plus camera/microphone disable controls where possible. Avoid storing or transmitting media unless the user explicitly requests it.
