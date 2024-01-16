/*
 * DISCLAIMER:
 * Code from this file is HEAVILY based on Doppleganger's disassembly of Super
 * Mario Bros., without their hard work this would not be possible.
 * 
 * Original disassembly can be found here:
 * https://gist.github.com/1wErt3r/4048722
 */

#include "smb1.h"

#include "apu.h"
#include "cpu.h"
#include "ppu.h"

#include <stdint.h>

#include <stdio.h>

uint8_t memory[0x800];

void Start(void);
const uint8_t *VRAM_AddrTable[];
uint8_t VRAM_Buffer_Offset[];
void NonMaskableInterrupt(void);
void PauseRoutine(void);
void SpriteShuffler(void);
void OperModeExecutionTree(void);
void MoveAllSpritesOffscreen(void);
void MoveSpritesOffscreen(void);
void TitleScreenMode(void);
void GameMenuRoutine(void);
void VictoryMode(void);
void ScreenRoutines(void);
const uint8_t WaterPaletteData[];
const uint8_t GroundPaletteData[];
const uint8_t UndergroundPaletteData[];
const uint8_t CastlePaletteData[];
const uint8_t DaySnowPaletteData[];
const uint8_t NightSnowPaletteData[];
const uint8_t MushroomPaletteData[];
const uint8_t BowserPaletteData[];
const uint8_t MarioThanksMessage[];
const uint8_t LuigiThanksMessage[];
const uint8_t MushroomRetainerSaved[];
const uint8_t PrincessSaved1[];
const uint8_t PrincessSaved2[];
const uint8_t WorldSelectMessage1[];
const uint8_t WorldSelectMessage2[];
void InitializeNameTables(void);
void WriteNTAddr(uint8_t a);
void ReadJoypads(void);
void ReadPortBits(uint8_t x);
void UpdateScreen(const uint8_t *address);
void InitScroll(uint8_t a);
void WritePPUReg1(uint8_t a);
void UpdateTopScore(void);
void TopScoreCheck(uint8_t x);
void InitializeGame(void);
void PrimaryGameSetup(void);
uint8_t InitializeMemory(uint8_t y);
void GameOverMode(void);
void GameMode(void);
void SoundEngine(void);
void Dump_Squ1_Regs(uint8_t x, uint8_t y);
bool PlaySqu1Sfx(uint8_t a, uint8_t x, uint8_t y);
void SetFreq_Squ1(uint8_t *a, uint8_t *x, uint8_t *y, bool *z);
void Dump_Freq_Regs(uint8_t *a, uint8_t *x, uint8_t *y, bool *z);
void SetFreq_Squ2(uint8_t *a, uint8_t *x, uint8_t *y, bool *z);
const uint8_t SwimStompEnvelopeData[0xe];
void Square1SfxHandler(void);
void StopSquare1Sfx(void);
void Square2SfxHandler(void);
void StopSquare2Sfx(void);
void NoiseSfxHandler(void);
void MusicHandler(void);
void AlternateLengthHandler(uint8_t *a, uint8_t *x, bool c);
uint8_t ProcessLengthData(uint8_t a);
void LoadControlRegs(uint8_t *a, uint8_t *x, uint8_t *y);
uint8_t LoadEnvelopeData(uint8_t y);
const uint8_t MusicHeaderData[0xab];
#define TimeRunningOutHdr (MusicHeaderData + 0x31)
#define Star_CloudHdr (MusicHeaderData + 0x36)
#define EndOfLevelMusHdr (MusicHeaderData + 0x3c)
#define UndergroundMusHdr (MusicHeaderData + 0x46)
#define SilenceHdr (MusicHeaderData + 0x4b)
#define CastleMusHdr (MusicHeaderData + 0x4f)
#define VictoryMusHdr (MusicHeaderData + 0x54)
#define GameOverMusHdr (MusicHeaderData + 0x59)
#define WaterMusHdr (MusicHeaderData + 0x5e)
#define WinCastleMusHdr (MusicHeaderData + 0x64)
#define GroundLevelPart1Hdr (MusicHeaderData + 0x69)
#define GroundLevelPart2AHdr (MusicHeaderData + 0x6f)
#define GroundLevelPart2BHdr (MusicHeaderData + 0x75)
#define GroundLevelPart2CHdr (MusicHeaderData + 0x7b)
#define GroundLevelPart3AHdr (MusicHeaderData + 0x81)
#define GroundLevelPart3BHdr (MusicHeaderData + 0x87)
#define GroundLevelLeadInHdr (MusicHeaderData + 0x8d)
#define GroundLevelPart4AHdr (MusicHeaderData + 0x93)
#define GroundLevelPart4BHdr (MusicHeaderData + 0x99)
#define GroundLevelPart4CHdr (MusicHeaderData + 0x9f)
#define DeathMusHdr (MusicHeaderData + 0xa5)
const uint8_t Star_CloudMData[0x49];
const uint8_t GroundM_P1Data[0xc1];
#define SilenceData (GroundM_P1Data + 0x1b)
#define GroundM_P2AData (GroundM_P1Data + 0x48)
#define GroundM_P2BData (GroundM_P1Data + 0x74)
#define GroundM_P2CData (GroundM_P1Data + 0x9c)
const uint8_t GroundM_P3AData[0x63];
#define GroundM_P3BData (GroundM_P3AData + 0x19)
#define GroundMLdInData (GroundM_P3AData + 0x37)
const uint8_t GroundM_P4AData[0x7f];
#define GroundM_P4BData (GroundM_P4AData + 0x26)
#define DeathMusData (GroundM_P4AData + 0x4d)
#define GroundM_P4CData (GroundM_P4AData + 0x4f)
const uint8_t CastleMusData[0xa1];
const uint8_t GameOverMusData[0x2d];
const uint8_t TimeRunOutMusData[0x3e];
const uint8_t WinLevelMusData[0x61];
const uint8_t UndergroundMusData[0x41];
const uint8_t WaterMusData[0xff];
const uint8_t EndOfCastleMusData[0x77];
const uint8_t VictoryMusData[0x35];
const uint8_t FreqRegLookupTbl[0x66];
const uint8_t MusicLengthLookupTbl[0x30];
const uint8_t EndOfCastleMusicEnvData[0x4];
const uint8_t AreaMusicEnvData[0x8];
const uint8_t WaterEventMusEnvData[0x28];
const uint8_t BowserFlameEnvData[0x20];
const uint8_t BrickShatterEnvData[0x10];

// GAME SPECIFIC DEFINES

uint8_t *ObjectOffset           = memory + 0x08;

uint8_t *FrameCounter           = memory + 0x09;

uint8_t *SavedJoypadBits        = memory + 0x06fc;
uint8_t *SavedJoypad1Bits       = memory + 0x06fc;
uint8_t *SavedJoypad2Bits       = memory + 0x06fd;
uint8_t *JoypadBitMask          = memory + 0x074a;
uint8_t *JoypadOverride         = memory + 0x758;

uint8_t *A_B_Buttons            = memory + 0x0a;
uint8_t *PreviousA_B_Buttons    = memory + 0x0d;
uint8_t *Up_Down_Buttons        = memory + 0x0b;
uint8_t *Left_Right_Buttons     = memory + 0x0c;

uint8_t *GameEngineSubroutine   = memory + 0x0e;

uint8_t *Mirror_PPU_CTRL_REG1   = memory + 0x0778;
uint8_t *Mirror_PPU_CTRL_REG2   = memory + 0x0779;

uint8_t *OperMode               = memory + 0x0770;
uint8_t *OperMode_Task          = memory + 0x0772;
uint8_t *ScreenRoutineTask      = memory + 0x073c;

uint8_t *GamePauseStatus        = memory + 0x0776;
uint8_t *GamePauseTimer         = memory + 0x0777;

uint8_t *DemoAction             = memory + 0x0717;
uint8_t *DemoActionTimer        = memory + 0x0718;

uint8_t *TimerControl           = memory + 0x0747;
uint8_t *IntervalTimerControl   = memory + 0x077f;

uint8_t *Timers                 = memory + 0x0780;
uint8_t *SelectTimer            = memory + 0x0780;
uint8_t *PlayerAnimTimer        = memory + 0x0781;
uint8_t *JumpSwimTimer          = memory + 0x0782;
uint8_t *RunningTimer           = memory + 0x0783;
uint8_t *BlockBounceTimer       = memory + 0x0784;
uint8_t *SideCollisionTimer     = memory + 0x0785;
uint8_t *JumpspringTimer        = memory + 0x0786;
uint8_t *GameTimerCtrlTimer     = memory + 0x0787;
uint8_t *ClimbSideTimer         = memory + 0x0789;
uint8_t *EnemyFrameTimer        = memory + 0x078a;
uint8_t *FrenzyEnemyTimer       = memory + 0x078f;
uint8_t *BowserFireBreathTimer  = memory + 0x0790;
uint8_t *StompTimer             = memory + 0x0791;
uint8_t *AirBubbleTimer         = memory + 0x0792;
uint8_t *ScrollIntervalTimer    = memory + 0x0795;
uint8_t *EnemyIntervalTimer     = memory + 0x0796;
uint8_t *BrickCoinTimer         = memory + 0x079d;
uint8_t *InjuryTimer            = memory + 0x079e;
uint8_t *StarInvincibleTimer    = memory + 0x079f;
uint8_t *ScreenTimer            = memory + 0x07a0;
uint8_t *WorldEndTimer          = memory + 0x07a1;
uint8_t *DemoTimer              = memory + 0x07a2;

uint8_t *Sprite_Data            = memory + 0x0200;

uint8_t *Sprite_Y_Position      = memory + 0x0200;
uint8_t *Sprite_Tilenumber      = memory + 0x0201;
uint8_t *Sprite_Attributes      = memory + 0x0202;
uint8_t *Sprite_X_Position      = memory + 0x0203;

uint8_t *ScreenEdge_PageLoc     = memory + 0x071a;
uint8_t *ScreenEdge_X_Pos       = memory + 0x071c;
uint8_t *ScreenLeft_PageLoc     = memory + 0x071a;
uint8_t *ScreenRight_PageLoc    = memory + 0x071b;
uint8_t *ScreenLeft_X_Pos       = memory + 0x071c;
uint8_t *ScreenRight_X_Pos      = memory + 0x071d;

uint8_t *PlayerFacingDir        = memory + 0x33;
uint8_t *DestinationPageLoc     = memory + 0x34;
uint8_t *VictoryWalkControl     = memory + 0x35;
uint8_t *ScrollFractional       = memory + 0x0768;
uint8_t *PrimaryMsgCounter      = memory + 0x0719;
uint8_t *SecondaryMsgCounter    = memory + 0x0749;

uint8_t *HorizontalScroll       = memory + 0x073f;
uint8_t *VerticalScroll         = memory + 0x0740;
uint8_t *ScrollLock             = memory + 0x0723;
uint8_t *ScrollThirtyTwo        = memory + 0x073d;
uint8_t *Player_X_Scroll        = memory + 0x06ff;
uint8_t *Player_Pos_ForScroll   = memory + 0x0755;
uint8_t *ScrollAmount           = memory + 0x0775;

uint8_t *AreaData               = memory + 0xe7;
uint8_t *AreaDataLow            = memory + 0xe7;
uint8_t *AreaDataHigh           = memory + 0xe8;
uint8_t *EnemyData              = memory + 0xe9;
uint8_t *EnemyDataLow           = memory + 0xe9;
uint8_t *EnemyDataHigh          = memory + 0xea;

uint8_t *AreaParserTaskNum      = memory + 0x071f;
uint8_t *ColumnSets             = memory + 0x071e;
uint8_t *CurrentPageLoc         = memory + 0x0725;
uint8_t *CurrentColumnPos       = memory + 0x0726;
uint8_t *BackloadingFlag        = memory + 0x0728;
uint8_t *BehindAreaParserFlag   = memory + 0x0729;
uint8_t *AreaObjectPageLoc      = memory + 0x072a;
uint8_t *AreaObjectPageSel      = memory + 0x072b;
uint8_t *AreaDataOffset         = memory + 0x072c;
uint8_t *AreaObjOffsetBuffer    = memory + 0x072d;
uint8_t *AreaObjectLength       = memory + 0x0730;
uint8_t *StaircaseControl       = memory + 0x0734;
uint8_t *AreaObjectHeight       = memory + 0x0735;
uint8_t *MushroomLedgeHalfLen   = memory + 0x0736;
uint8_t *EnemyDataOffset        = memory + 0x0739;
uint8_t *EnemyObjectPageLoc     = memory + 0x073a;
uint8_t *EnemyObjectPageSel     = memory + 0x073b;
uint8_t *MetatileBuffer         = memory + 0x06a1;
uint8_t *BlockBufferColumnPos   = memory + 0x06a0;
uint8_t *CurrentNTAddr_Low      = memory + 0x0721;
uint8_t *CurrentNTAddr_High     = memory + 0x0720;
uint8_t *AttributeBuffer        = memory + 0x03f9;

uint8_t *LoopCommand            = memory + 0x0745;

uint8_t *DisplayDigits          = memory + 0x07d7;
uint8_t *TopScoreDisplay        = memory + 0x07d7;
uint8_t *ScoreAndCoinDisplay    = memory + 0x07dd;
uint8_t *PlayerScoreDisplay     = memory + 0x07dd;
uint8_t *GameTimerDisplay       = memory + 0x07f8;
uint8_t *DigitModifier          = memory + 0x0134;

uint8_t *VerticalFlipFlag       = memory + 0x0109;
uint8_t *FloateyNum_Control     = memory + 0x0110;
uint8_t *ShellChainCounter      = memory + 0x0125;
uint8_t *FloateyNum_Timer       = memory + 0x012c;
uint8_t *FloateyNum_X_Pos       = memory + 0x0117;
uint8_t *FloateyNum_Y_Pos       = memory + 0x011e;
uint8_t *FlagpoleFNum_Y_Pos     = memory + 0x010d;
uint8_t *FlagpoleFNum_YMFDummy  = memory + 0x010e;
uint8_t *FlagpoleScore          = memory + 0x010f;
uint8_t *FlagpoleCollisionYPos  = memory + 0x070f;
uint8_t *StompChainCounter      = memory + 0x0484;

uint8_t *VRAM_Buffer1_Offset    = memory + 0x0300;
uint8_t *VRAM_Buffer1           = memory + 0x0301;
uint8_t *VRAM_Buffer2_Offset    = memory + 0x0340;
uint8_t *VRAM_Buffer2           = memory + 0x0341;
uint8_t *VRAM_Buffer_AddrCtrl   = memory + 0x0773;
uint8_t *Sprite0HitDetectFlag   = memory + 0x0722;
uint8_t *DisableScreenFlag      = memory + 0x0774;
uint8_t *DisableIntermediate    = memory + 0x0769;
uint8_t *ColorRotateOffset      = memory + 0x06d4;

uint8_t *TerrainControl         = memory + 0x0727;
uint8_t *AreaStyle              = memory + 0x0733;
uint8_t *ForegroundScenery      = memory + 0x0741;
uint8_t *BackgroundScenery      = memory + 0x0742;
uint8_t *CloudTypeOverride      = memory + 0x0743;
uint8_t *BackgroundColorCtrl    = memory + 0x0744;
uint8_t *AreaType               = memory + 0x074e;
uint8_t *AreaAddrsLOffset       = memory + 0x074f;
uint8_t *AreaPointer            = memory + 0x0750;

uint8_t *PlayerEntranceCtrl     = memory + 0x0710;
uint8_t *GameTimerSetting       = memory + 0x0715;
uint8_t *AltEntranceControl     = memory + 0x0752;
uint8_t *EntrancePage           = memory + 0x0751;
uint8_t *NumberOfPlayers        = memory + 0x077a;
uint8_t *WarpZoneControl        = memory + 0x06d6;
uint8_t *ChangeAreaTimer        = memory + 0x06de;

uint8_t *MultiLoopCorrectCntr   = memory + 0x06d9;
uint8_t *MultiLoopPassCntr      = memory + 0x06da;

uint8_t *FetchNewGameTimerFlag  = memory + 0x0757;
uint8_t *GameTimerExpiredFlag   = memory + 0x0759;

uint8_t *PrimaryHardMode        = memory + 0x076a;
uint8_t *SecondaryHardMode      = memory + 0x06cc;
uint8_t *WorldSelectNumber      = memory + 0x076b;
uint8_t *WorldSelectEnableFlag  = memory + 0x07fc;
uint8_t *ContinueWorld          = memory + 0x07fd;

uint8_t *CurrentPlayer          = memory + 0x0753;
uint8_t *PlayerSize             = memory + 0x0754;
uint8_t *PlayerStatus           = memory + 0x0756;

uint8_t *OnscreenPlayerInfo     = memory + 0x075a;
uint8_t *NumberofLives          = memory + 0x075a; //used by current player
uint8_t *HalfwayPage            = memory + 0x075b;
uint8_t *LevelNumber            = memory + 0x075c; //the actual dash number
uint8_t *Hidden1UpFlag          = memory + 0x075d;
uint8_t *CoinTally              = memory + 0x075e;
uint8_t *WorldNumber            = memory + 0x075f;
uint8_t *AreaNumber             = memory + 0x0760; //internal number used to find areas

uint8_t *CoinTallyFor1Ups       = memory + 0x0748;

uint8_t *OffscreenPlayerInfo    = memory + 0x0761;
uint8_t *OffScr_NumberofLives   = memory + 0x0761; //used by offscreen player
uint8_t *OffScr_HalfwayPage     = memory + 0x0762;
uint8_t *OffScr_LevelNumber     = memory + 0x0763;
uint8_t *OffScr_Hidden1UpFlag   = memory + 0x0764;
uint8_t *OffScr_CoinTally       = memory + 0x0765;
uint8_t *OffScr_WorldNumber     = memory + 0x0766;
uint8_t *OffScr_AreaNumber      = memory + 0x0767;

