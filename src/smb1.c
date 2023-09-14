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
uint8_t InitializeMemory(uint8_t y);
void SoundEngine(void);
void Dump_Squ1_Regs(uint8_t x, uint8_t y);
void PlaySqu1Sfx(uint8_t a, uint8_t x, uint8_t y);
uint8_t SetFreq_Squ1(uint8_t a);
uint8_t Dump_Freq_Regs(uint8_t a, uint8_t x);
uint8_t SetFreq_Squ2(uint8_t a);
const uint8_t SwimStompEnvelopeData[0xe];
void Square1SfxHandler(void);
void StopSquare1Sfx(void);
void Square2SfxHandler(void);
void StopSquare2Sfx(void);
void NoiseSfxHandler(void);
void MusicHandler(void);
void AlternateLengthHandler(uint8_t *a, uint8_t *x);
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
uint8_t *MusicData;
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
const uint8_t Sfx_SmallJump         = 0x80;
const uint8_t Sfx_Flagpole          = 0x40;
const uint8_t Sfx_Fireball          = 0x20;
const uint8_t Sfx_PipeDown_Injury   = 0x10;
const uint8_t Sfx_EnemySmack        = 0x08;
const uint8_t Sfx_EnemyStomp        = 0x04;
const uint8_t Sfx_Bump              = 0x02;
const uint8_t Sfx_BigJump           = 0x01;

const uint8_t Sfx_BowserFall        = 0x80;
const uint8_t Sfx_ExtraLife         = 0x40;
const uint8_t Sfx_PowerUpGrab       = 0x20;
const uint8_t Sfx_TimerTick         = 0x10;
const uint8_t Sfx_Blast             = 0x08;
const uint8_t Sfx_GrowVine          = 0x04;
const uint8_t Sfx_GrowPowerUp       = 0x02;
const uint8_t Sfx_CoinGrab          = 0x01;

const uint8_t Sfx_BowserFlame       = 0x02;
const uint8_t Sfx_BrickShatter      = 0x01;

// music constants
const uint8_t Silence               = 0x80;

const uint8_t StarPowerMusic        = 0x40;
const uint8_t PipeIntroMusic        = 0x20;
const uint8_t CloudMusic            = 0x10;
const uint8_t CastleMusic           = 0x08;
const uint8_t UndergroundMusic      = 0x04;
const uint8_t WaterMusic            = 0x02;
const uint8_t GroundMusic           = 0x01;

const uint8_t TimeRunningOutMusic   = 0x40;
const uint8_t EndOfLevelMusic       = 0x20;
const uint8_t AltGameOverMusic      = 0x10;
const uint8_t EndOfCastleMusic      = 0x08;
const uint8_t VictoryMusic          = 0x04;
const uint8_t GameOverMusic         = 0x02;
const uint8_t DeathMusic            = 0x01;

//enemy object constants
const uint8_t GreenKoopa            = 0x00;
const uint8_t BuzzyBeetle           = 0x02;
const uint8_t RedKoopa              = 0x03;
const uint8_t HammerBro             = 0x05;
const uint8_t Goomba                = 0x06;
const uint8_t Bloober               = 0x07;
const uint8_t BulletBill_FrenzyVar  = 0x08;
const uint8_t GreyCheepCheep        = 0x0a;
const uint8_t RedCheepCheep         = 0x0b;
const uint8_t Podoboo               = 0x0c;
const uint8_t PiranhaPlant          = 0x0d;
const uint8_t GreenParatroopaJump   = 0x0e;
const uint8_t RedParatroopa         = 0x0f;
const uint8_t GreenParatroopaFly    = 0x10;
const uint8_t Lakitu                = 0x11;
const uint8_t Spiny                 = 0x12;
const uint8_t FlyCheepCheepFrenzy   = 0x14;
const uint8_t FlyingCheepCheep      = 0x14;
const uint8_t BowserFlame           = 0x15;
const uint8_t Fireworks             = 0x16;
const uint8_t BBill_CCheep_Frenzy   = 0x17;
const uint8_t Stop_Frenzy           = 0x18;
const uint8_t Bowser                = 0x2d;
const uint8_t PowerUpObject         = 0x2e;
const uint8_t VineObject            = 0x2f;
const uint8_t FlagpoleFlagObject    = 0x30;
const uint8_t StarFlagObject        = 0x31;
const uint8_t JumpspringObject      = 0x32;
const uint8_t BulletBill_CannonVar  = 0x33;
const uint8_t RetainerObject        = 0x35;
const uint8_t TallEnemy             = 0x09;

//other constants
const uint8_t World1 = 0;
const uint8_t World2 = 1;
const uint8_t World3 = 2;
const uint8_t World4 = 3;
const uint8_t World5 = 4;
const uint8_t World6 = 5;
const uint8_t World7 = 6;
const uint8_t World8 = 7;

const uint8_t WarmBootOffset            = (uint8_t)0x07d6;
const uint8_t ColdBootOffset            = (uint8_t)0x07fe;
const uint16_t TitleScreenDataOffset    = 0x1ec0;
const uint16_t SoundMemory              = 0x07b0;
const uint8_t *MusicHeaderOffsetData    = MusicHeaderData - 1;
#define MHD MusicHeaderData

const uint8_t A_Button              = 0x80;
const uint8_t B_Button              = 0x40;
const uint8_t Select_Button         = 0x20;
const uint8_t Start_Button          = 0x10;
const uint8_t Up_Dir                = 0x08;
const uint8_t Down_Dir              = 0x04;
const uint8_t Left_Dir              = 0x02;
const uint8_t Right_Dir             = 0x01;

const uint8_t TitleScreenModeValue  = 0;
const uint8_t GameModeValue         = 1;
const uint8_t VictoryModeValue      = 2;
const uint8_t GameOverModeValue     = 3;