uint8_t *BalPlatformAlignment   = memory + 0x03a0;
uint8_t *Platform_X_Scroll      = memory + 0x03a1;
uint8_t *PlatformCollisionFlag  = memory + 0x03a2;
uint8_t *YPlatformTopYPos       = memory + 0x0401;
uint8_t *YPlatformCenterYPos    = memory + 0x58;

uint8_t *BrickCoinTimerFlag     = memory + 0x06bc;
uint8_t *StarFlagTaskControl    = memory + 0x0746;

uint8_t *PseudoRandomBitReg     = memory + 0x07a7;
uint8_t *WarmBootValidation     = memory + 0x07ff;

uint8_t *SprShuffleAmtOffset    = memory + 0x06e0;
uint8_t *SprShuffleAmt          = memory + 0x06e1;
uint8_t *SprDataOffset          = memory + 0x06e4;
uint8_t *Player_SprDataOffset   = memory + 0x06e4;
uint8_t *Enemy_SprDataOffset    = memory + 0x06e5;
uint8_t *Block_SprDataOffset    = memory + 0x06ec;
uint8_t *Alt_SprDataOffset      = memory + 0x06ec;
uint8_t *Bubble_SprDataOffset   = memory + 0x06ee;
uint8_t *FBall_SprDataOffset    = memory + 0x06f1;
uint8_t *Misc_SprDataOffset     = memory + 0x06f3;
uint8_t *SprDataOffset_Ctrl     = memory + 0x03ee;

uint8_t *Player_State           = memory + 0x1d;
uint8_t *Enemy_State            = memory + 0x1e;
uint8_t *Fireball_State         = memory + 0x24;
uint8_t *Block_State            = memory + 0x26;
uint8_t *Misc_State             = memory + 0x2a;

uint8_t *Player_MovingDir       = memory + 0x45;
uint8_t *Enemy_MovingDir        = memory + 0x46;

uint8_t *SprObject_X_Speed      = memory + 0x57;
uint8_t *Player_X_Speed         = memory + 0x57;
uint8_t *Enemy_X_Speed          = memory + 0x58;
uint8_t *Fireball_X_Speed       = memory + 0x5e;
uint8_t *Block_X_Speed          = memory + 0x60;
uint8_t *Misc_X_Speed           = memory + 0x64;

uint8_t *Jumpspring_FixedYPos   = memory + 0x58;
uint8_t *JumpspringAnimCtrl     = memory + 0x070e;
uint8_t *JumpspringForce        = memory + 0x06db;

uint8_t *SprObject_PageLoc      = memory + 0x6d;
uint8_t *Player_PageLoc         = memory + 0x6d;
uint8_t *Enemy_PageLoc          = memory + 0x6e;
uint8_t *Fireball_PageLoc       = memory + 0x74;
uint8_t *Block_PageLoc          = memory + 0x76;
uint8_t *Misc_PageLoc           = memory + 0x7a;
uint8_t *Bubble_PageLoc         = memory + 0x83;

uint8_t *SprObject_X_Position   = memory + 0x86;
uint8_t *Player_X_Position      = memory + 0x86;
uint8_t *Enemy_X_Position       = memory + 0x87;
uint8_t *Fireball_X_Position    = memory + 0x8d;
uint8_t *Block_X_Position       = memory + 0x8f;
uint8_t *Misc_X_Position        = memory + 0x93;
uint8_t *Bubble_X_Position      = memory + 0x9c;

uint8_t *SprObject_Y_Speed      = memory + 0x9f;
uint8_t *Player_Y_Speed         = memory + 0x9f;
uint8_t *Enemy_Y_Speed          = memory + 0xa0;
uint8_t *Fireball_Y_Speed       = memory + 0xa6;
uint8_t *Block_Y_Speed          = memory + 0xa8;
uint8_t *Misc_Y_Speed           = memory + 0xac;

uint8_t *SprObject_Y_HighPos    = memory + 0xb5;
uint8_t *Player_Y_HighPos       = memory + 0xb5;
uint8_t *Enemy_Y_HighPos        = memory + 0xb6;
uint8_t *Fireball_Y_HighPos     = memory + 0xbc;
uint8_t *Block_Y_HighPos        = memory + 0xbe;
uint8_t *Misc_Y_HighPos         = memory + 0xc2;
uint8_t *Bubble_Y_HighPos       = memory + 0xcb;

uint8_t *SprObject_Y_Position   = memory + 0xce;
uint8_t *Player_Y_Position      = memory + 0xce;
uint8_t *Enemy_Y_Position       = memory + 0xcf;
uint8_t *Fireball_Y_Position    = memory + 0xd5;
uint8_t *Block_Y_Position       = memory + 0xd7;
uint8_t *Misc_Y_Position        = memory + 0xdb;
uint8_t *Bubble_Y_Position      = memory + 0xe4;

uint8_t *SprObject_Rel_XPos     = memory + 0x03ad;
uint8_t *Player_Rel_XPos        = memory + 0x03ad;
uint8_t *Enemy_Rel_XPos         = memory + 0x03ae;
uint8_t *Fireball_Rel_XPos      = memory + 0x03af;
uint8_t *Bubble_Rel_XPos        = memory + 0x03b0;
uint8_t *Block_Rel_XPos         = memory + 0x03b1;
uint8_t *Misc_Rel_XPos          = memory + 0x03b3;

uint8_t *SprObject_Rel_YPos     = memory + 0x03b8;
uint8_t *Player_Rel_YPos        = memory + 0x03b8;
uint8_t *Enemy_Rel_YPos         = memory + 0x03b9;
uint8_t *Fireball_Rel_YPos      = memory + 0x03ba;
uint8_t *Bubble_Rel_YPos        = memory + 0x03bb;
uint8_t *Block_Rel_YPos         = memory + 0x03bc;
uint8_t *Misc_Rel_YPos          = memory + 0x03be;

uint8_t *SprObject_SprAttrib    = memory + 0x03c4;
uint8_t *Player_SprAttrib       = memory + 0x03c4;
uint8_t *Enemy_SprAttrib        = memory + 0x03c5;

uint8_t *SprObject_X_MoveForce  = memory + 0x0400;
uint8_t *Enemy_X_MoveForce      = memory + 0x0401;

uint8_t *SprObject_YMF_Dummy    = memory + 0x0416;
uint8_t *Player_YMF_Dummy       = memory + 0x0416;
uint8_t *Enemy_YMF_Dummy        = memory + 0x0417;
uint8_t *Bubble_YMF_Dummy       = memory + 0x042c;

uint8_t *SprObject_Y_MoveForce  = memory + 0x0433;
uint8_t *Player_Y_MoveForce     = memory + 0x0433;
uint8_t *Enemy_Y_MoveForce      = memory + 0x0434;
uint8_t *Block_Y_MoveForce      = memory + 0x043c;

uint8_t *DisableCollisionDet    = memory + 0x0716;
uint8_t *Player_CollisionBits   = memory + 0x0490;
uint8_t *Enemy_CollisionBits    = memory + 0x0491;

uint8_t *SprObj_BoundBoxCtrl    = memory + 0x0499;
uint8_t *Player_BoundBoxCtrl    = memory + 0x0499;
uint8_t *Enemy_BoundBoxCtrl     = memory + 0x049a;
uint8_t *Fireball_BoundBoxCtrl  = memory + 0x04a0;
uint8_t *Misc_BoundBoxCtrl      = memory + 0x04a2;

uint8_t *EnemyFrenzyBuffer      = memory + 0x06cb;
uint8_t *EnemyFrenzyQueue       = memory + 0x06cd;
uint8_t *Enemy_Flag             = memory + 0x0f;
uint8_t *Enemy_ID               = memory + 0x16;

uint8_t *PlayerGfxOffset        = memory + 0x06d5;
uint8_t *Player_XSpeedAbsolute  = memory + 0x0700;
uint8_t *FrictionAdderHigh      = memory + 0x0701;
uint8_t *FrictionAdderLow       = memory + 0x0702;
uint8_t *RunningSpeed           = memory + 0x0703;
uint8_t *SwimmingFlag           = memory + 0x0704;
uint8_t *Player_X_MoveForce     = memory + 0x0705;
uint8_t *DiffToHaltJump         = memory + 0x0706;
uint8_t *JumpOrigin_Y_HighPos   = memory + 0x0707;
uint8_t *JumpOrigin_Y_Position  = memory + 0x0708;
uint8_t *VerticalForce          = memory + 0x0709;
uint8_t *VerticalForceDown      = memory + 0x070a;
uint8_t *PlayerChangeSizeFlag   = memory + 0x070b;
uint8_t *PlayerAnimTimerSet     = memory + 0x070c;
uint8_t *PlayerAnimCtrl         = memory + 0x070d;
uint8_t *DeathMusicLoaded       = memory + 0x0712;
uint8_t *FlagpoleSoundQueue     = memory + 0x0713;
uint8_t *CrouchingFlag          = memory + 0x0714;
uint8_t *MaximumLeftSpeed       = memory + 0x0450;
uint8_t *MaximumRightSpeed      = memory + 0x0456;

uint8_t *SprObject_OffscrBits   = memory + 0x03d0;
uint8_t *Player_OffscreenBits   = memory + 0x03d0;
uint8_t *Enemy_OffscreenBits    = memory + 0x03d1;
uint8_t *FBall_OffscreenBits    = memory + 0x03d2;
uint8_t *Bubble_OffscreenBits   = memory + 0x03d3;
uint8_t *Block_OffscreenBits    = memory + 0x03d4;
uint8_t *Misc_OffscreenBits     = memory + 0x03d6;
uint8_t *EnemyOffscrBitsMasked  = memory + 0x03d8;

uint8_t *Cannon_Offset          = memory + 0x046a;
uint8_t *Cannon_PageLoc         = memory + 0x046b;
uint8_t *Cannon_X_Position      = memory + 0x0471;
uint8_t *Cannon_Y_Position      = memory + 0x0477;
uint8_t *Cannon_Timer           = memory + 0x047d;

uint8_t *Whirlpool_Offset       = memory + 0x046a;
uint8_t *Whirlpool_PageLoc      = memory + 0x046b;
uint8_t *Whirlpool_LeftExtent   = memory + 0x0471;
uint8_t *Whirlpool_Length       = memory + 0x0477;
uint8_t *Whirlpool_Flag         = memory + 0x047d;

uint8_t *VineFlagOffset         = memory + 0x0398;
uint8_t *VineHeight             = memory + 0x0399;
uint8_t *VineObjOffset          = memory + 0x039a;
uint8_t *VineStart_Y_Position   = memory + 0x039d;

uint8_t *Block_Orig_YPos        = memory + 0x03e4;
uint8_t *Block_BBuf_Low         = memory + 0x03e6;
uint8_t *Block_Metatile         = memory + 0x03e8;
uint8_t *Block_PageLoc2         = memory + 0x03ea;
uint8_t *Block_RepFlag          = memory + 0x03ec;
uint8_t *Block_ResidualCounter  = memory + 0x03f0;
uint8_t *Block_Orig_XPos        = memory + 0x03f1;

uint8_t *BoundingBox_UL_XPos    = memory + 0x04ac;
uint8_t *BoundingBox_UL_YPos    = memory + 0x04ad;
uint8_t *BoundingBox_DR_XPos    = memory + 0x04ae;
uint8_t *BoundingBox_DR_YPos    = memory + 0x04af;
uint8_t *BoundingBox_UL_Corner  = memory + 0x04ac;
uint8_t *BoundingBox_LR_Corner  = memory + 0x04ae;
uint8_t *EnemyBoundingBoxCoord  = memory + 0x04b0;

uint8_t *PowerUpType            = memory + 0x39;

uint8_t *FireballBouncingFlag   = memory + 0x3a;
uint8_t *FireballCounter        = memory + 0x06ce;
uint8_t *FireballThrowingTimer  = memory + 0x0711;

uint8_t *HammerEnemyOffset      = memory + 0x06ae;
uint8_t *JumpCoinMiscOffset     = memory + 0x06b7;

uint8_t *Block_Buffer_1         = memory + 0x0500;
uint8_t *Block_Buffer_2         = memory + 0x05d0;

uint8_t *HammerThrowingTimer    = memory + 0x03a2;
uint8_t *HammerBroJumpTimer     = memory + 0x3c;
uint8_t *Misc_Collision_Flag    = memory + 0x06be;

uint8_t *RedPTroopaOrigXPos     = memory + 0x0401;
uint8_t *RedPTroopaCenterYPos   = memory + 0x58;

uint8_t *XMovePrimaryCounter    = memory + 0xa0;
uint8_t *XMoveSecondaryCounter  = memory + 0x58;

uint8_t *CheepCheepMoveMFlag    = memory + 0x58;
uint8_t *CheepCheepOrigYPos     = memory + 0x0434;
uint8_t *BitMFilter             = memory + 0x06dd;

uint8_t *LakituReappearTimer    = memory + 0x06d1;
uint8_t *LakituMoveSpeed        = memory + 0x58;
uint8_t *LakituMoveDirection    = memory + 0xa0;

uint8_t *FirebarSpinState_Low   = memory + 0x58;
uint8_t *FirebarSpinState_High  = memory + 0xa0;
uint8_t *FirebarSpinSpeed       = memory + 0x0388;
uint8_t *FirebarSpinDirection   = memory + 0x34;

uint8_t *DuplicateObj_Offset    = memory + 0x06cf;
uint8_t *NumberofGroupEnemies   = memory + 0x06d3;

uint8_t *BlooperMoveCounter     = memory + 0xa0;
uint8_t *BlooperMoveSpeed       = memory + 0x58;

uint8_t *BowserBodyControls     = memory + 0x0363;
uint8_t *BowserFeetCounter      = memory + 0x0364;
uint8_t *BowserMovementSpeed    = memory + 0x0365;
uint8_t *BowserOrigXPos         = memory + 0x0366;
uint8_t *BowserFlameTimerCtrl   = memory + 0x0367;
uint8_t *BowserFront_Offset     = memory + 0x0368;
uint8_t *BridgeCollapseOffset   = memory + 0x0369;
uint8_t *BowserGfxFlag          = memory + 0x036a;
uint8_t *BowserHitPoints        = memory + 0x0483;
uint8_t *MaxRangeFromOrigin     = memory + 0x06dc;

uint8_t *BowserFlamePRandomOfs  = memory + 0x0417;

uint8_t *PiranhaPlantUpYPos     = memory + 0x0417;
uint8_t *PiranhaPlantDownYPos   = memory + 0x0434;
uint8_t *PiranhaPlant_Y_Speed   = memory + 0x58;
uint8_t *PiranhaPlant_MoveFlag  = memory + 0xa0;

uint8_t *FireworksCounter       = memory + 0x06d7;
uint8_t *ExplosionGfxCounter    = memory + 0x58;
uint8_t *ExplosionTimerCounter  = memory + 0xa0;

//sound related defines
uint8_t *Squ2_NoteLenBuffer     = memory + 0x07b3;
uint8_t *Squ2_NoteLenCounter    = memory + 0x07b4;
uint8_t *Squ2_EnvelopeDataCtrl  = memory + 0x07b5;
uint8_t *Squ1_NoteLenCounter    = memory + 0x07b6;
uint8_t *Squ1_EnvelopeDataCtrl  = memory + 0x07b7;
uint8_t *Tri_NoteLenBuffer      = memory + 0x07b8;
uint8_t *Tri_NoteLenCounter     = memory + 0x07b9;
uint8_t *Noise_BeatLenCounter   = memory + 0x07ba;
uint8_t *Squ1_SfxLenCounter     = memory + 0x07bb;
uint8_t *Squ2_SfxLenCounter     = memory + 0x07bd;
uint8_t *Sfx_SecondaryCounter   = memory + 0x07be;
uint8_t *Noise_SfxLenCounter    = memory + 0x07bf;

uint8_t *PauseSoundQueue        = memory + 0xfa;
uint8_t *Square1SoundQueue      = memory + 0xff;
uint8_t *Square2SoundQueue      = memory + 0xfe;
uint8_t *NoiseSoundQueue        = memory + 0xfd;
uint8_t *AreaMusicQueue         = memory + 0xfb;
uint8_t *EventMusicQueue        = memory + 0xfc;

uint8_t *Square1SoundBuffer     = memory + 0xf1;
uint8_t *Square2SoundBuffer     = memory + 0xf2;
uint8_t *NoiseSoundBuffer       = memory + 0xf3;
uint8_t *AreaMusicBuffer        = memory + 0xf4;
uint8_t *EventMusicBuffer       = memory + 0x07b1;
uint8_t *PauseSoundBuffer       = memory + 0x07b2;

// uint8_t *MusicData              = memory + 0xf5;
const uint8_t *MusicData;
uint8_t *MusicDataLow           = memory + 0xf5;
uint8_t *MusicDataHigh          = memory + 0xf6;
uint8_t *MusicOffset_Square2    = memory + 0xf7;
uint8_t *MusicOffset_Square1    = memory + 0xf8;
uint8_t *MusicOffset_Triangle   = memory + 0xf9;
uint8_t *MusicOffset_Noise      = memory + 0x07b0;

uint8_t *NoteLenLookupTblOfs    = memory + 0xf0;
uint8_t *DAC_Counter            = memory + 0x07c0;
uint8_t *NoiseDataLoopbackOfs   = memory + 0x07c1;
uint8_t *NoteLengthTblAdder     = memory + 0x07c4;
uint8_t *AreaMusicBuffer_Alt    = memory + 0x07c5;
uint8_t *PauseModeFlag          = memory + 0x07c6;
uint8_t *GroundMusicHeaderOfs   = memory + 0x07c7;
uint8_t *AltRegContentFlag      = memory + 0x07ca;

// CONSTANTS

// sound effects constants
#define Sfx_SmallJump           ((uint8_t)0x80)
#define Sfx_Flagpole            ((uint8_t)0x40)
#define Sfx_Fireball            ((uint8_t)0x20)
#define Sfx_PipeDown_Injury     ((uint8_t)0x10)
#define Sfx_EnemySmack          ((uint8_t)0x08)
#define Sfx_EnemyStomp          ((uint8_t)0x04)
#define Sfx_Bump                ((uint8_t)0x02)
#define Sfx_BigJump             ((uint8_t)0x01)

#define Sfx_BowserFall          ((uint8_t)0x80)
#define Sfx_ExtraLife           ((uint8_t)0x40)
#define Sfx_PowerUpGrab         ((uint8_t)0x20)
#define Sfx_TimerTick           ((uint8_t)0x10)
#define Sfx_Blast               ((uint8_t)0x08)
#define Sfx_GrowVine            ((uint8_t)0x04)
#define Sfx_GrowPowerUp         ((uint8_t)0x02)
#define Sfx_CoinGrab            ((uint8_t)0x01)

#define Sfx_BowserFlame         ((uint8_t)0x02)
#define Sfx_BrickShatter        ((uint8_t)0x01)

// music constants
#define Silence                 ((uint8_t)0x80)

#define StarPowerMusic          ((uint8_t)0x40)
#define PipeIntroMusic          ((uint8_t)0x20)
#define CloudMusic              ((uint8_t)0x10)
#define CastleMusic             ((uint8_t)0x08)
#define UndergroundMusic        ((uint8_t)0x04)
#define WaterMusic              ((uint8_t)0x02)
#define GroundMusic             ((uint8_t)0x01)

#define TimeRunningOutMusic     ((uint8_t)0x40)
#define EndOfLevelMusic         ((uint8_t)0x20)
#define AltGameOverMusic        ((uint8_t)0x10)
#define EndOfCastleMusic        ((uint8_t)0x08)
#define VictoryMusic            ((uint8_t)0x04)
#define GameOverMusic           ((uint8_t)0x02)
#define DeathMusic              ((uint8_t)0x01)

//enemy object constants
#define GreenKoopa              ((uint8_t)0x00)
#define BuzzyBeetle             ((uint8_t)0x02)
#define RedKoopa                ((uint8_t)0x03)
#define HammerBro               ((uint8_t)0x05)
#define Goomba                  ((uint8_t)0x06)
#define Bloober                 ((uint8_t)0x07)
#define BulletBill_FrenzyVar    ((uint8_t)0x08)
#define GreyCheepCheep          ((uint8_t)0x0a)
#define RedCheepCheep           ((uint8_t)0x0b)
#define Podoboo                 ((uint8_t)0x0c)
#define PiranhaPlant            ((uint8_t)0x0d)
#define GreenParatroopaJump     ((uint8_t)0x0e)
#define RedParatroopa           ((uint8_t)0x0f)
#define GreenParatroopaFly      ((uint8_t)0x10)
#define Lakitu                  ((uint8_t)0x11)
#define Spiny                   ((uint8_t)0x12)
#define FlyCheepCheepFrenzy     ((uint8_t)0x14)
#define FlyingCheepCheep        ((uint8_t)0x14)
#define BowserFlame             ((uint8_t)0x15)
#define Fireworks               ((uint8_t)0x16)
#define BBill_CCheep_Frenzy     ((uint8_t)0x17)
#define Stop_Frenzy             ((uint8_t)0x18)
#define Bowser                  ((uint8_t)0x2d)
#define PowerUpObject           ((uint8_t)0x2e)
#define VineObject              ((uint8_t)0x2f)
#define FlagpoleFlagObject      ((uint8_t)0x30)
#define StarFlagObject          ((uint8_t)0x31)
#define JumpspringObject        ((uint8_t)0x32)
#define BulletBill_CannonVar    ((uint8_t)0x33)
#define RetainerObject          ((uint8_t)0x35)
#define TallEnemy               ((uint8_t)0x09)

//other constants
#define World1  ((uint8_t)0)
#define World2  ((uint8_t)1)
#define World3  ((uint8_t)2)
#define World4  ((uint8_t)3)
#define World5  ((uint8_t)4)
#define World6  ((uint8_t)5)
#define World7  ((uint8_t)6)
#define World8  ((uint8_t)7)

#define WarmBootOffset          ((uint8_t)0x07d6)
#define ColdBootOffset          ((uint8_t)0x07fe)
#define TitleScreenDataOffset   ((uint16_t)0x1ec0)
#define SoundMemory             ((uint16_t)0x07b0)
#define MusicHeaderOffsetData   ((const uint8_t *)(MusicHeaderData - 1))
#define MHD                     MusicHeaderData

#define A_Button                ((uint8_t)0x80)
#define B_Button                ((uint8_t)0x40)
#define Select_Button           ((uint8_t)0x20)
#define Start_Button            ((uint8_t)0x10)
#define Up_Dir                  ((uint8_t)0x08)
#define Down_Dir                ((uint8_t)0x04)
#define Left_Dir                ((uint8_t)0x02)
#define Right_Dir               ((uint8_t)0x01)

#define TitleScreenModeValue    ((uint8_t)0)
#define GameModeValue           ((uint8_t)1)
#define VictoryModeValue        ((uint8_t)2)
#define GameOverModeValue       ((uint8_t)3)

// $8000
void Start(void) {
    uint8_t a, x, y;
    bool n, z, c;

    // sei                      // pretty standard 6502 type init here
    // cld
    lda(0x10);                  // init PPU control register 1
    ppu_write_ppuctrl(a);
    ldx(0xff);                  // reset stack pointer
    sp = x;
// VBlank1:
    lda(ppu_read_ppustatus());  // wait two frames
    // bpl VBlank1
// VBlank2:
    lda(ppu_read_ppustatus());
    // bpl VBlank2
    ldy(ColdBootOffset);        // load default cold boot pointer
    ldx(0x05);                  // this is where we check for a warm boot
WBootCheck:
    lda(TopScoreDisplay[x]);    // check each score digit in the top score
    cmp(10);                    // to see if we have a valid digit
    bcs(ColdBoot);              // if not, give up and proceed with cold boot
    dex();
    bpl(WBootCheck);
    lda(*WarmBootValidation);   // second checkpoint, check to see if
    cmp(0xa5);                  // another location has a specific value
    bne(ColdBoot);
    ldy(WarmBootOffset);        // if passed both, load warm boot pointer
ColdBoot:
    a = InitializeMemory(y);    // clear memory using pointer in Y
    apu_write_dmc_raw(a);       // reset delta counter load register
    sta(*OperMode);             // reset primary mode of operation
    lda(0xa5);                  // set warm boot flag
    sta(*WarmBootValidation);
    sta(*PseudoRandomBitReg);   // set seed for pseudorandom register
    lda(0x0f);
    apu_write_snd_chn(a);       // enable all sound channels except dmc
    lda(0x06);
    ppu_write_ppumask(a);       // turn off clipping for OAM and background
    MoveAllSpritesOffscreen();
    InitializeNameTables();     // initialize both name tables
    inc(*DisableScreenFlag);    // set flag to disable screen output
    lda(*Mirror_PPU_CTRL_REG1);
    ora(0x80);                  // enable NMIs
    WritePPUReg1(a);
// EndlessLoop:
    // jmp EndlessLoop          // endless loop, need I say more?
}

const uint8_t *VRAM_AddrTable[0x13] = {
    NULL, WaterPaletteData, GroundPaletteData,
    UndergroundPaletteData, CastlePaletteData, NULL,
    NULL, NULL, BowserPaletteData,
    DaySnowPaletteData, NightSnowPaletteData, MushroomPaletteData,
    MarioThanksMessage, LuigiThanksMessage, MushroomRetainerSaved,
    PrincessSaved1, PrincessSaved2, WorldSelectMessage1,
    WorldSelectMessage2
};

uint8_t VRAM_Buffer_Offset[0x02] = {
    0x00, 0x00
};

// $8082
void NonMaskableInterrupt(void) {
    uint8_t a, x, y;
    uint8_t m0;
    bool n, z, c;

    uint8_t s[1];
    uint8_t *sp = s + sizeof(s) - 1;

    lda(*Mirror_PPU_CTRL_REG1);         // disable NMIs in mirror reg
    and(0x7f);                          // save all other bits
    sta(*Mirror_PPU_CTRL_REG1);
    and(0x7e);                          // alter name table address to be $2800
    ppu_write_ppuctrl(a);               // (essentially $2000) but save other bits
    lda(*Mirror_PPU_CTRL_REG2);         // disable OAM and background display by default
    and(0xe6);
    ldy(*DisableScreenFlag);            // get screen disable flag
    bne(ScreenOff);                     // if set, used bits as-is
    lda(*Mirror_PPU_CTRL_REG2);         // otherwise reenable bits and save them
    ora(0x1e);
ScreenOff:
    sta(*Mirror_PPU_CTRL_REG2);         // save bits for later but not in register at the moment
    and(0xe7);                          // disable screen for now
    ppu_write_ppumask(a);
    ldx(ppu_read_ppustatus());          // reset flip-flop and reset scroll registers to zero
    lda(0x00);
    InitScroll(a);
    ppu_write_oamaddr(a);               // reset spr-ram address register
    lda(0x02);                          // perform spr-ram DMA access on $0200-$02ff
    cpu_write_oamdma(a);
    ldx(*VRAM_Buffer_AddrCtrl);         // load control for pointer to buffer contents
                                        // set indirect at $00 to pointer
    UpdateScreen(VRAM_AddrTable[x]);    // update screen with buffer contents
    ldy(0x00);
    ldx(*VRAM_Buffer_AddrCtrl);         // check for usage of $0341
    cpx(0x06);
    bne(InitBuffer);
    iny();                              // get offset based on usage
InitBuffer:
    ldx(VRAM_Buffer_Offset[y]);
    lda(0x00);                          // clear buffer header at last location
    sta(VRAM_Buffer1_Offset[x]);
    sta(VRAM_Buffer1[x]);
    sta(*VRAM_Buffer_AddrCtrl);         // reinit address control to $0301
    lda(*Mirror_PPU_CTRL_REG2);         // copy mirror of $2001 to register
    ppu_write_ppumask(a);
    SoundEngine();                      // play sound
    ReadJoypads();                      // read joypads
    PauseRoutine();                     // handle pause
    UpdateTopScore();
    lda(*GamePauseStatus);              // check for pause status
    lsr(a);
    bcs(PauseSkip);
    lda(*TimerControl);                 // if master timer control not set, decrement
    beq(DecTimers);                     // all frame and interval timers
    dec(*TimerControl);
    bne(NoDecTimers);
DecTimers:
    ldx(0x14);                          // load end offset for end of frame timers
    dec(*IntervalTimerControl);         // decrement interval timer control,
    bpl(DecTimersLoop);                 // if not expired, only frame timers will decrement
    lda(0x14);
    sta(*IntervalTimerControl);         // if control for interval timers expired,
    ldx(0x23);                          // interval timers will decrement along with frame timers
DecTimersLoop:
    lda(Timers[x]);                     // check current timer
    beq(SkipExpTimer);                  // if current timer expired, branch to skip,
    dec(Timers[x]);                     // otherwise decrement the current timer
SkipExpTimer:
    dex();                              // move onto next timer
    bpl(DecTimersLoop);                 // do this until all timers are dealt with
NoDecTimers:
    inc(*FrameCounter);                 // increment frame counter
PauseSkip:
    ldx(0x00);
    ldy(0x07);
    lda(*PseudoRandomBitReg);           // get first memory location of LSFR bytes
    and(0x02);                          // mask out all but d1
    sta(m0);                            // save here
    lda(PseudoRandomBitReg[1]);         // get second memory location
    and(0x02);                          // mask out all but d1
    eor(m0);                            // perform exclusive-OR on d1 from first and second bytes
    clc();                              // if neither or both are set, carry will be clear
    beq(RotPRandomBit);
    sec();                              // if one or the other is set, carry will be set
RotPRandomBit:
    ror(PseudoRandomBitReg[x]);         // rotate carry into d7, and rotate last bit into carry
    inx();                              // increment to next byte
    dey();                              // decrement for loop
    bne(RotPRandomBit);
    lda(*Sprite0HitDetectFlag);         // check for flag here
    beq(SkipSprite0);
Sprite0Clr:
    lda(ppu_read_ppustatus());          // wait for sprite 0 flag to clear, which will
    // and #%01000000                   // not happen until vblank has ended
    // bne Sprite0Clr
    lda(*GamePauseStatus);              // if in pause mode, do not bother with sprites at all
    lsr(a);
    bcs(Sprite0Hit);
    MoveSpritesOffscreen();
    SpriteShuffler();
Sprite0Hit:
    lda(ppu_read_ppustatus());          // do sprite #0 hit detection
    // and #%01000000
    // beq Sprite0Hit
    ppu_draw(0x1f);
    ldy(0x14);                          // small delay, to wait until we hit horizontal blank time
HBlankDelay:
    dey();
    bne(HBlankDelay);
SkipSprite0:
    lda(*HorizontalScroll);             // set scroll registers from variables
    ppu_write_ppuscroll(a);
    lda(*VerticalScroll);
    ppu_write_ppuscroll(a);
    lda(*Mirror_PPU_CTRL_REG1);         // load saved mirror of $2000
    pha();
    ppu_write_ppuctrl(a);
    lda(*GamePauseStatus);              // if in pause mode, do not perform operation mode stuff
    lsr(a);
    bcs(SkipMainOper);
    OperModeExecutionTree();            // otherwise do one of many, many possible subroutines
SkipMainOper:
    lda(ppu_read_ppustatus());          // reset flip-flop
    pla();
    ora(0x80);                          // reactivate NMIs
    ppu_write_ppuctrl(a);
    // rti                              // we are done until the next frame!
}

// -------------------------------------------------------------------------------------

void PauseRoutine(void) {
    uint8_t a, y;
    bool n, z, c;

    lda(*OperMode);         // are we in victory mode?
    cmp(VictoryModeValue);  // if so, go ahead
    beq(ChkPauseTimer);
    cmp(GameModeValue);     // are we in game mode?
    bne(ExitPause);         // if not, leave
    lda(*OperMode_Task);    // if we are in game mode, are we running game engine?
    cmp(0x03);
    bne(ExitPause);         // if not, leave
ChkPauseTimer:
    lda(*GamePauseTimer);   // check if pause timer is still counting down
    beq(ChkStart);
    dec(*GamePauseTimer);   // if so, decrement and leave
    return;
ChkStart:
    lda(*SavedJoypad1Bits); // check to see if start is pressed
    and(Start_Button);      // on controller 1
    beq(ClrPauseTimer);
    lda(*GamePauseStatus);  // check to see if timer flag is set
    and(0x80);              // and if so, do not reset timer (residual,
    bne(ExitPause);         // joypad reading routine makes this unnecessary)
    lda(0x2b);              // set pause timer
    sta(*GamePauseTimer);
    lda(*GamePauseStatus);
    tay();
    iny();                  // set pause sfx queue for next pause mode
    sty(*PauseSoundQueue);
    eor(0x01);              // invert d0 and set d7
    ora(0x80);
    bne(SetPause);          // unconditional branch
ClrPauseTimer:
    lda(*GamePauseStatus)   // clear timer flag if timer is at zero and start button
    and(0x7f);              // is not pressed
SetPause:
    sta(*GamePauseStatus);
ExitPause:
    // rts
}

// -------------------------------------------------------------------------------------
// $00 - used for preset value

void SpriteShuffler(void) {
    uint8_t a, x, y;
    bool n, v, z, c;
    uint8_t m0;

SpriteShuffler:
    ldy(*AreaType);                 // load level type, likely residual code
    lda(0x28);                      // load preset value which will put it at
    sta(m0);                        // sprite #10
    ldx(0x0e);                      // start at the end of OAM data offsets
ShuffleLoop:
    lda(SprDataOffset[x]);          // check for offset value against
    cmp(m0);                        // the preset value
    bcc(NextSprOffset);             // if less, skip this part
    ldy(*SprShuffleAmtOffset);      // get current offset to preset value we want to add
    clc();
    adc(SprShuffleAmt[y]);          // get shuffle amount, add to current sprite offset
    bcc(StrSprOffset);              // if not exceeded $ff, skip second add
    clc();
    adc(m0);                        // otherwise add preset value $28 to offset
StrSprOffset:
    sta(SprDataOffset[x]);          // store new offset here or old one if branched to here
NextSprOffset:
    dex();                          // move backwards to next one
    bpl(ShuffleLoop);
    ldx(*SprShuffleAmtOffset);      // load offset
    inx();
    cpx(0x03);                      // check if offset + 1 goes to 3
    bne(SetAmtOffset);              // if offset + 1 not 3, store
    ldx(0x00);                      // otherwise, init to 0
SetAmtOffset:
    stx(*SprShuffleAmtOffset);
    ldx(0x08);                      // load offsets for values and storage
    ldy(0x02);
SetMiscOffset:
    lda(SprDataOffset[y+5]);        // load one of three OAM data offsets
    sta(Misc_SprDataOffset[x-2]);   // store first one unmodified, but
    clc();                          // add eight to the second and eight
    adc(0x08);                      // more to the third one
    sta(Misc_SprDataOffset[x-1]);   // note that due to the way X is set up,
    clc();                          // this code loads into the misc sprite offsets
    adc(0x08);
    sta(Misc_SprDataOffset[x]);        
    dex();
    dex();
    dex();
    dey();
    bpl(SetMiscOffset);             // do this until all misc spr offsets are loaded
    // rts
}

// -------------------------------------------------------------------------------------