// $8000
void Start(void) {
    uint8_t a, x, y;
    bool c, z, v, n;

    // sei();                   // pretty standard 6502 type init here
    // cld();
    lda(0x10);                  // init PPU control register 1
    ppu_write_ppuctrl(a);
    ldx(0xff);                  // reset stack pointer
    sp = x; // txs();
// VBlank1:
    lda(ppu_read_ppustatus());  // wait two frames
    // bpl(VBlank1);
// VBlank2:
    lda(ppu_read_ppustatus());
    // bpl(VBlank2);
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
    // jmp(EndlessLoop);        // endless loop, need I say more?
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
    bool c, z, v, n;
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
    sta(memory[0x00]);                  // save here
    lda(PseudoRandomBitReg[1]);         // get second memory location
    and(0x02);                          // mask out all but d1
    eor(memory[0x00]);                  // perform exclusive-OR on d1 from first and second bytes
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
    // and(0x40);                       // not happen until vblank has ended
    // bne(Sprite0Clr);
    lda(*GamePauseStatus);              // if in pause mode, do not bother with sprites at all
    lsr(a);
    bcs(Sprite0Hit);
    MoveSpritesOffscreen();
    SpriteShuffler();
Sprite0Hit:
    lda(ppu_read_ppustatus());          // do sprite #0 hit detection
    ppu_draw(0x1f);
    // and(0x40);
    // beq(Sprite0Hit);
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
    // rti();                           // we are done until the next frame!
}

// $8182
void PauseRoutine(void) {
    pc = 0x8182;
    cpu_execute();
}

// $81c6
void SpriteShuffler(void) {
    pc = 0x81c6;
    cpu_execute();
}

// $8212
void OperModeExecutionTree(void) {
    pc = 0x8212;
    cpu_execute();
}