void OperModeExecutionTree(void) {
    // this is the heart of the entire program,
    // most of what goes on starts here

    const void (*f[])(void) = {
        TitleScreenMode,
        GameMode,
        VictoryMode,
        GameOverMode
    };

    f[*OperMode]();
}

// -------------------------------------------------------------------------------------

void MoveAllSpritesOffscreen(void) {
    uint8_t a, y;
    bool n, z;

    ldy(0x00);                  // this routine moves all sprites off the screen
    // bit $04a0                // BIT instruction opcode
    lda(0xf8);
SprInitLoop:
    sta(Sprite_Y_Position[y]);  // write 248 into OAM data's Y coordinate
    iny();                      // which will move it off the screen
    iny();
    iny();
    iny();
    bne(SprInitLoop);
    // rts
}

void MoveSpritesOffscreen(void) {
    uint8_t a, y;
    bool n, z;

    ldy(0x04);                  // this routine moves all but sprite 0
    lda(0xf8);                  // off the screen
SprInitLoop:
    sta(Sprite_Y_Position[y]);  // write 248 into OAM data's Y coordinate
    iny();                      // which will move it off the screen
    iny();
    iny();
    iny();
    bne(SprInitLoop);
    // rts
}

// -------------------------------------------------------------------------------------

void TitleScreenMode(void) {
    const void (*f[])(void) = {
        InitializeGame,
        ScreenRoutines,
        PrimaryGameSetup,
        GameMenuRoutine
    };

    f[*OperMode_Task]();
}

// -------------------------------------------------------------------------------------

void GameMenuRoutine(void) {
    pc = 0x8245;
    cpu_execute();
}

void VictoryMode(void) {
    pc = 0x838b;
    cpu_execute();
}

void ScreenRoutines(void) {
    pc = 0x8567;
    cpu_execute();
}

// VRAM BUFFER DATA FOR LOCATIONS IN PRG-ROM

const uint8_t WaterPaletteData[0x24] = {
    0x3f, 0x00, 0x20,
    0x0f, 0x15, 0x12, 0x25,
    0x0f, 0x3a, 0x1a, 0x0f,
    0x0f, 0x30, 0x12, 0x0f,
    0x0f, 0x27, 0x12, 0x0f,
    0x22, 0x16, 0x27, 0x18,
    0x0f, 0x10, 0x30, 0x27,
    0x0f, 0x16, 0x30, 0x27,
    0x0f, 0x0f, 0x30, 0x10,
    0x00
};

const uint8_t GroundPaletteData[0x24] = {
    0x3f, 0x00, 0x20,
    0x0f, 0x29, 0x1a, 0x0f,
    0x0f, 0x36, 0x17, 0x0f,
    0x0f, 0x30, 0x21, 0x0f,
    0x0f, 0x27, 0x17, 0x0f,
    0x0f, 0x16, 0x27, 0x18,
    0x0f, 0x1a, 0x30, 0x27,
    0x0f, 0x16, 0x30, 0x27,
    0x0f, 0x0f, 0x36, 0x17,
    0x00
};

const uint8_t UndergroundPaletteData[0x24] = {
    0x3f, 0x00, 0x20,
    0x0f, 0x29, 0x1a, 0x09,
    0x0f, 0x3c, 0x1c, 0x0f,
    0x0f, 0x30, 0x21, 0x1c,
    0x0f, 0x27, 0x17, 0x1c,
    0x0f, 0x16, 0x27, 0x18,
    0x0f, 0x1c, 0x36, 0x17,
    0x0f, 0x16, 0x30, 0x27,
    0x0f, 0x0c, 0x3c, 0x1c,
    0x00
};

const uint8_t CastlePaletteData[0x24] = {
    0x3f, 0x00, 0x20,
    0x0f, 0x30, 0x10, 0x00,
    0x0f, 0x30, 0x10, 0x00,
    0x0f, 0x30, 0x16, 0x00,
    0x0f, 0x27, 0x17, 0x00,
    0x0f, 0x16, 0x27, 0x18,
    0x0f, 0x1c, 0x36, 0x17,
    0x0f, 0x16, 0x30, 0x27,
    0x0f, 0x00, 0x30, 0x10,
    0x00
};

const uint8_t DaySnowPaletteData[0x08] = {
    0x3f, 0x00, 0x04,
    0x22, 0x30, 0x00, 0x10,
    0x00
};

const uint8_t NightSnowPaletteData[0x08] = {
    0x3f, 0x00, 0x04,
    0x0f, 0x30, 0x00, 0x10,
    0x00
};

const uint8_t MushroomPaletteData[0x08] = {
    0x3f, 0x00, 0x04,
    0x22, 0x27, 0x16, 0x0f,
    0x00
};

const uint8_t BowserPaletteData[0x08] = {
    0x3f, 0x14, 0x04,
    0x0f, 0x1a, 0x30, 0x27,
    0x00
};

const uint8_t MarioThanksMessage[0x14] = {
    // "THANK YOU MARIO!"
    0x25, 0x48, 0x10,
    0x1d, 0x11, 0x0a, 0x17, 0x14, 0x24,
    0x22, 0x18, 0x1e, 0x24,
    0x16, 0x0a, 0x1b, 0x12, 0x18, 0x2b,
    0x00
};

const uint8_t LuigiThanksMessage[0x14] = {
    // "THANK YOU LUIGI!"
    0x25, 0x48, 0x10,
    0x1d, 0x11, 0x0a, 0x17, 0x14, 0x24,
    0x22, 0x18, 0x1e, 0x24,
    0x15, 0x1e, 0x12, 0x10, 0x12, 0x2b,
    0x00
};

const uint8_t MushroomRetainerSaved[0x2c] = {
    // "BUT OUR PRINCESS IS IN"
    0x25, 0xc5, 0x16,
    0x0b, 0x1e, 0x1d, 0x24, 0x18, 0x1e, 0x1b, 0x24,
    0x19, 0x1b, 0x12, 0x17, 0x0c, 0x0e, 0x1c, 0x1c, 0x24,
    0x12, 0x1c, 0x24, 0x12, 0x17,
    // "ANOTHER CASTLE!"
    0x26, 0x05, 0x0f,
    0x0a, 0x17, 0x18, 0x1d, 0x11, 0x0e, 0x1b, 0x24,
    0x0c, 0x0a, 0x1c, 0x1d, 0x15, 0x0e, 0x2b, 0x00
};

const uint8_t PrincessSaved1[0x17] = {
    // "YOUR QUEST IS OVER."
    0x25, 0xa7, 0x13,
    0x22, 0x18, 0x1e, 0x1b, 0x24,
    0x1a, 0x1e, 0x0e, 0x1c, 0x1d, 0x24,
    0x12, 0x1c, 0x24, 0x18, 0x1f, 0x0e, 0x1b, 0xaf,
    0x00
};

const uint8_t PrincessSaved2[0x1f] = {
    // "WE PRESENT YOU A NEW QUEST."
    0x25, 0xe3, 0x1b,
    0x20, 0x0e, 0x24,
    0x19, 0x1b, 0x0e, 0x1c, 0x0e, 0x17, 0x1d, 0x24,
    0x22, 0x18, 0x1e, 0x24, 0x0a, 0x24, 0x17, 0x0e, 0x20, 0x24,
    0x1a, 0x1e, 0x0e, 0x1c, 0x1d, 0xaf,
    0x00
};

const uint8_t WorldSelectMessage1[0x11] = {
    // "PUSH BUTTON B"
    0x26, 0x4a, 0x0d,
    0x19, 0x1e, 0x1c, 0x11, 0x24,
    0x0b, 0x1e, 0x1d, 0x1d, 0x18, 0x17, 0x24, 0x0b,
    0x00
};

const uint8_t WorldSelectMessage2[0x15] = {
    // "TO SELECT A WORLD"
    0x26, 0x88, 0x11,
    0x1d, 0x18, 0x24, 0x1c, 0x0e, 0x15, 0x0e, 0x0c, 0x1d, 0x24,
    0x0a, 0x24, 0x20, 0x18, 0x1b, 0x15, 0x0d,
    0x00
};

// $8e19
void InitializeNameTables(void) {
    uint8_t a;
    bool n, z;

    lda(ppu_read_ppustatus());  // reset flip-flop
    lda(*Mirror_PPU_CTRL_REG1); // load mirror of ppu reg $2000
    ora(0x10);                  // set sprites for first 4k and background for second 4k
    and(0xf0);                  // clear rest of lower nybble, leave higher alone
    WritePPUReg1(a);
    lda(0x24);                  // set vram address to start of name table 1
    WriteNTAddr(a);
    lda(0x20);                  // and then set it to name table 0
    WriteNTAddr(a);
}

// $8e2d
// inputs:              a
// possible outputs:    a, x, y, n, z
void WriteNTAddr(uint8_t a) {
    uint8_t x, y;
    bool n, z;

    ppu_write_ppuaddr(a);
    lda(0x00);
    ppu_write_ppuaddr(a);
    ldx(0x04);                  // clear name table with blank tile #24
    ldy(0xc0);
    lda(0x24);
InitNTLoop:
    ppu_write_ppudata(a);       // count out exactly 768 tiles
    dey();
    bne(InitNTLoop);
    dex();
    bne(InitNTLoop);
    ldy(64);                    // now to clear the attribute table (with zero this time)
    txa();
    sta(*VRAM_Buffer1_Offset);  // init vram buffer 1 offset
    sta(*VRAM_Buffer1);         // init vram buffer 1
InitATLoop:
    ppu_write_ppudata(a);
    dey();
    bne(InitATLoop);
    sta(*HorizontalScroll);     // reset scroll variables
    sta(*VerticalScroll);
    InitScroll(a);              // initialize scroll registers to zero
}

void ReadJoypads(void) {
    uint8_t a, x;
    bool n, z, c;

    lda(0x01);                  // reset and clear strobe of joypad ports
    cpu_write_joy1(a);
    lsr(a);
    tax();                      // start with joypad 1's port
    cpu_write_joy1(a);
    ReadPortBits(x);
    inx();                      // increment for joypad 2's port
    ReadPortBits(x);
}

void ReadPortBits(uint8_t x) {
    static const uint8_t (*cpu_read_joy[2])(void) = {
        cpu_read_joy1, cpu_read_joy2
    };

    uint8_t a, y;
    bool n, z, c;
    uint8_t m0;

    uint8_t s[1];
    uint8_t *sp = s + sizeof(s) - 1;

    ldy(0x08);
PortLoop:
    pha();                      // push previous bit onto stack
    lda(cpu_read_joy[x]());     // read current bit on joypad port
    sta(m0);                    // check d1 and d0 of port output
    lsr(a);                     // this is necessary on the old
    ora(m0);                    // famicom systems in japan
    lsr(a);
    pla();                      // read bits from stack
    rol(a);                     // rotate bit from carry flag
    dey();
    bne(PortLoop);              // count down bits left
    sta(SavedJoypadBits[x]);    // save controller status here always
    pha();
    and(0x30);                  // check for select or start
    and(JoypadBitMask[x]);      // if neither saved state nor current state
    beq(Save8Bits);             // have any of these two set, branch
    pla();
    and(0xcf);                  // otherwise store without select
    sta(SavedJoypadBits[x]);    // or start bits and leave
    return;
Save8Bits:
    pla();
    sta(JoypadBitMask[x]);      // save with all bits in another place and leave
    // rts
}

void UpdateScreen(const uint8_t *m0) {
    uint8_t a, x, y;
    bool n, v, z, c;

    uint8_t s[1];
    uint8_t *sp = s + sizeof(s) - 1;

    goto UpdateScreen;

WriteBufferToScreen:
    ppu_write_ppuaddr(a);       // store high byte of vram address
    iny();
    lda(m0[y]);                 // load next byte (second)
    ppu_write_ppuaddr(a);       // store low byte of vram address
    iny();
    lda(m0[y]);                 // load next byte (third)
    asl(a);                     // shift to left and save in stack
    pha();
    lda(*Mirror_PPU_CTRL_REG1); // load mirror of $2000,
    ora(0x04);                  // set ppu to increment by 32 by default
    bcs(SetupWrites);           // if d7 of third byte was clear, ppu will
    and(0xfb);                  // only increment by 1
SetupWrites:
    WritePPUReg1(a);            // write to register
    pla();                      // pull from stack and shift to left again
    asl(a);
    bcc(GetLength);             // if d6 of third byte was clear, do not repeat byte
    ora(0x02);                  // otherwise set d1 and increment Y
    iny();
GetLength:
    lsr(a);                     // shift back to the right to get proper length
    lsr(a);                     // note that d1 will now be in carry
    tax();
OutputToVRAM:
    bcs(RepeatByte);            // if carry set, repeat loading the same byte
    iny();                      // otherwise increment Y to load next byte
RepeatByte:
    lda(m0[y]);                 // load more data from buffer and write to vram
    ppu_write_ppudata(a);
    dex();                      // done writing?
    bne(OutputToVRAM);
    sec();
    tya();
    // adc $00                  // add end length plus one to the indirect at $00
    // sta $00                  // to allow this routine to read another set of updates
    // lda #$00
    // adc $01
    // sta $01
    m0 += a + (c ? 1 : 0);
    lda(0x3f);                  // sets vram address to $3f00
    ppu_write_ppuaddr(a);
    lda(0x00);
    ppu_write_ppuaddr(a);
    ppu_write_ppuaddr(a);       // then reinitializes it for some reason
    ppu_write_ppuaddr(a);
UpdateScreen:
    ldx(ppu_read_ppustatus());  // reset flip-flop
    ldy(0x00);                  // load first byte from indirect as a pointer
    lda(m0[y]);
    bne(WriteBufferToScreen);   // if byte is zero we have no further updates to make here
    InitScroll(a);
}

// $8ee6
// inputs:  a
// outputs: none
void InitScroll(uint8_t a) {
    ppu_write_ppuscroll(a); // store contents of A into scroll registers
    ppu_write_ppuscroll(a); // and end whatever subroutine led us here
}

// inputs:  a
// outputs: none
void WritePPUReg1(uint8_t a) {
    ppu_write_ppuctrl(a);       // write contents of A to PPU register 1
    sta(*Mirror_PPU_CTRL_REG1); // and its mirror
}

void UpdateTopScore(void) {
    uint8_t x;
    bool n, z;

    ldx(0x05);          // start with mario's score
    TopScoreCheck(x);
    ldx(0x0b);          // now do luigi's score
    TopScoreCheck(x);
}

void TopScoreCheck(uint8_t x) {
    uint8_t a, y;
    bool n, v, z, c;

    ldy(0x05);                  // start with the lowest digit
    sec();         
GetScoreDiff:
    lda(PlayerScoreDisplay[x]); // subtract each player digit from each high score digit
    sbc(TopScoreDisplay[y]);    // from lowest to highest, if any top score digit exceeds
    dex();                      // any player digit, borrow will be set until a subsequent
    dey();                      // subtraction clears it (player digit is higher than top)
    bpl(GetScoreDiff);      
    bcc(NoTopSc);               // check to see if borrow is still set, if so, no new high score
    inx();                      // increment X and Y once to the start of the score
    iny();
CopyScore:
    lda(PlayerScoreDisplay[x]); // store player's score digits into high score memory area
    sta(TopScoreDisplay[y]);
    inx();
    iny();
    cpy(0x06);                  // do this until we have stored them all
    bcc(CopyScore);
NoTopSc:
    // rts
}

void InitializeGame(void) {
    pc = 0x8fcf;
    cpu_execute();
}

void PrimaryGameSetup(void) {
    pc = 0x9061;
    cpu_execute();
}

// $90cc
// inputs:              y
// possible outputs:    a, x, y, n, z, c
uint8_t InitializeMemory(uint8_t y) {
    uint8_t a, x;
    bool n, z, c;
    uint8_t m6, m7;

    ldx(0x07);                      // set initial high byte to $0700-$07ff
    lda(0x00);                      // set initial low byte to start of page (at $00 of page)
    sta(m6);
InitPageLoop:
    stx(m7);
InitByteLoop:
    cpx(0x01);                      // check to see if we're on the stack ($0100-$01ff)
    bne(InitByte);                  // if not, go ahead anyway
    cpy(0x60);                      // otherwise, check to see if we're at $0160-$01ff
    bcs(SkipByte);                  // if so, skip write
InitByte:
    sta(memory[(m6|(m7<<8))+y]);    // otherwise, initialize byte with current low byte in Y
SkipByte:
    dey();
    cpy(0xff);                      // do this until all bytes in page have been erased
    bne(InitByteLoop);
    dex();                          // go onto the next page
    bpl(InitPageLoop);              // do this until all pages of memory have been erased
    return a;
}

void GameOverMode(void) {
    pc = 0x9218;
    cpu_execute();
}

void GameMode(void) {
    pc = 0xaedc;
    cpu_execute();
}

void SoundEngine(void) {
    uint8_t a, x, y;
    bool n, z, c;

    lda(*OperMode);             // are we in title screen mode?
    bne(SndOn);
    apu_write_snd_chn(a);       // if so, disable sound and leave
    return;
SndOn:
    lda(0xff);
    apu_write_joy2(a);          // disable irqs and set frame counter mode???
    lda(0x0f);
    apu_write_snd_chn(a);       // enable first four channels
    lda(*PauseModeFlag);        // is sound already in pause mode?
    bne(InPause);
    lda(*PauseSoundQueue);      // if not, check pause sfx queue    
    cmp(0x01);
    bne(RunSoundSubroutines);   // if queue is empty, skip pause mode routine
InPause:
    lda(*PauseSoundBuffer);     // check pause sfx buffer
    bne(ContPau);
    lda(*PauseSoundQueue);      // check pause queue
    beq(SkipSoundSubroutines);
    sta(*PauseSoundBuffer);     // if queue full, store in buffer and activate
    sta(*PauseModeFlag);        // pause mode to interrupt game sounds
    lda(0x00);                  // disable sound and clear sfx buffers
    apu_write_snd_chn(a);
    sta(*Square1SoundBuffer);
    sta(*Square2SoundBuffer);
    sta(*NoiseSoundBuffer);
    lda(0x0f);
    apu_write_snd_chn(a);       // enable sound again
    lda(0x2a);                  // store length of sound in pause counter
    sta(*Squ1_SfxLenCounter);
PTone1F:
    lda(0x44);                  // play first tone
    bne(PTRegC);                // unconditional branch
ContPau:
    lda(*Squ1_SfxLenCounter);   // check pause length left
    cmp(0x24);                  // time to play second?
    beq(PTone2F);
    cmp(0x1e);                  // time to play first again?
    beq(PTone1F);
    cmp(0x18);                  // time to play second again?
    bne(DecPauC);               // only load regs during times, otherwise skip
PTone2F:
    lda(0x64);                  // store reg contents and play the pause sfx
PTRegC:
    ldx(0x84);
    ldy(0x7f);
    PlaySqu1Sfx(a, x, y);
DecPauC:
    dec(*Squ1_SfxLenCounter);   // decrement pause sfx counter
    bne(SkipSoundSubroutines);
    lda(0x00);                  // disable sound if in pause mode and
    apu_write_snd_chn(a);       // not currently playing the pause sfx
    lda(*PauseSoundBuffer);     // if no longer playing pause sfx, check to see
    cmp(0x02);                  // if we need to be playing sound again
    bne(SkipPIn);
    lda(0x00);                  // clear pause mode to allow game sounds again
    sta(*PauseModeFlag);
SkipPIn:
    lda(0x00);                  // clear pause sfx buffer
    sta(*PauseSoundBuffer);
    beq(SkipSoundSubroutines);

RunSoundSubroutines:
    Square1SfxHandler();    // play sfx on square channel 1
    Square2SfxHandler();    //  ''  ''  '' square channel 2
    NoiseSfxHandler();      //  ''  ''  '' noise channel
    MusicHandler();         // play music on all channels
    lda(0x00);              // clear the music queues
    sta(*AreaMusicQueue);
    sta(*EventMusicQueue);

SkipSoundSubroutines:
    lda(0x00);              // clear the sound effects queues
    sta(*Square1SoundQueue);
    sta(*Square2SoundQueue);
    sta(*NoiseSoundQueue);
    sta(*PauseSoundQueue);
    ldy(*DAC_Counter);      // load some sort of counter 
    lda(*AreaMusicBuffer);
    and(0x03);              // check for specific music
    beq(NoIncDAC);
    inc(*DAC_Counter);      // increment and check counter
    cpy(0x30);
    bcc(StrWave);           // if not there yet, just store it
NoIncDAC:
    tya();
    beq(StrWave);           // if we are at zero, do not decrement 
    dec(*DAC_Counter);      // decrement counter
StrWave:
    apu_write_dmc_raw(y);   // store into DMC load register (??)
    // rts                  // we are done here
}

void Dump_Squ1_Regs(uint8_t x, uint8_t y) {
    apu_write_sq1_sweep(y); // dump the contents of X and Y into square 1's control regs
    apu_write_sq1_vol(x);
    // rts
}

bool PlaySqu1Sfx(uint8_t a, uint8_t x, uint8_t y) {
    bool z;

    Dump_Squ1_Regs(x, y);   // do sub to set ctrl regs for square 1, then set frequency regs
    SetFreq_Squ1(&a, &x, &y, &z);

    return z;
}

void SetFreq_Squ1(uint8_t *pa, uint8_t *px, uint8_t *py, bool *pz) {
    uint8_t a = *pa, x = *px, y = *py;
    bool n, z = *pz;

    ldx(0x00);  // set frequency reg offset for square 1 sound channel
    Dump_Freq_Regs(&a, &x, &y, &z);

    *pa = a, *px = x, *py = y, *pz = z;
}

void Dump_Freq_Regs(uint8_t *pa, uint8_t *px, uint8_t *py, bool *pz) {
    uint8_t a = *pa, x = *px, y = *py;
    bool n, z = *pz;
    
    static const void (*apu_write[])(uint8_t) = {
        NULL,
        NULL,
        apu_write_sq1_lo,
        apu_write_sq1_hi,
        NULL,
        NULL,
        apu_write_sq2_lo,
        apu_write_sq2_hi,
        NULL,
        NULL,
        apu_write_tri_lo,
        apu_write_tri_hi
    };

    tay();
    lda(FreqRegLookupTbl[y+1]); // use previous contents of A for sound reg offset
    beq(NoTone);                // if zero, then do not load
    apu_write[x+2](a);          // first byte goes into LSB of frequency divider
    lda(FreqRegLookupTbl[y]);   // second byte goes into 3 MSB plus extra bit for 
    ora(0x08);                  // length counter
    apu_write[x+3](a);
NoTone:
    // rts

    *pa = a, *px = x, *py = y, *pz = z;
}

void Dump_Sq2_Regs(uint8_t x, uint8_t y) {
    apu_write_sq2_vol(x);   // dump the contents of X and Y into square 2's control regs
    apu_write_sq2_sweep(y);
    // rts
}

void PlaySqu2Sfx(uint8_t a, uint8_t x, uint8_t y) {
    bool z;

    Dump_Sq2_Regs(x, y);    // do sub to set ctrl regs for square 2 sound channel
    SetFreq_Squ2(&a, &x, &y, &z);
}

void SetFreq_Squ2(uint8_t *pa, uint8_t *px, uint8_t *py, bool *pz) {
    uint8_t a = *pa, x = *px, y = *py;
    bool n, z = *pz;

    ldx(0x04);                      // set frequency reg offset for square 2 sound channel
    Dump_Freq_Regs(&a, &x, &y, &z); // unconditional branch
    
    *pa = a, *px = x, *py = y, *pz = z;
}

void SetFreq_Tri(uint8_t a) {
    uint8_t x, y;
    bool n, z;

    ldx(0x08);                      // set frequency reg offset for triangle sound channel
    Dump_Freq_Regs(&a, &x, &y, &z); // unconditional branch
}

const uint8_t SwimStompEnvelopeData[0xe] = {
    0x9f, 0x9b, 0x98, 0x96, 0x95, 0x94, 0x92, 0x90,
    0x90, 0x9a, 0x97, 0x95, 0x93, 0x92
};

// $f41b
void Square1SfxHandler(void) {
    uint8_t a, x, y;
    bool n, z, c;

    goto Square1SfxHandler;

PlayFlagpoleSlide:
    lda(0x40);                          // store length of flagpole sound
    sta(*Squ1_SfxLenCounter);
    lda(0x62);                          // load part of reg contents for flagpole sound
    SetFreq_Squ1(&a, &x, &y, &z);
    ldx(0x99);                          // now load the rest
    bne(FPS2nd);

PlaySmallJump:
    lda(0x26);                          // branch here for small mario jumping sound
    bne(JumpRegContents);

PlayBigJump:
    lda(0x18);                          // branch here for big mario jumping sound

JumpRegContents:
    ldx(0x82);                          // note that small and big jump borrow each others' reg contents
    ldy(0xa7);                          // anyway, this loads the first part of mario's jumping sound
    PlaySqu1Sfx(a, x, y);
    lda(0x28);                          // store length of sfx for both jumping sounds
    sta(*Squ1_SfxLenCounter);           // then continue on here

ContinueSndJump:
    lda(*Squ1_SfxLenCounter);           // jumping sounds seem to be composed of three parts
    cmp(0x25);                          // check for time to play second part yet
    bne(N2Prt);
    ldx(0x5f);                          // load second part
    ldy(0xf6);
    bne(DmpJpFPS);                      // unconditional branch
N2Prt:
    cmp(0x20);                          // check for third part
    bne(DecJpFPS);
    ldx(0x48);                          // load third part
FPS2nd:
    ldy(0xbc);                          // the flagpole slide sound shares part of third part
DmpJpFPS:
    Dump_Squ1_Regs(x, y);
    bne(DecJpFPS);                      // unconditional branch outta here

PlayFireballThrow:
    lda(0x05);
    ldy(0x99);                          // load reg contents for fireball throw sound
    bne(Fthrow);                        // unconditional branch

PlayBump:
    lda(0x0a);                          // load length of sfx and reg contents for bump sound
    ldy(0x93);
Fthrow:
    ldx(0x9e);                          // the fireball sound shares reg contents with the bump sound
    sta(*Squ1_SfxLenCounter);
    lda(0x0c);                          // load offset for bump sound
    PlaySqu1Sfx(a, x, y);

ContinueBumpThrow:    
    lda(*Squ1_SfxLenCounter);           // check for second part of bump sound
    cmp(0x06);   
    bne(DecJpFPS);
    lda(0xbb);                          // load second part directly
    apu_write_sq1_sweep(a);
DecJpFPS:
    bne(BranchToDecLength1);            // unconditional branch


Square1SfxHandler:
    ldy(*Square1SoundQueue);            // check for sfx in queue
    beq(CheckSfx1Buffer);
    sty(*Square1SoundBuffer);           // if found, put in buffer
    bmi(PlaySmallJump);                 // small jump
    lsr(*Square1SoundQueue);
    bcs(PlayBigJump);                   // big jump
    lsr(*Square1SoundQueue);
    bcs(PlayBump);                      // bump
    lsr(*Square1SoundQueue);
    bcs(PlaySwimStomp);                 // swim/stomp
    lsr(*Square1SoundQueue);
    bcs(PlaySmackEnemy);                // smack enemy
    lsr(*Square1SoundQueue);
    bcs(PlayPipeDownInj);               // pipedown/injury
    lsr(*Square1SoundQueue);
    bcs(PlayFireballThrow);             // fireball throw
    lsr(*Square1SoundQueue);
    bcs(PlayFlagpoleSlide);             // slide flagpole

CheckSfx1Buffer:
    lda(*Square1SoundBuffer);           // check for sfx in buffer 
    beq(ExS1H);                         // if not found, exit sub
    bmi(ContinueSndJump);               // small mario jump 
    lsr(a);
    bcs(ContinueSndJump);               // big mario jump 
    lsr(a);
    bcs(ContinueBumpThrow);             // bump
    lsr(a);
    bcs(ContinueSwimStomp);             // swim/stomp
    lsr(a);
    bcs(ContinueSmackEnemy);            // smack enemy
    lsr(a);
    bcs(ContinuePipeDownInj);           // pipedown/injury
    lsr(a);
    bcs(ContinueBumpThrow);             // fireball throw
    lsr(a);
    bcs(DecrementSfx1Length);           // slide flagpole
ExS1H:
    return;

PlaySwimStomp:
    lda(0x0e);                          // store length of swim/stomp sound
    sta(*Squ1_SfxLenCounter);
    ldy(0x9c);                          // store reg contents for swim/stomp sound
    ldx(0x9e);
    lda(0x26);
    PlaySqu1Sfx(a, x, y);

ContinueSwimStomp: 
    ldy(*Squ1_SfxLenCounter)            // look up reg contents in data section based on
    lda(SwimStompEnvelopeData[y-1]);    // length of sound left, used to control sound's
    apu_write_sq1_vol(a);               // envelope
    cpy(0x06);   
    bne(BranchToDecLength1);
    lda(0x9e);                          // when the length counts down to a certain point, put this
    apu_write_sq1_lo(a);                // directly into the LSB of square 1's frequency divider

BranchToDecLength1: 
    bne(DecrementSfx1Length);           // unconditional branch (regardless of how we got here)

PlaySmackEnemy:
    lda(0x0e);                          // store length of smack enemy sound
    ldy(0xcb);
    ldx(0x9f);
    sta(*Squ1_SfxLenCounter);
    lda(0x28);                          // store reg contents for smack enemy sound
    z = PlaySqu1Sfx(a, x, y);
    bne(DecrementSfx1Length);           // unconditional branch

ContinueSmackEnemy:
    ldy(*Squ1_SfxLenCounter);           // check about halfway through
    cpy(0x08);
    bne(SmSpc);
    lda(0xa0);                          // if we're at the about-halfway point, make the second tone
    apu_write_sq1_lo(a);                // in the smack enemy sound
    lda(0x9f);
    bne(SmTick);
SmSpc:
    lda(0x90);                          // this creates spaces in the sound, giving it its distinct noise
SmTick:
    apu_write_sq1_vol(a);

DecrementSfx1Length:
    dec(*Squ1_SfxLenCounter);           // decrement length of sfx
    bne(ExSfx1);

    StopSquare1Sfx();
ExSfx1:
    return;

PlayPipeDownInj:  
    lda(0x2f);                          // load length of pipedown sound
    sta(*Squ1_SfxLenCounter);

ContinuePipeDownInj:
    lda(*Squ1_SfxLenCounter);           // some bitwise logic, forces the regs
    lsr(a);                             // to be written to only during six specific times
    bcs(NoPDwnL);                       // during which d3 must be set and d1-0 must be clear
    lsr(a);
    bcs(NoPDwnL);
    and(0x02);
    beq(NoPDwnL);
    ldy(0x91);                          // and this is where it actually gets written in
    ldx(0x9a);
    lda(0x44);
    PlaySqu1Sfx(a, x, y);
NoPDwnL:
    jmp(DecrementSfx1Length);
}

void StopSquare1Sfx(void) {
    uint8_t x;
    bool n, z;

    ldx(0x00);                  // if end of sfx reached, clear buffer
    stx(*Square1SoundBuffer);   // and stop making the sfx
    ldx(0x0e);
    apu_write_snd_chn(x);
    ldx(0x0f);
    apu_write_snd_chn(x);
ExSfx1:
    // rts
}

const uint8_t ExtraLifeFreqData[0x6] = {
    0x58, 0x02, 0x54, 0x56, 0x4e, 0x44
};

const uint8_t PowerUpGrabFreqData[0x1b] = {
    0x4c, 0x52, 0x4c, 0x48, 0x3e, 0x36, 0x3e, 0x36, 0x30,
    0x28, 0x4a, 0x50, 0x4a, 0x64, 0x3c, 0x32, 0x3c, 0x32,
    0x2c, 0x24, 0x3a, 0x64, 0x3a, 0x34, 0x2c, 0x22, 0x2c
};

const uint8_t PUp_VGrow_FreqData[0x20] = {
    0x14, 0x04, 0x22, 0x24, 0x16, 0x04, 0x24, 0x26, // used by both
    0x18, 0x04, 0x26, 0x28, 0x1a, 0x04, 0x28, 0x2a,
    0x1c, 0x04, 0x2a, 0x2c, 0x1e, 0x04, 0x2c, 0x2e, // used by vinegrow
    0x20, 0x04, 0x2e, 0x30, 0x22, 0x04, 0x30, 0x32
};