// $8220
void MoveAllSpritesOffscreen(void) {
    uint8_t a, x, y;
    bool c, z, v, n;

    ldy(0x00);                  // this routine moves all sprites off the screen
    bit(memory[0x04a0]);        // BIT instruction opcode
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

// $8223
void MoveSpritesOffscreen(void) {
    uint8_t a, x, y;
    bool c, z, v, n;

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
    uint8_t a, x, y;
    bool c, z, v, n;

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
void WriteNTAddr(uint8_t a) {
    uint8_t x, y;
    bool c, z, v, n;

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

// $8e5c
void ReadJoypads(void) {
    uint8_t a, x, y;
    bool c, z, v, n;

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
    bool c, z, v, n;
    uint8_t s[1];
    uint8_t *sp = s + sizeof(s) - 1;

    ldy(0x08);
PortLoop:
    pha();                      // push previous bit onto stack
    lda(cpu_read_joy[x]());     // read current bit on joypad port
    sta(memory[0x00]);          // check d1 and d0 of port output
    lsr(a);                     // this is necessary on the old
    ora(memory[0x00]);          // famicom systems in japan
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
    return; // rts();
Save8Bits:
    pla();
    sta(JoypadBitMask[x]);      // save with all bits in another place and leave
    // rts();
}

// $8edd
void UpdateScreen(const uint8_t *address) {
    uint8_t a, x, y;
    bool c, z, v, n;

    uint8_t s[1];
    uint8_t *sp = s + sizeof(s) - 1;

    goto UpdateScreen;

WriteBufferToScreen:
    ppu_write_ppuaddr(a);       // store high byte of vram address
    iny();
    lda(address[y]);            // load next byte (second)
    ppu_write_ppuaddr(a);       // store low byte of vram address
    iny();
    lda(address[y]);            // load next byte (third)
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
    lda(address[y]);            // load more data from buffer and write to vram
    ppu_write_ppudata(a);
    dex();                      // done writing?
    bne(OutputToVRAM);
    sec();
    tya();
    // adc $00                   ;add end length plus one to the indirect at $00
    // sta $00                   ;to allow this routine to read another set of updates
    // lda #$00
    // adc $01
    // sta $01
    address += a + (c ? 1 : 0);
    lda(0x3f);                  // sets vram address to $3f00
    ppu_write_ppuaddr(a);
    lda(0x00);
    ppu_write_ppuaddr(a);
    ppu_write_ppuaddr(a);       // then reinitializes it for some reason
    ppu_write_ppuaddr(a);
UpdateScreen:
    ldx(ppu_read_ppustatus());  // reset flip-flop
    ldy(0x00);                  // load first byte from indirect as a pointer
    lda(address[y]);
    bne(WriteBufferToScreen);   // if byte is zero we have no further updates to make here
    InitScroll(a);
}

// $8ee6
void InitScroll(uint8_t a) {
    ppu_write_ppuscroll(a); // store contents of A into scroll registers
    ppu_write_ppuscroll(a); // and end whatever subroutine led us here
}

void WritePPUReg1(uint8_t a) {
    ppu_write_ppuctrl(a);       // write contents of A to PPU register 1
    sta(*Mirror_PPU_CTRL_REG1); // and its mirror
}

// $8f97
void UpdateTopScore(void) {
    pc = 0x8f97;
    cpu_execute();
}

// $90cc
// a = 0
uint8_t InitializeMemory(uint8_t y) {
    uint8_t a, x;
    bool c, z, v, n;

    ldx(0x07);          // set initial high byte to $0700-$07ff
    lda(0x00);          // set initial low byte to start of page (at $00 of page)
    sta(memory[0x06]);
InitPageLoop:
    stx(memory[0x07]);
InitByteLoop:
    cpx(0x01);          // check to see if we're on the stack ($0100-$01ff)
    bne(InitByte);      // if not, go ahead anyway
    cpy(0x60);          // otherwise, check to see if we're at $0160-$01ff
    bcs(SkipByte);      // if so, skip write
InitByte:
    sta(memory[(memory[0x06]|(memory[0x07]<<8))+y]);    // otherwise, initialize byte with current low byte in Y
SkipByte:
    dey();
    cpy(0xff);          // do this until all bytes in page have been erased
    bne(InitByteLoop);
    dex();              // go onto the next page
    bpl(InitPageLoop);  // do this until all pages of memory have been erased
}

// $f2d0
void SoundEngine(void) {
    uint8_t a, y;

    // are we in title screen mode?
    if (!*OperMode) {
        // if so, disable sound and leave
        apu_write_snd_chn(0x00);
    } else {
        // disable irqs and set frame counter mode???
        apu_write_joy2(0xff);
        // enable first four channels
        apu_write_snd_chn(0x0f);
        // is sound already in pause mode?
        // if not, check pause sfx queue
        // if queue is empty, skip pause mode routine
        if (*PauseModeFlag || (*PauseSoundQueue == 0x01)) {
            // check pause sfx buffer
            if (!*PauseSoundBuffer) {
                // check pause queue
                a = *PauseSoundQueue;
                if (!a) {
                    goto SkipSoundSubroutines;
                }
                // if queue full, store in buffer and activvate
                // pause mode to interrupt game sounds
                *PauseSoundBuffer = a;
                *PauseModeFlag = a;
                // disable sound and clear sfx buffers
                apu_write_snd_chn(0x00);
                *Square1SoundBuffer = 0x00;
                *Square2SoundBuffer = 0x00;
                *NoiseSoundBuffer = 0x00;
                // enable sound again
                apu_write_snd_chn(0x0f);
                // store length of sound in pause counter
                *Squ1_SfxLenCounter = 0x2a;
PTone1F:
                // play first tone
                a = 0x44;
                // unconditional branch
            } else {
                // check pause length left
                a = *Squ1_SfxLenCounter;
                // time to play second?
                if (a != 0x24) {
                    // time to play first again?
                    if (a == 0x1e) {
                        goto PTone1F;
                    }
                    // time to play second again?
                    if (a != 0x18) {
                        // only load regs during times, otherwise skip
                        goto DecPauC;
                    }
                }
                // store reg contents and play the pause sfx
                a = 0x64;
            }
            PlaySqu1Sfx(a, 0x84, 0x7f);
DecPauC:
            // decrement pause sfx counter
            if (!--*Squ1_SfxLenCounter) {
                // disable sound if in pause mode and
                // not currently playing the pause sfx
                apu_write_snd_chn(0x00);
                // if no longer playing pause sfx, check to see
                // if we need to be playing sound again
                if (*PauseSoundBuffer == 0x02) {
                    // clear pause mode to allow game sounds again
                    *PauseModeFlag = 0x00;
                }
                // clear pause sfx buffer
                *PauseSoundBuffer = 0x00;
            }
        } else {
            // play sfx on square channel 1
            Square1SfxHandler();
            //  ''  ''  '' square channel 2
            Square2SfxHandler();
            //  ''  ''  '' noise channel
            NoiseSfxHandler();
            // play music on all channels
            MusicHandler();
            // clear the music queues
            *AreaMusicQueue = 0x00;
            *EventMusicQueue = 0x00;
        }
SkipSoundSubroutines:
        // clear the sound effects queues
        *Square1SoundQueue = 0x00;
        *Square2SoundQueue = 0x00;
        *NoiseSoundQueue = 0x00;
        *PauseSoundQueue = 0x00;
        // load some sort of counter
        y = *DAC_Counter;
        // check for specific music
        // increment and check counter
        // if not there yet, just store it
        // if we are at zero, do not decrement
        if ((!(*AreaMusicBuffer & 0x03) || (++*DAC_Counter, y >= 0x30)) && y) {
            // decrement counter
            --*DAC_Counter;
        }
        // store into DMC load register (??)
        apu_write_dmc_raw(y);
        // we are done here
    }
}

void Dump_Squ1_Regs(uint8_t x, uint8_t y) {
    // dump the contents of X and Y into square 1's control regs
    apu_write_sq1_sweep(y);
    apu_write_sq1_vol(x);
}

void PlaySqu1Sfx(uint8_t a, uint8_t x, uint8_t y) {
    // do sub to set ctrl regs for square 1, then set frequency regs
    Dump_Squ1_Regs(x, y);
    SetFreq_Squ1(a);
}

uint8_t SetFreq_Squ1(uint8_t a) {
    // set frequency reg offset for square 1 sound channel
    return Dump_Freq_Regs(a, 0x00);
}

uint8_t Dump_Freq_Regs(uint8_t a, uint8_t x) {
    uint8_t y;
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

    y = a;
    // use previous contents of A for sound reg offset
    a = FreqRegLookupTbl[y+1];
    // if zero, then do not load
    if (a) {
        // first byte goes into LSB of frequency divider
        apu_write[x+2](a);
        // second byte goes into 3 MSB plus extra bit for
        // length counter
        a = FreqRegLookupTbl[y] | 0x08;
        apu_write[x+3](a);
    }
    return a;
}

void Dump_Sq2_Regs(uint8_t x, uint8_t y) {
    // dump the contents of X and Y into square 2's control regs
    apu_write_sq2_vol(x);
    apu_write_sq2_sweep(y);
}

void PlaySqu2Sfx(uint8_t a, uint8_t x, uint8_t y) {
    // do sub to set ctrl regs for square 2 sound channel
    Dump_Sq2_Regs(x, y);
    SetFreq_Squ2(a);
}

uint8_t SetFreq_Squ2(uint8_t a) {
    // set frequency reg offset for square 2 sound channel
    // unconditional branch
    return Dump_Freq_Regs(a, 0x04);
}

uint8_t SetFreq_Tri(uint8_t a) {
    // set frequency reg offset for triangle sound channel
    // unconditional branch
    return Dump_Freq_Regs(a, 0x08);
}

const uint8_t SwimStompEnvelopeData[0xe] = {
    0x9f, 0x9b, 0x98, 0x96, 0x95, 0x94, 0x92, 0x90,
    0x90, 0x9a, 0x97, 0x95, 0x93, 0x92
};

// $f41b
void Square1SfxHandler(void) {
    uint8_t a, x, y;
    bool c;

    // check for sfx in queue
    y = *Square1SoundQueue;
    if (y) {
        // if found, put in buffer
        *Square1SoundBuffer = y;
        if (y & 0x80) {
            // small jump
            goto PlaySmallJump;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x01) {
            // big jump
            goto PlayBigJump;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x02) {
            // bump
            goto PlayBump;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x04) {
            // swim/stomp
            goto PlaySwimStomp;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x08) {
            // smack enemy
            goto PlaySmackEnemy;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x10) {
            // pipedown/injury
            goto PlayPipeDownInj;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x20) {
            // fireball throw
            goto PlayFireballThrow;
        }
        *Square1SoundQueue >>= 1;
        if (y & 0x40) {
            // slide flagpole
            goto PlayFlagpoleSlide;
        }
    }
    // check for sfx in buffer
    a = *Square1SoundBuffer;
    // if not found, exit sub
    if (a) {
        if (a & 0x80) {
            // small mario jump
            goto ContinueSndJump;
        }
        if (a & 0x01) {
            // big mario jump
            goto ContinueSndJump;
        }
        if (a & 0x02) {
            // bump
            goto ContinueBumpThrow;
        }
        if (a & 0x04) {
            // swim/stomp
            goto ContinueSwimStomp;
        }
        if (a & 0x08) {
            // smack enemy
            goto ContinueSmackEnemy;
        }
        if (a & 0x10) {
            // pipedown/injury
            goto ContinuePipeDownInj;
        }
        if (a & 0x20) {
            // fireball throw
            goto ContinueBumpThrow;
        }
        if (a & 0x40) {
            // slide flagpole
            goto DecrementSfx1Length;
        }
    }
    return;

PlaySwimStomp:
    // store length of swim/stomp sound
    *Squ1_SfxLenCounter = 0x0e;
    // store reg contents for swim/stomp sound
    PlaySqu1Sfx(0x26, 0x9e, 0x9c);

ContinueSwimStomp:
    // look up reg contents in data section based on
    // length of sound left, used to control sound's
    // envelope
    y = *Squ1_SfxLenCounter;
    apu_write_sq1_vol(SwimStompEnvelopeData[y-1]);
    if (y == 0x06) {
        // when the length counts down to a certain point, put this
        // directly into the LSB of square 1's frequency divider
        apu_write_sq1_lo(0x9e);
    }
    // unconditional branch (regardless of how we got here)
    goto DecrementSfx1Length;

PlaySmackEnemy:
    // store length of smack enemy sound
    *Squ1_SfxLenCounter = 0x0e;
    // store reg contents for smack enemy sound
    PlaySqu1Sfx(0x28, 0x9f, 0xcb);
    // unconditional branch
    goto DecrementSfx1Length;

ContinueSmackEnemy:
    // check about halfway through
    if (*Squ1_SfxLenCounter == 0x08) {
        // if we're at the about-halfway point, make the second tone
        // in the smack enemy sound
        apu_write_sq1_lo(0xa0);
        a = 0x9f;
    } else {
        // this creates space in the sound, giving it its distinct noise
        a = 0x90;
    }
    apu_write_sq1_vol(a);

DecrementSfx1Length:
    // decrement length of sfx
    if (!--*Squ1_SfxLenCounter) {
        StopSquare1Sfx();
    }
    return;

PlayPipeDownInj:
    // load length of pipedown sound
    *Squ1_SfxLenCounter = 0x2f;

ContinuePipeDownInj:
    // some bitwise logic, forces the regs
    // to be written to only during six specific times
    // during which d3 must be set and d1-0 must be clear
    a = *Squ1_SfxLenCounter;
    if (!(a & 0x01) && !(a & 0x02) && (a & 0x08)) {
        // and this is where it actually gets written in
        PlaySqu1Sfx(0x44, 0x9a, 0x91);
    }
    goto DecrementSfx1Length;

PlayFlagpoleSlide:
    // store length of flagpole sound
    *Squ1_SfxLenCounter = 0x40;
    // load part of reg contents for flagpole sound
    SetFreq_Squ1(0x62);
    // now load the rest
    x = 0x99;
    goto FPS2nd;

PlaySmallJump:
    // branch here for small mario jumping sound
    a = 0x26;
    goto JumpRegContents;

PlayBigJump:
    // branch here for big mario jumping sound
    a = 0x18;

JumpRegContents:
    // note that small and big jump borrow each others' reg contents
    // anyway, this loads the first part of mario's jumping sound
    PlaySqu1Sfx(a, 0x82, 0xa7);
    // store length of sfx for both jumping sounds
    // then continue on here
    *Squ1_SfxLenCounter = 0x28;

ContinueSndJump:
    // jumping sounds seem to be composed of three parts
    a = *Squ1_SfxLenCounter;
    // check for time to play second yet
    if (a == 0x25) {
        // load second part
        x = 0x5f;
        y = 0xf6;
        // unconditional branch
    // check for third part
    } else {
        if (a != 0x20) {
            goto DecrementSfx1Length;
        }
        x = 0x48;
FPS2nd:
        y = 0xbc;
    }
    Dump_Squ1_Regs(x, y);
    // unconditional branch outta here
    goto DecrementSfx1Length;

PlayFireballThrow:
    a = 0x05;
    // load reg contents for fireball throw sound
    y = 0x99;
    // unconditional branch
    goto Fthrow;

PlayBump:
    // load length of sfx and reg contents for bump sound
    a = 0x0a;
    y = 0x93;
Fthrow:
    // the fireball sound shares reg contents with the bump sound
    *Squ1_SfxLenCounter = a;
    // load offset for bump sound
    PlaySqu1Sfx(0x0c, 0x9e, y);

ContinueBumpThrow:
    // check for second part of bump sound
    a = *Squ1_SfxLenCounter;
    if (a == 0x06) {
        // load second part directly
        apu_write_sq1_sweep(0xbb);
    }
    // unconditional branch
    goto DecrementSfx1Length;
}

void StopSquare1Sfx(void) {
    // if end of sfx reached, clear buffer
    // and stop making the sfx
    *Square1SoundBuffer = 0x00;
    apu_write_snd_chn(0x0e);
    apu_write_snd_chn(0x0f);
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
    bool c;

    // special handling for the 1-up sound to keep it
    // from being interrupted by other sounds on square 2
    if (*Square2SoundBuffer & Sfx_ExtraLife) {
        goto ContinueExtraLife;
    }
    // check for sfx in queue
    y = *Square2SoundQueue;
    if (y) {
        // if found, put in buffer and check for the following
        *Square2SoundBuffer = y;
        if (y & 0x80) {
            // bowser fall
            goto PlayBowserFall;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x01) {
            // coin grab
            goto PlayCoinGrab;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x02) {
            // power-up reveal
            goto PlayGrowPowerUp;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x04) {
            // vine grow
            goto PlayGrowVine;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x08) {
            // fireworks/gunfire
            goto PlayBlast;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x10) {
            // timer tick
            goto PlayTimerTick;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x20) {
            // power-up grab
            goto PlayPowerUpGrab;
        }
        *Square2SoundQueue >>= 1;
        if (y & 0x40) {
            // 1-up
            goto PlayExtraLife;
        }
    }
    // check for sfx in buffer
    a = *Square2SoundBuffer;
    // if not found, exit sub
    if (a) {
        if (a & 0x80) {
            // bowser fall
            goto ContinueBowserFall;
        }
        if (a & 0x01) {
            // coin grab
            goto ContinueCGrabTTick;
        }
        if (a & 0x02) {
            // power-up reveal
            goto ContinueGrowItems;
        }
        if (a & 0x04) {
            // vine grow
            goto ContinueGrowItems;
        }
        if (a & 0x08) {
            // fireworks/gunfire
            goto ContinueBlast;
        }
        if (a & 0x10) {
            // timer tick
            goto ContinueCGrabTTick;
        }
        if (a & 0x20) {
            // power-up grab
            goto ContinuePowerUpGrab;
        }
        if (a & 0x40) {
            // 1-up
            goto ContinueExtraLife;
        }
    }
    return;

PlayBowserFall:
    // load length of bowser defeat sound
    *Squ2_SfxLenCounter = 0x38;
    // load contents of reg for bowser defeat sound
    y = 0xc4;
    a = 0x18;
    goto PBFRegs;

ContinueBowserFall:
    // check for almost near the end
    if (*Squ2_SfxLenCounter != 0x08) {
        goto DecrementSfx2Length;
    }
    // if so, load the rest of reg contents for bowser defeat sound
    y = 0xa4;
    a = 0x5a;
PBFRegs:
    // the fireworks/gunfire sound shares part of reg contents here
    x = 0x9f;
    // this is an unconditional branch outta here
    goto LoadSqu2Regs;

PlayExtraLife:
    // load length of 1-up sound
    *Squ2_SfxLenCounter = 0x30;

ContinueExtraLife:
    a = *Squ2_SfxLenCounter;
    // load new tones only every eight frames
    // if any bits are set here, branch to dec the length
    // do this until all bits checked, if none set, continue
    if (a & 0x07) {
        goto DecrementSfx2Length;
    }
    // load our reg contents
    a = ExtraLifeFreqData[(a>>3)-1];
    x = 0x82;
    y = 0x7f;
    // unconditional branch
    goto LoadSqu2Regs;

PlayGrowPowerUp:
    // load length of power-up reveal sound
    a = 0x10;
    goto GrowItemRegs;

PlayGrowVine:
    // load length of vine grow sound
    a = 0x20;

GrowItemRegs:
    *Squ2_SfxLenCounter = a;
    // load contents of reg for both sounds directly
    apu_write_sq2_sweep(0x7f);
    // start secondary counter for both sounds
    *Sfx_SecondaryCounter = 0x00;

ContinueGrowItems:
    // increment secondary counter for both sounds
    ++*Sfx_SecondaryCounter;
    // this sound doesn't decrement the usual counter
    // divide by 2 to get the offset
    a = *Sfx_SecondaryCounter >> 1;
    y = a;
    // have we reached the end yet?
    if (y == *Squ2_SfxLenCounter) {
        // if so, branch to jump, and stop playing sounds
        goto EmptySfx2Buffer;
    }
    // load contents of other reg directly
    apu_write_sq2_vol(0x9d);
    // use secondary counter / 2 as offset for frequency regs
    SetFreq_Squ2(PUp_VGrow_FreqData[y]);
    return;

PlayCoinGrab:
    // load length of coin grab sound
    // and part of reg contents
    a = 0x35;
    x = 0x8d;
    goto CGrab_TTickRegL;

PlayTimerTick:
    // load length of timer tick sound
    // and part of reg contents
    a = 0x06;
    x = 0x98;

CGrab_TTickRegL:
    *Squ2_SfxLenCounter = a;
    // load the rest of reg contents
    // of coin grab and timer tick sound
    PlaySqu2Sfx(0x42, x, 0x7f);
    
ContinueCGrabTTick:
    // check for time to play second tone yet
    // timer tick sound also executes this, not sure why
    if (*Squ2_SfxLenCounter == 0x30) {
        // if so, load the tone directly into the reg
        apu_write_sq2_lo(0x54);   
    }
    goto DecrementSfx2Length;

PlayBlast:
    // load length of fireworks/gunfire sound
    *Squ2_SfxLenCounter = 0x20;
    // load reg contents of fireworks/gunfire sound
    y = 0x94;
    a = 0x5e;
    goto PBFRegs;

ContinueBlast:
    // check for time to play second part
    if (*Squ2_SfxLenCounter != 0x18) {
        goto DecrementSfx2Length;
    }
    // load second part reg contents then
    y = 0x93;
    a = 0x18;
    // unconditional branch to load rest of reg contents
    goto PBFRegs;

PlayPowerUpGrab:
    // load length of power-up grab sound
    *Squ2_SfxLenCounter = 0x36;

ContinuePowerUpGrab:
    // load frequency reg based on length left over
    a = *Squ2_SfxLenCounter;
    // divide by 2
    if (a & 0x01) {
        // alter frequency every other frame
        goto DecrementSfx2Length;
    }
    // use length left over / 2 for frequency offset
    a = PowerUpGrabFreqData[(a>>1)-1];
    // store reg contents of power-up grab sound
    x = 0x5d;
    y = 0x7f;

LoadSqu2Regs:
    PlaySqu2Sfx(a, x, y);

DecrementSfx2Length:
    // decrement length of sfc
    if (!--*Squ2_SfxLenCounter) {
EmptySfx2Buffer:
        // initialize square 2's sound effects buffer
        *Square2SoundBuffer = 0x00;
        StopSquare2Sfx();
    }
}

void StopSquare2Sfx(void) {
    // stop playing the sfx
    apu_write_snd_chn(0x0d);
    apu_write_snd_chn(0x0f);
}

const uint8_t BrickShatterFreqData[0x10] = {
    0x01, 0x0e, 0x0e, 0x0d, 0x0b, 0x06, 0x0c, 0x0f,
    0x0a, 0x09, 0x03, 0x0d, 0x08, 0x0d, 0x06, 0x0c
};

// $f667
void NoiseSfxHandler(void) {
    uint8_t a, y, x;
    bool c;

    // check for sfx in queue
    y = *NoiseSoundQueue;
    if (*NoiseSoundQueue) {
        // if found, put in buffer
        *NoiseSoundBuffer = y;
        *NoiseSoundQueue >>= 1;
        if (y & 0x01) {
            // brick shatter
            goto PlayBrickShatter;
        }
        *NoiseSoundQueue >>= 1;
        if (y & 0x02) {
            // bowser flame
            goto PlayBowserFlame;
        }
    }
    // check for sfx in buffer
    a = *NoiseSoundBuffer;
    // if not found, exit sub
    if (a) {
        if (a & 0x01) {
            // brick shatter
            goto ContinueBrickShatter;
        }
        if (a & 0x02) {
            // bowser flame
            goto ContinueBowserFlame;
        }
    }
    return;

PlayBowserFlame:
    // load length of bowser flame sound
    *Noise_SfxLenCounter = 0x40;

ContinueBowserFlame:
    // load reg contents of bowser flame sound
    x = 0x0f;
    a = BowserFlameEnvData[(*Noise_SfxLenCounter>>1)-1];
    // unconditional branch here
    goto PlayNoiseSfx;

PlayBrickShatter:
    // load length of brick shatter sound
    *Noise_SfxLenCounter = 0x20;

ContinueBrickShatter:
    a = *Noise_SfxLenCounter;
    // divide by 2 and check for bit set to use offset
    if (!(a & 0x01)) {
        goto DecrementSfx3Length;
    }
    a >>= 1;
    // load reg contents of brick shatter sound
    x = BrickShatterFreqData[a];
    a = BrickShatterEnvData[a];

PlayNoiseSfx:
    // play the sfx
    apu_write_noise_vol(a);
    apu_write_noise_lo(x);
    apu_write_noise_hi(0x18);

DecrementSfx3Length:
    // decrement the length of sfx
    if (!--*Noise_SfxLenCounter) {
        // if done, stop playing the sfx
        apu_write_noise_vol(0xf0);
        *NoiseSoundBuffer = 0x00;
    }
}

// $f694
void MusicHandler(void) {
    uint8_t a, x, y;
    bool c;

    a = *EventMusicQueue;
    if (a) {
        goto LoadEventMusic;
    }
    a = *AreaMusicQueue;
    if (a) {
        goto LoadAreaMusic;
    }
    a = *EventMusicBuffer;
    a |= *AreaMusicBuffer;
    if (a) {
        goto HandleSquare2Music;
    }
    return;

LoadEventMusic:
    *EventMusicBuffer = a;
    if (a != DeathMusic) {
        goto NoStopSfx;
    }
    StopSquare1Sfx();
    StopSquare2Sfx();
NoStopSfx:
    x = *AreaMusicBuffer;
    *AreaMusicBuffer_Alt = x;
    y = 0x00;
    *NoteLengthTblAdder = y;
    *AreaMusicBuffer = y;
    if (a != TimeRunningOutMusic) {
        goto FindEventMusicHeader;
    }
    x = 0x08;
    *NoteLengthTblAdder = x;
    goto FindEventMusicHeader;

LoadAreaMusic:
    if (a != 0x04) {
        goto NoStop1;
    }
    StopSquare1Sfx();
NoStop1:
    y = 0x10;
GMLoopB:
    *GroundMusicHeaderOfs = y;

HandleAreaMusicLoopB:
    y = 0x00;
    *EventMusicBuffer = y;
    *AreaMusicBuffer = a;
    if (a != 0x01) {
        goto FindAreaMusicHeader;
    }
    ++*GroundMusicHeaderOfs;
    y = *GroundMusicHeaderOfs;
    if (y != 0x32) {
        goto LoadHeader;
    }
    y = 0x11;
    goto GMLoopB;

FindAreaMusicHeader:
    y = 0x08;
    *MusicOffset_Square2 = y;

FindEventMusicHeader:
    ++y;
    c = (a & 0x01) ? true : false;
    a >>= 1;
    if (!c) {
        goto FindEventMusicHeader;
    }

LoadHeader:
    a = MusicHeaderOffsetData[y];
    y = a;
    a = MusicHeaderData[y];
    *NoteLenLookupTblOfs = a;
    a = MusicHeaderData[y+1];
    *MusicDataLow = a;
    a = MusicHeaderData[y+2];
    *MusicDataHigh = a;
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
    a = MusicHeaderData[y+3];
    *MusicOffset_Triangle = a;
    a = MusicHeaderData[y+4];
    *MusicOffset_Square1 = a;
    a = MusicHeaderData[y+5];
    *MusicOffset_Noise = a;
    *NoiseDataLoopbackOfs = a;
    a = 0x01;
    *Squ2_NoteLenCounter = a;
    *Squ1_NoteLenCounter = a;
    *Tri_NoteLenCounter = a;
    *Noise_BeatLenCounter = a;
    a = 0x00;
    *MusicOffset_Square2 = a;
    *AltRegContentFlag = a;
    a = 0x0b;
    apu_write_snd_chn(a);
    a = 0x0f;
    apu_write_snd_chn(a);

HandleSquare2Music:
    if (--*Squ2_NoteLenCounter) {
        goto MiscSqu2MusicTasks;
    }
    y = *MusicOffset_Square2;
    ++*MusicOffset_Square2;
    a = MusicData[y];
    if (!a) {
        goto EndOfMusicData;
    }
    if ((int8_t)a >= 0) {
        goto Squ2NoteHandler;
    }
    goto Squ2LengthHandler;

EndOfMusicData:
    a = *EventMusicBuffer;
    if (a != TimeRunningOutMusic) {
        goto NotTRO;
    }
    a = *AreaMusicBuffer_Alt;
    if (a) {
        goto MusicLoopBack;
    }
NotTRO:
    a &= VictoryMusic;
    if (a) {
        goto VictoryMLoopBack;
    }
    a = *AreaMusicBuffer;
    a &= 0x5f;
    if (a) {
        goto MusicLoopBack;
    }
    a = 0x00;
    *AreaMusicBuffer = a;
    *EventMusicBuffer = a;
    apu_write_tri_linear(a);
    a = 0x90;
    apu_write_sq1_vol(a);
    apu_write_sq2_vol(a);
    return;

MusicLoopBack:
    goto HandleAreaMusicLoopB;

VictoryMLoopBack:
    goto LoadEventMusic;

Squ2LengthHandler:
    a = ProcessLengthData(a);
    *Squ2_NoteLenBuffer = a;
    y = *MusicOffset_Square2;
    ++*MusicOffset_Square2;
    a = MusicData[y];

Squ2NoteHandler:
    x = *Square2SoundBuffer;
    if (x) {
        goto SkipFqL1;
    }
    a = SetFreq_Squ2(a);
    x = 0x04;
    y = a;
    if (!a) {
        goto Rest;
    }
    LoadControlRegs(&a, &x, &y);
Rest:
    *Squ2_EnvelopeDataCtrl = a;
    Dump_Sq2_Regs(x, y);
SkipFqL1:
    a = *Squ2_NoteLenBuffer;
    *Squ2_NoteLenCounter = a;

MiscSqu2MusicTasks:
    a = *Square2SoundBuffer;
    if (a) {
        goto HandleSquare1Music;
    }
    a = *EventMusicBuffer;
    a &= 0x91;
    if (a) {
        goto HandleSquare1Music;
    }
    y = *Squ2_EnvelopeDataCtrl;
    if (!y) {
        goto NoDecEnv1;
    }
    --*Squ2_EnvelopeDataCtrl;
NoDecEnv1:
    a = LoadEnvelopeData(y);
    apu_write_sq2_vol(a);
    x = 0x7f;
    apu_write_sq2_sweep(x);

HandleSquare1Music:
    y = *MusicOffset_Square1;
    if (!y) {
        goto HandleTriangleMusic;
    }
    if (--*Squ1_NoteLenCounter) {
        goto MiscSqu1MusicTasks;
    }

FetchSqu1MusicData:
    y = *MusicOffset_Square1;
    ++*MusicOffset_Square1;
    a = MusicData[y];
    if (a) {
        goto Squ1NoteHandler;
    }
    a = 0x83;
    apu_write_sq1_vol(a);
    a = 0x94;
    apu_write_sq1_sweep(a);
    *AltRegContentFlag = a;
    goto FetchSqu1MusicData;

Squ1NoteHandler:
    AlternateLengthHandler(&a, &x);
    *Squ1_NoteLenCounter = a;
    y = *Square1SoundBuffer;
    if (y) {
        goto HandleTriangleMusic;
    }
    a = x;
    a &= 0x3e;
    a = SetFreq_Squ1(a);
    x = 0x00;
    y = a;
    if (!a) {
        goto SkipCtrlL;
    }
    LoadControlRegs(&a, &x, &y);
SkipCtrlL:
    *Squ1_EnvelopeDataCtrl = a;
    Dump_Squ1_Regs(x, y);

MiscSqu1MusicTasks:
    a = *Square1SoundBuffer;
    if (a) {
        goto HandleTriangleMusic;
    }
    a = *EventMusicBuffer;
    a &= 0x91;
    if (a) {
        goto DeathMAltReg;
    }
    y = *Squ1_EnvelopeDataCtrl;
    if (!y) {
        goto NoDecEnv2;
    }
    --*Squ1_EnvelopeDataCtrl;
NoDecEnv2:
    a = LoadEnvelopeData(y);
    apu_write_sq1_vol(a);
DeathMAltReg:
    a = *AltRegContentFlag;
    if (a) {
        goto DoAltLoad;
    }
    a = 0x7f;
DoAltLoad:
    apu_write_sq1_sweep(a);

HandleTriangleMusic:
    a = *MusicOffset_Triangle;
    if (--*Tri_NoteLenCounter) {
        goto HandleNoiseMusic;
    }
    y = *MusicOffset_Triangle;
    ++*MusicOffset_Triangle;
    a = MusicData[y];
    if (!a) {
        goto LoadTriCtrlReg;
    }
    if ((int8_t)a >= 0) {
        goto TriNoteHandler;
    }
    a = ProcessLengthData(a);
    *Tri_NoteLenBuffer = a;
    a = 0x1f;
    apu_write_tri_linear(a);
    y = *MusicOffset_Triangle;
    ++*MusicOffset_Triangle;
    a = MusicData[y];
    if (!a) {
        goto LoadTriCtrlReg;
    }

TriNoteHandler:
    SetFreq_Tri(a);
    x = *Tri_NoteLenBuffer;
    *Tri_NoteLenCounter = x;
    a = *EventMusicBuffer;
    a &= 0x6e;
    if (a) {
        goto NotDOrD4;
    }
    a = *AreaMusicBuffer;
    a &= 0x0a;
    if (!a) {
        goto HandleNoiseMusic;
    }
NotDOrD4:
    a = x;
    if (a >= 0x12) {
        goto LongN;
    }
    a = *EventMusicBuffer;
    a &= EndOfCastleMusic;
    if (!a) {
        goto MediN;
    }
    a = 0x0f;
    goto LoadTriCtrlReg;
MediN:
    a = 0x1f;
    goto LoadTriCtrlReg;
LongN:
    a = 0xff;

LoadTriCtrlReg:
    apu_write_tri_linear(a);

HandleNoiseMusic:
    a = *AreaMusicBuffer;
    a &= 0xf3;
    if (!a) {
        goto ExitMusicHandler;
    }
    if (--*Noise_BeatLenCounter) {
        goto ExitMusicHandler;
    }

FetchNoiseBeatData:
    y = *MusicOffset_Noise;
    ++*MusicOffset_Noise;
    a = MusicData[y];
    if (a) {
        goto NoiseBeatHandler;
    }
    a = *NoiseDataLoopbackOfs;
    *MusicOffset_Noise = a;
    if (a) {
        goto FetchNoiseBeatData;
    }

NoiseBeatHandler:
    AlternateLengthHandler(&a, &x);
    *Noise_BeatLenCounter = a;
    a = x;
    a &= 0x3e;
    if (!a) {
        goto SilentBeat;
    }
    if (a == 0x30) {
        goto LongBeat;
    }
    if (a == 0x20) {
        goto StrongBeat;
    }
    a &= 0x10;
    if (!a) {
        goto SilentBeat;
    }
    a = 0x1c;
    x = 0x03;
    y = 0x18;
    goto PlayBeat;

StrongBeat:
    a = 0x1c;
    x = 0x0c;
    y = 0x18;
    goto PlayBeat;

LongBeat:
    a = 0x1c;
    x = 0x03;
    y = 0x58;
    goto PlayBeat;

SilentBeat:
    a = 0x10;

PlayBeat:
    apu_write_noise_vol(a);
    apu_write_noise_lo(x);
    apu_write_noise_hi(y);

ExitMusicHandler:
    return;

    // pc = 0xf694;
    // cpu_execute();
}

void AlternateLengthHandler(uint8_t *a, uint8_t *x) {
    bool c, c1;

    // save a copy of original byte into X
    // save LSB from original byte into carry
    // reload original byte and rotate three times
    // turning xx00000x into 00000xxx, with the
    // bit in carry as the MSB here
    *x = *a;
    c = (*a & 0x80) ? true : false;
    *a = (*a << 1) | (*a & 0x01);
    c1 = (*a & 0x80) ? true : false;
    *a = (*a << 1) | (c ? 0x01 : 0x00);
    *a = (*a << 1) | (c1 ? 0x01 : 0x00);

    *a = ProcessLengthData(*a);
}

uint8_t ProcessLengthData(uint8_t a) {
    uint16_t a16;

    // clear all but the three LSBs
    a &= 0x07;
    // add offset loaded from first header byte
    a16 = a + *NoteLenLookupTblOfs;
    // add extra if time running out music
    a = (a16&0xff) + (a16>>8) + *NoteLengthTblAdder;
    // load length
    a = MusicLengthLookupTbl[a];
    return a;
}

void LoadControlRegs(uint8_t *a, uint8_t *x, uint8_t *y) {
    // check secondary buffer for win castle music
    if (*EventMusicBuffer & EndOfCastleMusic) {
        // this value is only used for win castle music
        *a = 0x04;
        // unconditional branch
    // check primary buffer for water music
    } else if (*AreaMusicBuffer & 0x7d) {
        // this is the default value for all other music
        *a = 0x08;
    } else {
        // this value is used for water music and all other event music
        *a = 0x28;
    }
    // load contents of other sound regs for square 2
    *x = 0x82;
    *y = 0x7f;
}

uint8_t LoadEnvelopeData(uint8_t y) {
    // check secondary buffer for win castle music
    if (*EventMusicBuffer & EndOfCastleMusic) {
        // load data from offset for win castle music
        return EndOfCastleMusicEnvData[y];
    }

    // check primary buffer for water music
    if (*AreaMusicBuffer & 0x7d) {
        // load default data from offset for all other music
        return AreaMusicEnvData[y];
    }

    // load data from offset for water music and all other event music
    return WaterEventMusEnvData[y];
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