// $f57c
void Square2SfxHandler(void) {
    uint8_t a, x, y;
    bool n, z, c;

    goto Square2SfxHandler;

PlayCoinGrab:
    lda(0x35);                      // load length of coin grab sound
    ldx(0x8d);                      // and part of reg contents
    bne(CGrab_TTickRegL);

PlayTimerTick:
    lda(0x06);                      // load length of timer tick sound
    ldx(0x98);                      // and part of reg contents

CGrab_TTickRegL:
    sta(*Squ2_SfxLenCounter); 
    ldy(0x7f);                      // load the rest of reg contents 
    lda(0x42);                      // of coin grab and timer tick sound
    PlaySqu2Sfx(a, x, y);

ContinueCGrabTTick:
    lda(*Squ2_SfxLenCounter);       // check for time to play second tone yet
    cmp(0x30);                      // timer tick sound also executes this, not sure why
    bne(N2Tone);
    lda(0x54);                      // if so, load the tone directly into the reg
    apu_write_sq2_lo(a);
N2Tone:
    bne(DecrementSfx2Length);

PlayBlast:
    lda(0x20);                      // load length of fireworks/gunfire sound
    sta(*Squ2_SfxLenCounter);
    ldy(0x94);                      // load reg contents of fireworks/gunfire sound
    lda(0x5e);
    bne(SBlasJ);

ContinueBlast:
    lda(*Squ2_SfxLenCounter);       // check for time to play second part
    cmp(0x18);
    bne(DecrementSfx2Length);
    ldy(0x93);                      // load second part reg contents then
    lda(0x18);
SBlasJ:
    bne(BlstSJp);                   // unconditional branch to load rest of reg contents

PlayPowerUpGrab:
    lda(0x36);                      // load length of power-up grab sound
    sta(*Squ2_SfxLenCounter);

ContinuePowerUpGrab:   
    lda(*Squ2_SfxLenCounter);       // load frequency reg based on length left over
    lsr(a);                         // divide by 2
    bcs(DecrementSfx2Length);       // alter frequency every other frame
    tay();
    lda(PowerUpGrabFreqData[y-1]);  // use length left over / 2 for frequency offset
    ldx(0x5d);                      // store reg contents of power-up grab sound
    ldy(0x7f);

LoadSqu2Regs:
    PlaySqu2Sfx(a, x, y);

DecrementSfx2Length:
    dec(*Squ2_SfxLenCounter);       // decrement length of sfx
    bne(ExSfx2);

EmptySfx2Buffer:
    ldx(0x00);                      // initialize square 2's sound effects buffer
    stx(*Square2SoundBuffer);

    StopSquare2Sfx();
ExSfx2:
    return;

Square2SfxHandler:
    lda(*Square2SoundBuffer);       // special handling for the 1-up sound to keep it
    and(Sfx_ExtraLife);             // from being interrupted by other sounds on square 2
    bne(ContinueExtraLife);
    ldy(*Square2SoundQueue);        // check for sfx in queue
    beq(CheckSfx2Buffer);
    sty(*Square2SoundBuffer);       // if found, put in buffer and check for the following
    bmi(PlayBowserFall);            // bowser fall
    lsr(*Square2SoundQueue);
    bcs(PlayCoinGrab);              // coin grab
    lsr(*Square2SoundQueue);
    bcs(PlayGrowPowerUp);           // power-up reveal
    lsr(*Square2SoundQueue);
    bcs(PlayGrowVine);              // vine grow
    lsr(*Square2SoundQueue);
    bcs(PlayBlast);                 // fireworks/gunfire
    lsr(*Square2SoundQueue);
    bcs(PlayTimerTick);             // timer tick
    lsr(*Square2SoundQueue);
    bcs(PlayPowerUpGrab);           // power-up grab
    lsr(*Square2SoundQueue);
    bcs(PlayExtraLife);             // 1-up

CheckSfx2Buffer:
    lda(*Square2SoundBuffer);       // check for sfx in buffer
    beq(ExS2H);                     // if not found, exit sub
    bmi(ContinueBowserFall);        // bowser fall
    lsr(a);
    bcs(Cont_CGrab_TTick);          // coin grab
    lsr(a);
    bcs(ContinueGrowItems);         // power-up reveal
    lsr(a);
    bcs(ContinueGrowItems);         // vine grow
    lsr(a);
    bcs(ContinueBlast);             // fireworks/gunfire
    lsr(a);
    bcs(Cont_CGrab_TTick);          // timer tick
    lsr(a);
    bcs(ContinuePowerUpGrab);       // power-up grab
    lsr(a);
    bcs(ContinueExtraLife);         // 1-up
ExS2H:
    return;

Cont_CGrab_TTick:
    jmp(ContinueCGrabTTick);

JumpToDecLength2:
    jmp(DecrementSfx2Length);

PlayBowserFall:    
    lda(0x38);                      // load length of bowser defeat sound
    sta(*Squ2_SfxLenCounter);
    ldy(0xc4);                      // load contents of reg for bowser defeat sound
    lda(0x18);
BlstSJp:
    bne(PBFRegs);

ContinueBowserFall:
    lda(*Squ2_SfxLenCounter);       // check for almost near the end
    cmp(0x08);
    bne(DecrementSfx2Length);
    ldy(0xa4);                      // if so, load the rest of reg contents for bowser defeat sound
    lda(0x5a);
PBFRegs:
    ldx(0x9f);                      // the fireworks/gunfire sound shares part of reg contents here
EL_LRegs:
    bne(LoadSqu2Regs);              // this is an unconditional branch outta here

PlayExtraLife:
    lda(0x30);                      // load length of 1-up sound
    sta(*Squ2_SfxLenCounter);

ContinueExtraLife:
    lda(*Squ2_SfxLenCounter);   
    ldx(0x03);                      // load new tones only every eight frames
DivLLoop:
    lsr(a);
    bcs(JumpToDecLength2);          // if any bits set here, branch to dec the length
    dex();
    bne(DivLLoop);                  // do this until all bits checked, if none set, continue
    tay();
    lda(ExtraLifeFreqData[y-1]);    // load our reg contents
    ldx(0x82);
    ldy(0x7f);
    bne(EL_LRegs);                  // unconditional branch

PlayGrowPowerUp:
    lda(0x10);                      // load length of power-up reveal sound
    bne(GrowItemRegs);

PlayGrowVine:
    lda(0x20);                      // load length of vine grow sound

GrowItemRegs:
    sta(*Squ2_SfxLenCounter);
    lda(0x7f);                      // load contents of reg for both sounds directly
    apu_write_sq2_sweep(a);
    lda(0x00);                      // start secondary counter for both sounds
    sta(*Sfx_SecondaryCounter);

ContinueGrowItems:
    inc(*Sfx_SecondaryCounter);     // increment secondary counter for both sounds
    lda(*Sfx_SecondaryCounter);     // this sound doesn't decrement the usual counter
    lsr(a);                         // divide by 2 to get the offset
    tay();
    cpy(*Squ2_SfxLenCounter);       // have we reached the end yet?
    beq(StopGrowItems);             // if so, branch to jump, and stop playing sounds
    lda(0x9d);                      // load contents of other reg directly
    apu_write_sq2_vol(a);
    lda(PUp_VGrow_FreqData[y]);     // use secondary counter / 2 as offset for frequency regs
    SetFreq_Squ2(&a, &x, &y, &z);
    return;

StopGrowItems:
    jmp(EmptySfx2Buffer);           // branch to stop playing sounds
}

void StopSquare2Sfx(void) {
    uint8_t x;
    bool n, z;

    ldx(0x0d);              // stop playing the sfx
    apu_write_snd_chn(x); 
    ldx(0x0f);
    apu_write_snd_chn(x);
ExSfx2:
    // rts
}

const uint8_t BrickShatterFreqData[0x10] = {
    0x01, 0x0e, 0x0e, 0x0d, 0x0b, 0x06, 0x0c, 0x0f,
    0x0a, 0x09, 0x03, 0x0d, 0x08, 0x0d, 0x06, 0x0c
};

// $f667
void NoiseSfxHandler(void) {
    uint8_t a, x, y;
    bool n, z, c;

    goto NoiseSfxHandler;

PlayBrickShatter:
    lda(0x20);                      // load length of brick shatter sound
    sta(*Noise_SfxLenCounter);

ContinueBrickShatter:
    lda(*Noise_SfxLenCounter);  
    lsr(a);                         // divide by 2 and check for bit set to use offset
    bcc(DecrementSfx3Length);
    tay();
    ldx(BrickShatterFreqData[y]);   // load reg contents of brick shatter sound
    lda(BrickShatterEnvData[y]);

PlayNoiseSfx:
    apu_write_noise_vol(a);         // play the sfx
    apu_write_noise_lo(x);
    lda(0x18);
    apu_write_noise_hi(a);

DecrementSfx3Length:
    dec(*Noise_SfxLenCounter);      // decrement length of sfx
    bne(ExSfx3);
    lda(0xf0);                      // if done, stop playing the sfx
    apu_write_noise_vol(a);
    lda(0x00);
    sta(*NoiseSoundBuffer);
ExSfx3:
    return;

NoiseSfxHandler:
    ldy(*NoiseSoundQueue);          // check for sfx in queue
    beq(CheckNoiseBuffer);
    sty(*NoiseSoundBuffer);         // if found, put in buffer
    lsr(*NoiseSoundQueue);
    bcs(PlayBrickShatter);          // brick shatter
    lsr(*NoiseSoundQueue);
    bcs(PlayBowserFlame);           // bowser flame

CheckNoiseBuffer:
    lda(*NoiseSoundBuffer);         // check for sfx in buffer
    beq(ExNH);                      // if not found, exit sub
    lsr(a);
    bcs(ContinueBrickShatter);      // brick shatter
    lsr(a);
    bcs(ContinueBowserFlame);       // bowser flame
ExNH:
    return;

PlayBowserFlame:
    lda(0x40);                      // load length of bowser flame sound
    sta(*Noise_SfxLenCounter);

ContinueBowserFlame:
    lda(*Noise_SfxLenCounter);
    lsr(a);
    tay();
    ldx(0x0f);                      // load reg contents of bowser flame sound
    lda(BowserFlameEnvData[y-1]);
    bne(PlayNoiseSfx);              // unconditional branch here
}

// $f694
void MusicHandler(void) {
    uint8_t a, x, y;
    bool n, z, c;

    goto MusicHandler;

ContinueMusic:
    jmp(HandleSquare2Music);        // if we have music, start with square 2 channel

MusicHandler:
    lda(*EventMusicQueue);          // check event music queue
    bne(LoadEventMusic);
    lda(*AreaMusicQueue);           // check area music queue
    bne(LoadAreaMusic);
    lda(*EventMusicBuffer);         // check both buffers
    ora(*AreaMusicBuffer);
    bne(ContinueMusic); 
    return;                         // no music, then leave

LoadEventMusic:
    sta(*EventMusicBuffer);         // copy event music queue contents to buffer
    cmp(DeathMusic);                // is it death music?
    bne(NoStopSfx);                 // if not, jump elsewhere
    StopSquare1Sfx();               // stop sfx in square 1 and 2
    StopSquare2Sfx();               // but clear only square 1's sfx buffer
NoStopSfx:
    ldx(*AreaMusicBuffer);
    stx(*AreaMusicBuffer_Alt);      // save current area music buffer to be re-obtained later
    ldy(0x00);
    sty(*NoteLengthTblAdder);       // default value for additional length byte offset
    sty(*AreaMusicBuffer);          // clear area music buffer
    cmp(TimeRunningOutMusic);       // is it time running out music?
    bne(FindEventMusicHeader);
    ldx(0x08);                      // load offset to be added to length byte of header
    stx(*NoteLengthTblAdder);
    bne(FindEventMusicHeader);      // unconditional branch

LoadAreaMusic:
    cmp(0x04);                      // is it underground music?
    bne(NoStop1);                   // no, do not stop square 1 sfx
    StopSquare1Sfx();
NoStop1:
    ldy(0x10);                      // start counter used only by ground level music
GMLoopB:
    sty(*GroundMusicHeaderOfs);

HandleAreaMusicLoopB:
    ldy(0x00);                      // clear event music buffer
    sty(*EventMusicBuffer);
    sta(*AreaMusicBuffer);          // copy area music queue contents to buffer
    cmp(0x01);                      // is it ground level music?
    bne(FindAreaMusicHeader);
    inc(*GroundMusicHeaderOfs);     // increment but only if playing ground level music
    ldy(*GroundMusicHeaderOfs);     // is it time to loopback ground level music?
    cpy(0x32);
    bne(LoadHeader);                // branch ahead with alternate offset
    ldy(0x11);
    bne(GMLoopB);                   // unconditional branch

FindAreaMusicHeader:
    ldy(0x08);                      // load Y for offset of area music
    sty(*MusicOffset_Square2);      // residual instruction here

FindEventMusicHeader:
    iny();                          // increment Y pointer based on previously loaded queue contents
    lsr(a);                         // bit shift and increment until we find a set bit for music
    bcc(FindEventMusicHeader);

LoadHeader:
    lda(MusicHeaderOffsetData[y]);  // load offset for header
    tay();
    lda(MusicHeaderData[y]);        // now load the header
    sta(*NoteLenLookupTblOfs);
    lda(MusicHeaderData[y+1]);
    sta(*MusicDataLow);
    lda(MusicHeaderData[y+2]);
    sta(*MusicDataHigh);
    switch (*MusicDataLow|(*MusicDataHigh<<8)) {
        case 0xfc72: MusicData = TimeRunOutMusData; break;
        case 0xf9b8: MusicData = Star_CloudMData; break;
        case 0xfcb0: MusicData = WinLevelMusData; break;
        case 0xfd11: MusicData = UndergroundMusData; break;
        case 0xfa1c: MusicData = SilenceData; break;
        case 0xfba4: MusicData = CastleMusData; break;
        case 0xfec8: MusicData = VictoryMusData; break;
        case 0xfc45: MusicData = GameOverMusData; break;
        case 0xfd52: MusicData = WaterMusData; break;
        case 0xfe51: MusicData = EndOfCastleMusData; break;
        case 0xfa01: MusicData = GroundM_P1Data; break;
        case 0xfa49: MusicData = GroundM_P2AData; break;
        case 0xfa75: MusicData = GroundM_P2BData; break;
        case 0xfa9d: MusicData = GroundM_P2CData; break;
        case 0xfac2: MusicData = GroundM_P3AData; break;
        case 0xfadb: MusicData = GroundM_P3BData; break;
        case 0xfaf9: MusicData = GroundMLdInData; break;
        case 0xfb25: MusicData = GroundM_P4AData; break;
        case 0xfb4b: MusicData = GroundM_P4BData; break;
        case 0xfb74: MusicData = GroundM_P4CData; break;
        case 0xfb72: MusicData = DeathMusData; break;
    }
    lda(MusicHeaderData[y+3]);
    sta(*MusicOffset_Triangle);
    lda(MusicHeaderData[y+4]);
    sta(*MusicOffset_Square1);
    lda(MusicHeaderData[y+5]);
    sta(*MusicOffset_Noise);
    sta(*NoiseDataLoopbackOfs);
    lda(0x01);                      // initialize music note counters
    sta(*Squ2_NoteLenCounter);
    sta(*Squ1_NoteLenCounter);
    sta(*Tri_NoteLenCounter);
    sta(*Noise_BeatLenCounter);
    lda(0x00);                      // initialize music data offset for square 2
    sta(*MusicOffset_Square2);
    sta(*AltRegContentFlag);        // initialize alternate control reg data used by square 1
    lda(0x0b);                      // disable triangle channel and reenable it
    apu_write_snd_chn(a);
    lda(0x0f);
    apu_write_snd_chn(a);

HandleSquare2Music:
    dec(*Squ2_NoteLenCounter);      // decrement square 2 note length
    bne(MiscSqu2MusicTasks);        // is it time for more data?  if not, branch to end tasks
    ldy(*MusicOffset_Square2);      // increment square 2 music offset and fetch data
    inc(*MusicOffset_Square2);
    lda(MusicData[y]);
    beq(EndOfMusicData);            // if zero, the data is a null terminator
    bpl(Squ2NoteHandler);           // if non-negative, data is a note
    bne(Squ2LengthHandler);         // otherwise it is length data

EndOfMusicData:
    lda(*EventMusicBuffer);         // check secondary buffer for time running out music
    cmp(TimeRunningOutMusic);
    bne(NotTRO);
    lda(*AreaMusicBuffer_Alt);      // load previously saved contents of primary buffer
    bne(MusicLoopBack);             // and start playing the song again if there is one
NotTRO:
    and(VictoryMusic);              // check for victory music (the only secondary that loops)
    bne(VictoryMLoopBack);
    lda(*AreaMusicBuffer);          // check primary buffer for any music except pipe intro
    and(0x5f);
    bne(MusicLoopBack);             // if any area music except pipe intro, music loops
    lda(0x00);                      // clear primary and secondary buffers and initialize
    sta(*AreaMusicBuffer);          // control regs of square and triangle channels
    sta(*EventMusicBuffer);
    apu_write_tri_linear(a);
    lda(0x90);    
    apu_write_sq1_vol(a);
    apu_write_sq2_vol(a);
    return;

MusicLoopBack:
    jmp(HandleAreaMusicLoopB);

VictoryMLoopBack:
    jmp(LoadEventMusic);

Squ2LengthHandler:
    a = ProcessLengthData(a);       // store length of note
    sta(*Squ2_NoteLenBuffer);
    ldy(*MusicOffset_Square2);      // fetch another byte (MUST NOT BE LENGTH BYTE!)
    inc(*MusicOffset_Square2);
    lda(MusicData[y]);

Squ2NoteHandler:
    ldx(*Square2SoundBuffer);       // is there a sound playing on this channel?
    bne(SkipFqL1);
    SetFreq_Squ2(&a, &x, &y, &z);   // no, then play the note
    beq(Rest);                      // check to see if note is rest
    LoadControlRegs(&a, &x, &y);    // if not, load control regs for square 2
Rest:
    sta(*Squ2_EnvelopeDataCtrl);    // save contents of A
    Dump_Sq2_Regs(x, y);            // dump X and Y into square 2 control regs
SkipFqL1:
    lda(*Squ2_NoteLenBuffer);       // save length in square 2 note counter
    sta(*Squ2_NoteLenCounter);

MiscSqu2MusicTasks:
    lda(*Square2SoundBuffer);       // is there a sound playing on square 2?
    bne(HandleSquare1Music);
    lda(*EventMusicBuffer);         // check for death music or d4 set on secondary buffer
    and(0x91);                      // note that regs for death music or d4 are loaded by default
    bne(HandleSquare1Music);
    ldy(*Squ2_EnvelopeDataCtrl);    // check for contents saved from LoadControlRegs
    beq(NoDecEnv1);
    dec(*Squ2_EnvelopeDataCtrl);    // decrement unless already zero
NoDecEnv1:
    a = LoadEnvelopeData(y);        // do a load of envelope data to replace default
    apu_write_sq2_vol(a);           // based on offset set by first load unless playing
    ldx(0x7f);                      // death music or d4 set on secondary buffer
    apu_write_sq2_sweep(x);

HandleSquare1Music:
    ldy(*MusicOffset_Square1);      // is there a nonzero offset here?
    beq(HandleTriangleMusic);       // if not, skip ahead to the triangle channel
    dec(*Squ1_NoteLenCounter);      // decrement square 1 note length
    bne(MiscSqu1MusicTasks);        // is it time for more data?

FetchSqu1MusicData:
    ldy(*MusicOffset_Square1);      // increment square 1 music offset and fetch data
    inc(*MusicOffset_Square1);
    lda(MusicData[y]);
    bne(Squ1NoteHandler);           // if nonzero, then skip this part
    lda(0x83);
    apu_write_sq1_vol(a);           // store some data into control regs for square 1
    lda(0x94);                      // and fetch another byte of data, used to give
    apu_write_sq1_sweep(a);         // death music its unique sound
    sta(*AltRegContentFlag);
    bne(FetchSqu1MusicData);        // unconditional branch

Squ1NoteHandler:
    AlternateLengthHandler(&a, &x, c);
    sta(*Squ1_NoteLenCounter);      // save contents of A in square 1 note counter
    ldy(*Square1SoundBuffer);       // is there a sound playing on square 1?
    bne(HandleTriangleMusic);
    txa();
    and(0x3e);                      // change saved data to appropriate note format
    SetFreq_Squ1(&a, &x, &y, &z);   // play the note
    beq(SkipCtrlL);
    LoadControlRegs(&a, &x, &y);
SkipCtrlL:
    sta(*Squ1_EnvelopeDataCtrl);    // save envelope offset
    Dump_Squ1_Regs(x, y);

MiscSqu1MusicTasks:
    lda(*Square1SoundBuffer);       // is there a sound playing on square 1?
    bne(HandleTriangleMusic);
    lda(*EventMusicBuffer);         // check for death music or d4 set on secondary buffer
    and(0x91);
    bne(DeathMAltReg);
    ldy(*Squ1_EnvelopeDataCtrl);    // check saved envelope offset
    beq(NoDecEnv2);
    dec(*Squ1_EnvelopeDataCtrl);    // decrement unless already zero
NoDecEnv2:
    a = LoadEnvelopeData(y);        // do a load of envelope data
    apu_write_sq1_vol(a);           // based on offset set by first load
DeathMAltReg:
    lda(*AltRegContentFlag);        // check for alternate control reg data
    bne(DoAltLoad);
    lda(0x7f);                      // load this value if zero, the alternate value
DoAltLoad:
    apu_write_sq1_sweep(a);         // if nonzero, and let's move on

HandleTriangleMusic:
    lda(*MusicOffset_Triangle);
    dec(*Tri_NoteLenCounter);       // decrement triangle note length
    bne(HandleNoiseMusic);          // is it time for more data?
    ldy(*MusicOffset_Triangle);     // increment square 1 music offset and fetch data
    inc(*MusicOffset_Triangle);
    lda(MusicData[y]);
    beq(LoadTriCtrlReg);            // if zero, skip all this and move on to noise 
    bpl(TriNoteHandler);            // if non-negative, data is note
    a = ProcessLengthData(a);       // otherwise, it is length data
    sta(*Tri_NoteLenBuffer);        // save contents of A
    lda(0x1f);
    apu_write_tri_linear(a);        // load some default data for triangle control reg
    ldy(*MusicOffset_Triangle);     // fetch another byte
    inc(*MusicOffset_Triangle);
    lda(MusicData[y]);
    beq(LoadTriCtrlReg);            // check once more for nonzero data

TriNoteHandler:
    SetFreq_Tri(a);
    ldx(*Tri_NoteLenBuffer);        // save length in triangle note counter
    stx(*Tri_NoteLenCounter);
    lda(*EventMusicBuffer);
    and(0x6e);                      // check for death music or d4 set on secondary buffer
    bne(NotDOrD4);                  // if playing any other secondary, skip primary buffer check
    lda(*AreaMusicBuffer);          // check primary buffer for water or castle level music
    and(0x0a);
    beq(HandleNoiseMusic);          // if playing any other primary, or death or d4, go on to noise routine
NotDOrD4:
    txa();                          // if playing water or castle music or any secondary
    cmp(0x12);                      // besides death music or d4 set, check length of note
    bcs(LongN);
    lda(*EventMusicBuffer);         // check for win castle music again if not playing a long note
    and(EndOfCastleMusic);
    beq(MediN);
    lda(0x0f);                      // load value $0f if playing the win castle music and playing a short
    bne(LoadTriCtrlReg);            // note, load value $1f if playing water or castle level music or any
MediN:
    lda(0x1f);                      // secondary besides death and d4 except win castle or win castle and playing
    bne(LoadTriCtrlReg);            // a short note, and load value $ff if playing a long note on water, castle
LongN:
    lda(0xff);                      // or any secondary (including win castle) except death and d4

LoadTriCtrlReg:           
    apu_write_tri_linear(a);        // save final contents of A into control reg for triangle

HandleNoiseMusic:
    lda(*AreaMusicBuffer);          // check if playing underground or castle music
    and(0xf3);
    beq(ExitMusicHandler);          // if so, skip the noise routine
    dec(*Noise_BeatLenCounter);     // decrement noise beat length
    bne(ExitMusicHandler);          // is it time for more data?

FetchNoiseBeatData:
    ldy(*MusicOffset_Noise);        // increment noise beat offset and fetch data
    inc(*MusicOffset_Noise);
    lda(MusicData[y]);              // get noise beat data, if nonzero, branch to handle
    bne(NoiseBeatHandler);
    lda(*NoiseDataLoopbackOfs);     // if data is zero, reload original noise beat offset
    sta(*MusicOffset_Noise);        // and loopback next time around
    bne(FetchNoiseBeatData);       // unconditional branch

NoiseBeatHandler:
    AlternateLengthHandler(&a, &x, c);
    sta(*Noise_BeatLenCounter);     // store length in noise beat counter
    txa();
    and(0x3e);                      // reload data and erase length bits
    beq(SilentBeat);                // if no beat data, silence
    cmp(0x30);                      // check the beat data and play the appropriate
    beq(LongBeat);                  // noise accordingly
    cmp(0x20);
    beq(StrongBeat);
    and(0x10);  
    beq(SilentBeat);
    lda(0x1c);                      // short beat data
    ldx(0x03);
    ldy(0x18);
    bne(PlayBeat);

StrongBeat:
    lda(0x1c);                      // strong beat data
    ldx(0x0c);
    ldy(0x18);
    bne(PlayBeat);

LongBeat:
    lda(0x1c);                      // long beat data
    ldx(0x03);
    ldy(0x58);
    bne(PlayBeat);

SilentBeat:
    lda(0x10);                      // silence

PlayBeat:
    apu_write_noise_vol(a);         // load beat data into noise regs
    apu_write_noise_lo(x);
    apu_write_noise_hi(y);

ExitMusicHandler:
    return;
}

void AlternateLengthHandler(uint8_t *pa, uint8_t *px, bool c) {
    uint8_t a = *pa, x = *px;
    bool n, z;

    tax();      // save a copy of original byte into X
    ror(a);     // save LSB from original byte into carry
    txa();      // reload original byte and rotate three times
    rol(a);     // turning xx00000x into 00000xxx, with the
    rol(a);     // bit in carry as the MSB here
    rol(a);

    a = ProcessLengthData(a);

    *pa = a, *px = x;
}

uint8_t ProcessLengthData(uint8_t a) {
    uint8_t y;
    bool n, v, z, c;

    and(0x07);                      // clear all but the three LSBs
    clc();
    adc(*NoteLenLookupTblOfs);      // add offset loaded from first header byte
    adc(*NoteLengthTblAdder);       // add extra if time running out music
    tay()
    lda(MusicLengthLookupTbl[y]);   // load length
    return a;
}

void LoadControlRegs(uint8_t *pa, uint8_t *px, uint8_t *py) {
    uint8_t a = *pa, x = *px, y = *py;
    bool n, z;

    lda(*EventMusicBuffer); // check secondary buffer for win castle music
    and(EndOfCastleMusic);
    beq(NotECstlM);
    lda(0x04);              // this value is only used for win castle music
    bne(AllMus);            // unconditional branch
NotECstlM:
    lda(*AreaMusicBuffer);
    and(0x7d);              // check primary buffer for water music
    beq(WaterMus);
    lda(0x08);              // this is the default value for all other music
    bne(AllMus);
WaterMus:
    lda(0x28);              // this value is used for water music and all other event music
AllMus:
    ldx(0x82);              // load contents of other sound regs for square 2
    ldy(0x7f);
    // rts

    *pa = a, *px = x, *py = y;
}

uint8_t LoadEnvelopeData(uint8_t y) {
    uint8_t a;
    bool n, z;
    
    lda(*EventMusicBuffer);             // check secondary buffer for win castle music
    and(EndOfCastleMusic);
    beq(LoadUsualEnvData);
    lda(EndOfCastleMusicEnvData[y]);    // load data from offset for win castle music
    return a;

LoadUsualEnvData:
    lda(*AreaMusicBuffer);              // check primary buffer for water music
    and(0x7d);
    beq(LoadWaterEventMusEnvData);
    lda(AreaMusicEnvData[y]);           // load default data from offset for all other music
    return a;

LoadWaterEventMusEnvData:
    lda(WaterEventMusEnvData[y]);       // load data from offset for water music and all other event music
    return a;
}

const uint8_t MusicHeaderData[0xab] = {
    0xa5,            // event music
    0x59,
    0x54,
    0x64,
    0x59,
    0x3c,
    0x31,
    0x4b,

    0x69,    // area music
    0x5e,
    0x46,
    0x4f,
    0x36,
    0x8d,
    0x36,
    0x4b,

    0x8d,   // ground level music layout
    0x69, 0x69,
    0x6f, 0x75, 0x6f, 0x7b,
    0x6f, 0x75, 0x6f, 0x7b,
    0x81, 0x87, 0x81, 0x8d,
    0x69, 0x69,
    0x93, 0x99, 0x93, 0x9f,
    0x93, 0x99, 0x93, 0x9f,
    0x81, 0x87, 0x81, 0x8d,
    0x93, 0x99, 0x93, 0x9f,

// music headers
// header format is as follows:
// 1 byte - length byte offset
// 2 bytes -  music data address
// 1 byte - triangle data offset
// 1 byte - square 1 data offset
// 1 byte - noise data offset (not used by secondary music)

/* TimeRunningOutHdr */     0x08, 0x72, 0xfc, 0x27, 0x18,
/* Star_CloudHdr */         0x20, 0xb8, 0xf9, 0x2e, 0x1a, 0x40,
/* EndOfLevelMusHdr */      0x20, 0xb0, 0xfc, 0x3d, 0x21,
/* ResidualHeaderData */    0x20, 0xc4, 0xfc, 0x3f, 0x1d,
/* UndergroundMusHdr */     0x18, 0x11, 0xfd, 0x00, 0x00,
/* SilenceHdr */            0x08, 0x1c, 0xfa, 0x00,
/* CastleMusHdr */          0x00, 0xa4, 0xfb, 0x93, 0x62,
/* VictoryMusHdr */         0x10, 0xc8, 0xfe, 0x24, 0x14,
/* GameOverMusHdr */        0x18, 0x45, 0xfc, 0x1e, 0x14,
/* WaterMusHdr */           0x08, 0x52, 0xfd, 0xa0, 0x70, 0x68,
/* WinCastleMusHdr */       0x08, 0x51, 0xfe, 0x4c, 0x24,
/* GroundLevelPart1Hdr */   0x18, 0x01, 0xfa, 0x2d, 0x1c, 0xb8,
/* GroundLevelPart2AHdr */  0x18, 0x49, 0xfa, 0x20, 0x12, 0x70,
/* GroundLevelPart2BHdr */  0x18, 0x75, 0xfa, 0x1b, 0x10, 0x44,
/* GroundLevelPart2CHdr */  0x18, 0x9d, 0xfa, 0x11, 0x0a, 0x1c,
/* GroundLevelPart3AHdr */  0x18, 0xc2, 0xfa, 0x2d, 0x10, 0x58,
/* GroundLevelPart3BHdr */  0x18, 0xdb, 0xfa, 0x14, 0x0d, 0x3f,
/* GroundLevelLeadInHdr */  0x18, 0xf9, 0xfa, 0x15, 0x0d, 0x21,
/* GroundLevelPart4AHdr */  0x18, 0x25, 0xfb, 0x18, 0x10, 0x7a,
/* GroundLevelPart4BHdr */  0x18, 0x4b, 0xfb, 0x19, 0x0f, 0x54,
/* GroundLevelPart4CHdr */  0x18, 0x74, 0xfb, 0x1e, 0x12, 0x2b,
/* DeathMusHdr */           0x18, 0x72, 0xfb, 0x1e, 0x0f, 0x2d
};

// --------------------------------

// MUSIC DATA
// square 2/triangle format
// d7 - length byte flag (0-note, 1-length)
// if d7 is set to 0 and d6-d0 is nonzero:
// d6-d0 - note offset in frequency look-up table (must be even)
// if d7 is set to 1:
// d6-d3 - unused
// d2-d0 - length offset in length look-up table
// value of $00 in square 2 data is used as null terminator, affects all sound channels
// value of $00 in triangle data causes routine to skip note

// square 1 format
// d7-d6, d0 - length offset in length look-up table (bit order is d0,d7,d6)
// d5-d1 - note offset in frequency look-up table
// value of $00 in square 1 data is flag alternate control reg data to be loaded

// noise format
// d7-d6, d0 - length offset in length look-up table (bit order is d0,d7,d6)
// d5-d4 - beat type (0 - rest, 1 - short, 2 - strong, 3 - long)
// d3-d1 - unused
// value of $00 in noise data is used as null terminator, affects only noise

// all music data is organized into sections (unless otherwise stated):
// square 2, square 1, triangle, noise

const uint8_t Star_CloudMData[0x49] = {
    0x84, 0x2c, 0x2c, 0x2c, 0x82, 0x04, 0x2c, 0x04, 0x85, 0x2c, 0x84, 0x2c, 0x2c,
    0x2a, 0x2a, 0x2a, 0x82, 0x04, 0x2a, 0x04, 0x85, 0x2a, 0x84, 0x2a, 0x2a, 0x00,

    0x1f, 0x1f, 0x1f, 0x98, 0x1f, 0x1f, 0x98, 0x9e, 0x98, 0x1f,
    0x1d, 0x1d, 0x1d, 0x94, 0x1d, 0x1d, 0x94, 0x9c, 0x94, 0x1d,

    0x86, 0x18, 0x85, 0x26, 0x30, 0x84, 0x04, 0x26, 0x30,
    0x86, 0x14, 0x85, 0x22, 0x2c, 0x84, 0x04, 0x22, 0x2c,

    0x21, 0xd0, 0xc4, 0xd0, 0x31, 0xd0, 0xc4, 0xd0, 0x00
};

const uint8_t GroundM_P1Data[] = {
    0x85, 0x2c, 0x22, 0x1c, 0x84, 0x26, 0x2a, 0x82, 0x28, 0x26, 0x04,
    0x87, 0x22, 0x34, 0x3a, 0x82, 0x40, 0x04, 0x36, 0x84, 0x3a, 0x34,
    0x82, 0x2c, 0x30, 0x85, 0x2a,

// SilenceData
    0x00,

    0x5d, 0x55, 0x4d, 0x15, 0x19, 0x96, 0x15, 0xd5, 0xe3, 0xeb,
    0x2d, 0xa6, 0x2b, 0x27, 0x9c, 0x9e, 0x59,

    0x85, 0x22, 0x1c, 0x14, 0x84, 0x1e, 0x22, 0x82, 0x20, 0x1e, 0x04, 0x87,
    0x1c, 0x2c, 0x34, 0x82, 0x36, 0x04, 0x30, 0x34, 0x04, 0x2c, 0x04, 0x26,
    0x2a, 0x85, 0x22,

// GroundM_P2AData
    0x84, 0x04, 0x82, 0x3a, 0x38, 0x36, 0x32, 0x04, 0x34,
    0x04, 0x24, 0x26, 0x2c, 0x04, 0x26, 0x2c, 0x30, 0x00,

    0x05, 0xb4, 0xb2, 0xb0, 0x2b, 0xac, 0x84,
    0x9c, 0x9e, 0xa2, 0x84, 0x94, 0x9c, 0x9e,

    0x85, 0x14, 0x22, 0x84, 0x2c, 0x85, 0x1e,
    0x82, 0x2c, 0x84, 0x2c, 0x1e,


// GroundM_P2BData
    0x84, 0x04, 0x82, 0x3a, 0x38, 0x36, 0x32, 0x04, 0x34,
    0x04, 0x64, 0x04, 0x64, 0x86, 0x64, 0x00,

    0x05, 0xb4, 0xb2, 0xb0, 0x2b, 0xac, 0x84,
    0x37, 0xb6, 0xb6, 0x45,

    0x85, 0x14, 0x1c, 0x82, 0x22, 0x84, 0x2c,
    0x4e, 0x82, 0x4e, 0x84, 0x4e, 0x22,

// GroundM_P2CData
    0x84, 0x04, 0x85, 0x32, 0x85, 0x30, 0x86, 0x2c, 0x04, 0x00,

    0x05, 0xa4, 0x05, 0x9e, 0x05, 0x9d, 0x85,

    0x84, 0x14, 0x85, 0x24, 0x28, 0x2c, 0x82,
    0x22, 0x84, 0x22, 0x14,

    0x21, 0xd0, 0xc4, 0xd0, 0x31, 0xd0, 0xc4, 0xd0, 0x00
};

const uint8_t GroundM_P3AData[] = {
    0x82, 0x2c, 0x84, 0x2c, 0x2c, 0x82, 0x2c, 0x30,
    0x04, 0x34, 0x2c, 0x04, 0x26, 0x86, 0x22, 0x00,

    0xa4, 0x25, 0x25, 0xa4, 0x29, 0xa2, 0x1d, 0x9c, 0x95,

// GroundM_P3BData
    0x82, 0x2c, 0x2c, 0x04, 0x2c, 0x04, 0x2c, 0x30, 0x85, 0x34, 0x04, 0x04, 0x00,

    0xa4, 0x25, 0x25, 0xa4, 0xa8, 0x63, 0x04,

    // triangle data used by both sections of third part
    0x85, 0x0e, 0x1a, 0x84, 0x24, 0x85, 0x22, 0x14, 0x84, 0x0c,

// GroundMLdInData
    0x82, 0x34, 0x84, 0x34, 0x34, 0x82, 0x2c, 0x84, 0x34, 0x86, 0x3a, 0x04, 0x00,

    0xa0, 0x21, 0x21, 0xa0, 0x21, 0x2b, 0x05, 0xa3,

    0x82, 0x18, 0x84, 0x18, 0x18, 0x82, 0x18, 0x18, 0x04, 0x86, 0x3a, 0x22,

    // noise data used by lead-in and third part sections
    0x31, 0x90, 0x31, 0x90, 0x31, 0x71, 0x31, 0x90, 0x90, 0x90, 0x00
};

const uint8_t GroundM_P4AData[] = {
    0x82, 0x34, 0x84, 0x2c, 0x85, 0x22, 0x84, 0x24,
    0x82, 0x26, 0x36, 0x04, 0x36, 0x86, 0x26, 0x00,

    0xac, 0x27, 0x5d, 0x1d, 0x9e, 0x2d, 0xac, 0x9f,

    0x85, 0x14, 0x82, 0x20, 0x84, 0x22, 0x2c,
    0x1e, 0x1e, 0x82, 0x2c, 0x2c, 0x1e, 0x04,

// GroundM_P4BData
    0x87, 0x2a, 0x40, 0x40, 0x40, 0x3a, 0x36,
    0x82, 0x34, 0x2c, 0x04, 0x26, 0x86, 0x22, 0x00,

    0xe3, 0xf7, 0xf7, 0xf7, 0xf5, 0xf1, 0xac, 0x27, 0x9e, 0x9d,

    0x85, 0x18, 0x82, 0x1e, 0x84, 0x22, 0x2a,
    0x22, 0x22, 0x82, 0x2c, 0x2c, 0x22, 0x04,

// DeathMusData
    0x86, 0x04, // death music share data with fourth part c of ground level music

// GroundM_P4CData
    0x82, 0x2a, 0x36, 0x04, 0x36, 0x87, 0x36, 0x34, 0x30, 0x86, 0x2c, 0x04, 0x00,
      
    0x00, 0x68, 0x6a, 0x6c, 0x45, // death music only

    0xa2, 0x31, 0xb0, 0xf1, 0xed, 0xeb, 0xa2, 0x1d, 0x9c, 0x95,

    0x86, 0x04, // death music only

    0x85, 0x22, 0x82, 0x22, 0x87, 0x22, 0x26, 0x2a, 0x84, 0x2c, 0x22, 0x86, 0x14,

    // noise data used by fourth part sections
    0x51, 0x90, 0x31, 0x11, 0x00
};

const uint8_t CastleMusData[0xa1] = {
    0x80, 0x22, 0x28, 0x22, 0x26, 0x22, 0x24, 0x22, 0x26,
    0x22, 0x28, 0x22, 0x2a, 0x22, 0x28, 0x22, 0x26,
    0x22, 0x28, 0x22, 0x26, 0x22, 0x24, 0x22, 0x26,
    0x22, 0x28, 0x22, 0x2a, 0x22, 0x28, 0x22, 0x26,
    0x20, 0x26, 0x20, 0x24, 0x20, 0x26, 0x20, 0x28,
    0x20, 0x26, 0x20, 0x28, 0x20, 0x26, 0x20, 0x24,
    0x20, 0x26, 0x20, 0x24, 0x20, 0x26, 0x20, 0x28,
    0x20, 0x26, 0x20, 0x28, 0x20, 0x26, 0x20, 0x24,
    0x28, 0x30, 0x28, 0x32, 0x28, 0x30, 0x28, 0x2e,
    0x28, 0x30, 0x28, 0x2e, 0x28, 0x2c, 0x28, 0x2e,
    0x28, 0x30, 0x28, 0x32, 0x28, 0x30, 0x28, 0x2e,
    0x28, 0x30, 0x28, 0x2e, 0x28, 0x2c, 0x28, 0x2e, 0x00,

    0x04, 0x70, 0x6e, 0x6c, 0x6e, 0x70, 0x72, 0x70, 0x6e,
    0x70, 0x6e, 0x6c, 0x6e, 0x70, 0x72, 0x70, 0x6e,
    0x6e, 0x6c, 0x6e, 0x70, 0x6e, 0x70, 0x6e, 0x6c,
    0x6e, 0x6c, 0x6e, 0x70, 0x6e, 0x70, 0x6e, 0x6c,
    0x76, 0x78, 0x76, 0x74, 0x76, 0x74, 0x72, 0x74,
    0x76, 0x78, 0x76, 0x74, 0x76, 0x74, 0x72, 0x74,

    0x84, 0x1a, 0x83, 0x18, 0x20, 0x84, 0x1e, 0x83, 0x1c, 0x28,
    0x26, 0x1c, 0x1a, 0x1c
};

const uint8_t GameOverMusData[0x2d] = {
    0x82, 0x2c, 0x04, 0x04, 0x22, 0x04, 0x04, 0x84, 0x1c, 0x87,
    0x26, 0x2a, 0x26, 0x84, 0x24, 0x28, 0x24, 0x80, 0x22, 0x00,

    0x9c, 0x05, 0x94, 0x05, 0x0d, 0x9f, 0x1e, 0x9c, 0x98, 0x9d,

    0x82, 0x22, 0x04, 0x04, 0x1c, 0x04, 0x04, 0x84, 0x14,
    0x86, 0x1e, 0x80, 0x16, 0x80, 0x14
};

const uint8_t TimeRunOutMusData[0x3e] = {
    0x81, 0x1c, 0x30, 0x04, 0x30, 0x30, 0x04, 0x1e, 0x32, 0x04, 0x32, 0x32,
    0x04, 0x20, 0x34, 0x04, 0x34, 0x34, 0x04, 0x36, 0x04, 0x84, 0x36, 0x00,

    0x46, 0xa4, 0x64, 0xa4, 0x48, 0xa6, 0x66, 0xa6, 0x4a, 0xa8, 0x68, 0xa8,
    0x6a, 0x44, 0x2b,

    0x81, 0x2a, 0x42, 0x04, 0x42, 0x42, 0x04, 0x2c, 0x64, 0x04, 0x64, 0x64,
    0x04, 0x2e, 0x46, 0x04, 0x46, 0x46, 0x04, 0x22, 0x04, 0x84, 0x22
};

const uint8_t WinLevelMusData[0x61] = {
    0x87, 0x04, 0x06, 0x0c, 0x14, 0x1c, 0x22, 0x86, 0x2c, 0x22,
    0x87, 0x04, 0x60, 0x0e, 0x14, 0x1a, 0x24, 0x86, 0x2c, 0x24,
    0x87, 0x04, 0x08, 0x10, 0x18, 0x1e, 0x28, 0x86, 0x30, 0x30,
    0x80, 0x64, 0x00,

    0xcd, 0xd5, 0xdd, 0xe3, 0xed, 0xf5, 0xbb, 0xb5, 0xcf, 0xd5,
    0xdb, 0xe5, 0xed, 0xf3, 0xbd, 0xb3, 0xd1, 0xd9, 0xdf, 0xe9,
    0xf1, 0xf7, 0xbf, 0xff, 0xff, 0xff, 0x34,
    0x00, // unused byte

    0x86, 0x04, 0x87, 0x14, 0x1c, 0x22, 0x86, 0x34, 0x84, 0x2c,
    0x04, 0x04, 0x04, 0x87, 0x14, 0x1a, 0x24, 0x86, 0x32, 0x84,
    0x2c, 0x04, 0x86, 0x04, 0x87, 0x18, 0x1e, 0x28, 0x86, 0x36,
    0x87, 0x30, 0x30, 0x30, 0x80, 0x2c
};

// square 2 and triangle use the same data, square 1 is unused
const uint8_t UndergroundMusData[0x41] = {
    0x82, 0x14, 0x2c, 0x62, 0x26, 0x10, 0x28, 0x80, 0x04,
    0x82, 0x14, 0x2c, 0x62, 0x26, 0x10, 0x28, 0x80, 0x04,
    0x82, 0x08, 0x1e, 0x5e, 0x18, 0x60, 0x1a, 0x80, 0x04,
    0x82, 0x08, 0x1e, 0x5e, 0x18, 0x60, 0x1a, 0x86, 0x04,
    0x83, 0x1a, 0x18, 0x16, 0x84, 0x14, 0x1a, 0x18, 0x0e, 0x0c,
    0x16, 0x83, 0x14, 0x20, 0x1e, 0x1c, 0x28, 0x26, 0x87,
    0x24, 0x1a, 0x12, 0x10, 0x62, 0x0e, 0x80, 0x04, 0x04,
    0x00
};

// noise data directly follows square 2 here unlike in other songs
const uint8_t WaterMusData[0xff] = {
    0x82, 0x18, 0x1c, 0x20, 0x22, 0x26, 0x28,
    0x81, 0x2a, 0x2a, 0x2a, 0x04, 0x2a, 0x04, 0x83, 0x2a, 0x82, 0x22,
    0x86, 0x34, 0x32, 0x34, 0x81, 0x04, 0x22, 0x26, 0x2a, 0x2c, 0x30,
    0x86, 0x34, 0x83, 0x32, 0x82, 0x36, 0x84, 0x34, 0x85, 0x04, 0x81, 0x22,
    0x86, 0x30, 0x2e, 0x30, 0x81, 0x04, 0x22, 0x26, 0x2a, 0x2c, 0x2e,
    0x86, 0x30, 0x83, 0x22, 0x82, 0x36, 0x84, 0x34, 0x85, 0x04, 0x81, 0x22,
    0x86, 0x3a, 0x3a, 0x3a, 0x82, 0x3a, 0x81, 0x40, 0x82, 0x04, 0x81, 0x3a,
    0x86, 0x36, 0x36, 0x36, 0x82, 0x36, 0x81, 0x3a, 0x82, 0x04, 0x81, 0x36,
    0x86, 0x34, 0x82, 0x26, 0x2a, 0x36,
    0x81, 0x34, 0x34, 0x85, 0x34, 0x81, 0x2a, 0x86, 0x2c, 0x00,

    0x84, 0x90, 0xb0, 0x84, 0x50, 0x50, 0xb0, 0x00,

    0x98, 0x96, 0x94, 0x92, 0x94, 0x96, 0x58, 0x58, 0x58, 0x44,
    0x5c, 0x44, 0x9f, 0xa3, 0xa1, 0xa3, 0x85, 0xa3, 0xe0, 0xa6,
    0x23, 0xc4, 0x9f, 0x9d, 0x9f, 0x85, 0x9f, 0xd2, 0xa6, 0x23,
    0xc4, 0xb5, 0xb1, 0xaf, 0x85, 0xb1, 0xaf, 0xad, 0x85, 0x95,
    0x9e, 0xa2, 0xaa, 0x6a, 0x6a, 0x6b, 0x5e, 0x9d,

    0x84, 0x04, 0x04, 0x82, 0x22, 0x86, 0x22,
    0x82, 0x14, 0x22, 0x2c, 0x12, 0x22, 0x2a, 0x14, 0x22, 0x2c,
    0x1c, 0x22, 0x2c, 0x14, 0x22, 0x2c, 0x12, 0x22, 0x2a, 0x14,
    0x22, 0x2c, 0x1c, 0x22, 0x2c, 0x18, 0x22, 0x2a, 0x16, 0x20,
    0x28, 0x18, 0x22, 0x2a, 0x12, 0x22, 0x2a, 0x18, 0x22, 0x2a,
    0x12, 0x22, 0x2a, 0x14, 0x22, 0x2c, 0x0c, 0x22, 0x2c, 0x14, 0x22, 0x34, 0x12,
    0x22, 0x30, 0x10, 0x22, 0x2e, 0x16, 0x22, 0x34, 0x18, 0x26,
    0x36, 0x16, 0x26, 0x36, 0x14, 0x26, 0x36, 0x12, 0x22, 0x36,
    0x5c, 0x22, 0x34, 0x0c, 0x22, 0x22, 0x81, 0x1e, 0x1e, 0x85, 0x1e,
    0x81, 0x12, 0x86, 0x14
};

const uint8_t EndOfCastleMusData[0x77] = {
    0x81, 0x2c, 0x22, 0x1c, 0x2c, 0x22, 0x1c, 0x85, 0x2c, 0x04,
    0x81, 0x2e, 0x24, 0x1e, 0x2e, 0x24, 0x1e, 0x85, 0x2e, 0x04,
    0x81, 0x32, 0x28, 0x22, 0x32, 0x28, 0x22, 0x85, 0x32,
    0x87, 0x36, 0x36, 0x36, 0x84, 0x3a, 0x00,

    0x5c, 0x54, 0x4c, 0x5c, 0x54, 0x4c,
    0x5c, 0x1c, 0x1c, 0x5c, 0x5c, 0x5c, 0x5c,
    0x5e, 0x56, 0x4e, 0x5e, 0x56, 0x4e,
    0x5e, 0x1e, 0x1e, 0x5e, 0x5e, 0x5e, 0x5e,
    0x62, 0x5a, 0x50, 0x62, 0x5a, 0x50,
    0x62, 0x22, 0x22, 0x62, 0xe7, 0xe7, 0xe7, 0x2b,

    0x86, 0x14, 0x81, 0x14, 0x80, 0x14, 0x14, 0x81, 0x14, 0x14, 0x14, 0x14,
    0x86, 0x16, 0x81, 0x16, 0x80, 0x16, 0x16, 0x81, 0x16, 0x16, 0x16, 0x16,
    0x81, 0x28, 0x22, 0x1a, 0x28, 0x22, 0x1a, 0x28, 0x80, 0x28, 0x28,
    0x81, 0x28, 0x87, 0x2c, 0x2c, 0x2c, 0x84, 0x30
};

const uint8_t VictoryMusData[0x35] = {
    0x83, 0x04, 0x84, 0x0c, 0x83, 0x62, 0x10, 0x84, 0x12,
    0x83, 0x1c, 0x22, 0x1e, 0x22, 0x26, 0x18, 0x1e, 0x04, 0x1c, 0x00,

    0xe3, 0xe1, 0xe3, 0x1d, 0xde, 0xe0, 0x23,
    0xec, 0x75, 0x74, 0xf0, 0xf4, 0xf6, 0xea, 0x31, 0x2d,

    0x83, 0x12, 0x14, 0x04, 0x18, 0x1a, 0x1c, 0x14,
    0x26, 0x22, 0x1e, 0x1c, 0x18, 0x1e, 0x22, 0x0c, 0x14
};

const uint8_t FreqRegLookupTbl[0x66] = {
    0x00, 0x88, 0x00, 0x2f, 0x00, 0x00,
    0x02, 0xa6, 0x02, 0x80, 0x02, 0x5c, 0x02, 0x3a,
    0x02, 0x1a, 0x01, 0xdf, 0x01, 0xc4, 0x01, 0xab,
    0x01, 0x93, 0x01, 0x7c, 0x01, 0x67, 0x01, 0x53,
    0x01, 0x40, 0x01, 0x2e, 0x01, 0x1d, 0x01, 0x0d,
    0x00, 0xfe, 0x00, 0xef, 0x00, 0xe2, 0x00, 0xd5,
    0x00, 0xc9, 0x00, 0xbe, 0x00, 0xb3, 0x00, 0xa9,
    0x00, 0xa0, 0x00, 0x97, 0x00, 0x8e, 0x00, 0x86,
    0x00, 0x77, 0x00, 0x7e, 0x00, 0x71, 0x00, 0x54,
    0x00, 0x64, 0x00, 0x5f, 0x00, 0x59, 0x00, 0x50,
    0x00, 0x47, 0x00, 0x43, 0x00, 0x3b, 0x00, 0x35,
    0x00, 0x2a, 0x00, 0x23, 0x04, 0x75, 0x03, 0x57,
    0x02, 0xf9, 0x02, 0xcf, 0x01, 0xfc, 0x00, 0x6a
};

const uint8_t MusicLengthLookupTbl[0x30] = {
    0x05, 0x0a, 0x14, 0x28, 0x50, 0x1e, 0x3c, 0x02,
    0x04, 0x08, 0x10, 0x20, 0x40, 0x18, 0x30, 0x0c,
    0x03, 0x06, 0x0c, 0x18, 0x30, 0x12, 0x24, 0x08,
    0x36, 0x03, 0x09, 0x06, 0x12, 0x1b, 0x24, 0x0c,
    0x24, 0x02, 0x06, 0x04, 0x0c, 0x12, 0x18, 0x08,
    0x12, 0x01, 0x03, 0x02, 0x06, 0x09, 0x0c, 0x04
};

const uint8_t EndOfCastleMusicEnvData[0x4] = {
    0x98, 0x99, 0x9a, 0x9b
};

const uint8_t AreaMusicEnvData[0x8] = {
    0x90, 0x94, 0x94, 0x95, 0x95, 0x96, 0x97, 0x98
};

const uint8_t WaterEventMusEnvData[0x28] = {
    0x90, 0x91, 0x92, 0x92, 0x93, 0x93, 0x93, 0x94,
    0x94, 0x94, 0x94, 0x94, 0x94, 0x95, 0x95, 0x95,
    0x95, 0x95, 0x95, 0x96, 0x96, 0x96, 0x96, 0x96,
    0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96, 0x96,
    0x96, 0x96, 0x96, 0x96, 0x95, 0x95, 0x94, 0x93
};

const uint8_t BowserFlameEnvData[0x20] = {
    0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x19, 0x19,
    0x1a, 0x1a, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1f,
    0x1f, 0x1f, 0x1f, 0x1e, 0x1d, 0x1c, 0x1e, 0x1f,
    0x1f, 0x1e, 0x1d, 0x1c, 0x1a, 0x18, 0x16, 0x14
};

const uint8_t BrickShatterEnvData[0x10] = {
    0x15, 0x16, 0x16, 0x17, 0x17, 0x18, 0x19, 0x19,
    0x1a, 0x1a, 0x1c, 0x1d, 0x1d, 0x1e, 0x1e, 0x1f
};

void smb1_init(void) {
    apu_init();

    // init VRAM_AddrTable
    VRAM_AddrTable[0] = VRAM_Buffer1;
    VRAM_AddrTable[5] = VRAM_Buffer1_Offset;
    VRAM_AddrTable[6] = VRAM_Buffer2;
    VRAM_AddrTable[7] = VRAM_Buffer2;

    // init VRAM_Buffer_Offset
    VRAM_Buffer_Offset[0] = VRAM_Buffer1_Offset - memory;
    VRAM_Buffer_Offset[1] = VRAM_Buffer2_Offset - memory;

    Start();
}

void smb1_run(void) {
    NonMaskableInterrupt();
    ppu_draw(0xf0);
    apu_run();
}

void smb1_set_input_callback(smb1_input_callback_t callback) {
    cpu_input_callback = callback;
}
