  .inesprg 1   ; 1x 16KB PRG code
  .ineschr 1   ; 1x  8KB CHR data
  .inesmap 0   ; mapper 0 = NROM, no bank swapping
  .inesmir 1   ; background mirroring
  

;;;;;;;;;;;;;;;

;; DECLARE SOME VARIABLES HERE
  .rsset $0000  ;;start variables at ram location 0
  
gamestate  .rs 1  ; .rs 1 means reserve one byte of space
ballx      .rs 1  ; ball horizontal position
bally      .rs 1  ; ball vertical position
ballup     .rs 1  ; 1 = ball moving up
balldown   .rs 1  ; 1 = ball moving down
ballleft   .rs 1  ; 1 = ball moving left
ballright  .rs 1  ; 1 = ball moving right
ballspeedx .rs 1  ; ball horizontal speed per frame
ballspeedy .rs 1  ; ball vertical speed per frame
paddle1ytop   .rs 1  ; player 1 paddle top vertical position
paddle2ytop   .rs 1  ; player 2 paddle top vertical position
paddle2ybot   .rs 1  ; player 2 paddle bottom vertical position
buttons1   .rs 1  ; player 1 gamepad buttons, one bit per button
buttons2   .rs 1  ; player 2 gamepad buttons, one bit per button
score1     .rs 1  ; player 1 score, 0-15
score2     .rs 1  ; player 2 score, 0-15
decimalScore1 .rs 3
decimalScore2 .rs 3
lives1     .rs 1  ; player 1 lives
mvpTileYMemLoc .rs 2 ; MovePaddleSprites param - mem location of y position of tile
mvpY .rs 1; MovePaddleSprites param - y position
paddle1ybot .rs 1
bckgrndtlmemloc .rs 2
decimalResult .rs 3
tempBinary .rs 1
buttonStateCache .rs 1
;collision detection params
rect1leftx .rs 1
rect1rightx .rs 1
rect1topy .rs 1
rect1bottomy .rs 1
rect2leftx .rs 1
rect2rightx .rs 1
rect2topy .rs 1
rect2bottomy .rs 1
ballresetdelay .rs 1

;collision detection return
inrect .rs 1

;; DECLARE SOME CONSTANTS HERE
STATETITLE     = $00  ; displaying title screen
STATEPLAYING   = $01  ; move paddles/ball, check for collisions
STATEGAMEOVER  = $02  ; displaying game over screen
  
RIGHTWALL      = $FF  ; when ball reaches one of these, do something
TOPWALL        = $1F
BOTTOMWALL     = $E0
LEFTWALL       = $04
BALLWIDTH      = $08
BALLLENGTH     = $08
PADDLEYSTART   = $64
  
PADDLE1X       = $08  ; horizontal position for paddles, doesnt move. TODO using this as paddle1 surface x pos. Change so this is not a constant.
PADDLE2X       = $F8
PADDLELENGTH   = $04
WINSCORE       = $0A
;TOPPADDLETILEYPOSMEMLOCHIGH = $02 ;high byte of the mem location of y pos of top paddle tile
;TOPPADDLETILEYPOSMEMLOCLOW = $04  ;low byte of the mem location of y pos of top paddle tile   
;;;;;;;;;;;;;;;;;;




  .bank 0
  .org $C000 
RESET:
  SEI          ; disable IRQs
  CLD          ; disable decimal mode
  LDX #$40
  STX $4017    ; disable APU frame IRQ
  LDX #$FF
  TXS          ; Set up stack
  INX          ; now X = 0
  STX $2000    ; disable NMI
  STX $2001    ; disable rendering
  STX $4010    ; disable DMC IRQs

vblankwait1:       ; First wait for vblank to make sure PPU is ready
  BIT $2002
  BPL vblankwait1

clrmem:
  LDA #$00
  STA $0000, x
  STA $0100, x
  STA $0300, x
  STA $0400, x
  STA $0500, x
  STA $0600, x
  STA $0700, x
  LDA #$FE
  STA $0200, x
  INX
  BNE clrmem
   
vblankwait2:      ; Second wait for vblank, PPU is ready after this
  BIT $2002
  BPL vblankwait2


LoadPalettes:
  LDA $2002             ; read PPU status to reset the high/low latch
  LDA #$3F
  STA $2006             ; write the high byte of $3F00 address
  LDA #$00
  STA $2006             ; write the low byte of $3F00 address
  LDX #$00              ; start out at 0
LoadPalettesLoop:
  LDA palette, x        ; load data from address (palette + the value in x)
                          ; 1st time through loop it will load palette+0
                          ; 2nd time through loop it will load palette+1
                          ; 3rd time through loop it will load palette+2
                          ; etc
  STA $2007             ; write to PPU
  INX                   ; X = X + 1
  CPX #$20              ; Compare X to hex $10, decimal 16 - copying 16 bytes = 4 sprites
  BNE LoadPalettesLoop  ; Branch to LoadPalettesLoop if compare was Not Equal to zero
                        ; if compare was equal to 32, keep going down

LoadBackground:
  LDA $2002             ; read PPU status to reset the high/low latch
  LDA #$20
  STA $2006             ; write the high byte of $2000 address
  LDA #$00
  STA $2006             ; write the low byte of $2000 address
  LDX #$00              ; start out at 0

  LDA #low(background)
  STA bckgrndtlmemloc
  LDA #high(background)
  LDY #$01
  STA bckgrndtlmemloc, Y
  LDX #$00 
LoadBackgroundLoopOuter:
  LDY #0
LoadBackgroundLoopInner: 
  ;check if we have reached the end of the loop determined by the value of the looop counter
  ; the loop counter is tored in x and y registers, y= low byte of loop counter, x= high byte of loop counter
  CPX #$03
  BNE Incrementer
  CPY #$C0
  BNE Incrementer
  JMP LoadAttribute
Incrementer:
  LDA [bckgrndtlmemloc], Y
  STA $2007
  INY ; the zero flag is set if the resukt is zero 
  BNE LoadBackgroundLoopInner ; we can use the conditional branch here as INY affects the zero flag
  INX
  LDY #$01
  LDA bckgrndtlmemloc, Y
  CLC
  ADC #$01
  STA bckgrndtlmemloc, Y
  BNE LoadBackgroundLoopOuter
             
LoadAttribute:
  LDA $2002             ; read PPU status to reset the high/low latch
  LDA #$23
  STA $2006             ; write the high byte of $23C0 address
  LDA #$C0
  STA $2006             ; write the low byte of $23C0 address
  LDX #$00              ; start out at 0
LoadAttributeLoop:
  LDA attribute, x      ; load data from address (attribute + the value in x)
  STA $2007             ; write to PPU
  INX                   ; X = X + 1
  CPX #$40              ; Compare X to hex $08, decimal 8 - copying 8 bytes
  BNE LoadAttributeLoop  ; Branch to LoadAttributeLoop if compare was Not Equal to zero
                        ; if compare was equal to 128, keep going down
  


;;;Set some initial ball stats
  LDA #$01
  STA balldown
  STA ballright
  LDA #$00
  STA ballup
  STA ballleft
  
  LDA #$50
  STA bally
  
  LDA #$80
  STA ballx
  
  LDA #$02
  STA ballspeedx
  STA ballspeedy
;set starting lives
  LDA #$08
  STA lives1
;set starting paddle position
  LDA #PADDLEYSTART
  STA paddle1ytop
  STA paddle2ytop
;;:Set starting game state
  LDA #STATETITLE
  STA gamestate

  LDX #$00                ; start out at 0
PositionSprite:
  LDA sprites, x 
  STA $0200, x
  INX
  CPX #$24
  BNE PositionSprite
              
  LDA #%10010000   ; enable NMI, sprites from Pattern Table 0, background from Pattern Table 1
  STA $2000

  LDA #%00011110   ; enable sprites, enable background, no clipping on left side
  STA $2001

Forever:
  JMP Forever     ;jump back to Forever, infinite loop, waiting for NMI
  
 

NMI:
  LDA #$00
  STA $2003       ; set the low byte (00) of the RAM address
  LDA #$02
  STA $4014       ; set the high byte (02) of the RAM address, start the transfer
  
  JSR DrawInfo 
  JSR DrawScore

DrawingDone:
  ;;This is the PPU clean up section, so rendering the next frame starts properly.
  LDA #%10010000   ; enable NMI, sprites from Pattern Table 0, background from Pattern Table 1
  STA $2000
  LDA #%00011110   ; enable sprites, enable background, no clipping on left side
  STA $2001
  LDA #$00        ;;tell the ppu there is no background scrolling
  STA $2005
  STA $2005
    
  ;;;all graphics updates done by here, run game engine


  JSR ReadController1  ;;get the current button data for player 1
  JSR ReadController2  ;;get the current button data for player 2
  
GameEngine:  
  LDA gamestate
  CMP #STATETITLE
  BEQ EngineTitle    ;;game is displaying title screen
    
  LDA gamestate
  CMP #STATEGAMEOVER
  BEQ EngineGameOver  ;;game is displaying ending screen
  
  LDA gamestate
  CMP #STATEPLAYING
  BEQ EnginePlaying   ;;game is playing
GameEngineDone:  
  
  JSR UpdateSprites  ;;set ball/paddle sprites from positions

  RTI             ; return from interrupt
 
 
 
 
;;;;;;;;
 
EngineTitle:
  ;;if start button pressed
  ;;  turn screen off
  ;;  load game screen
  ;;  set starting paddle/ball position
  ;;  go to Playing State
  ;;  turn screen on

  LDA buttons1
  AND #%00010000; start button mask
  CMP #%00010000 ; if the start button is pressed
  BNE GameEngineDone 
  LDA buttonStateCache
  ORA #%11101111; start button mask
  CMP #%11101111 ; if the start button was pressed 
  BNE GameEngineDone
  LDA #STATEPLAYING
  STA gamestate
  JMP GameEngineDone

;;;;;;;;; 
 
EngineGameOver:
  ;;if start button pressed
  ;;  turn screen off
  ;;  load title screen
  ;;  go to Title State
  ;;  turn screen on 


  LDA buttons1
  AND #%00010000; start button mask
  CMP #%00010000 ; if the start button is pressed
  BNE GameEngineDone 
  LDA buttonStateCache
  ORA #%11101111; start button mask
  CMP #%11101111 ; if the start button was pressed 
  BNE GameEngineDone
  LDA #STATETITLE
  STA gamestate
  LDA #$08
  STA lives1
  LDA #PADDLEYSTART
  STA paddle1ytop
  STA paddle2ytop
  LDA #$00
  STA score1
  STA score2
  LDX #$03             ; start out at 0
ZeroDecimalScoreLoop:
  DEX
  STA decimalScore1,X 
  STA decimalScore2,X
  CPX #$00
  BNE ZeroDecimalScoreLoop

  JSR DrawScore 

  JMP GameEngineDone
 
;;;;;;;;;;;
 
EnginePlaying:
  LDA score1
  CMP #WINSCORE
  BEQ PlayerWon
  LDA score2
  CMP #WINSCORE
  BNE ScoreCheckDone
PlayerWon:
  LDA #STATEGAMEOVER
  STA gamestate
  JMP GameEngineDone

ScoreCheckDone:

  LDA buttons1
  AND #%00010000; start button mask
  CMP #%00010000 ; if the start button is pressed 
  BNE ButtonCheckDone
  LDA buttonStateCache
  ORA #%11101111; start button mask
  CMP #%11101111 ; if the start button was pressed 
  BNE ButtonCheckDone
  LDA #STATETITLE 
  STA gamestate
  JMP GameEngineDone
ButtonCheckDone:

  LDA ballresetdelay
  AND #%10000000
  CMP #%10000000
  BNE PlayingDelayDone
  LDA ballresetdelay
  AND #%01111111
  CMP #$40
  BCC PlayingDelayIncrease
  LDA #$00
  STA ballresetdelay
  JMP PlayingDelayDone
PlayingDelayIncrease:
  LDA ballresetdelay
  ADC #$01
  STA ballresetdelay
  JMP MoveBallDownDone
PlayingDelayDone:

MoveBallRight:
  LDA ballright
  BEQ MoveBallRightDone   ;;if ballright=0, skip this section

  LDA ballx
  CLC
  ADC ballspeedx        ;;ballx position = ballx + ballspeedx
  STA ballx

  LDA ballx
  ADC #BALLWIDTH
  SBC #$01
  ADC ballspeedx
  CMP #RIGHTWALL
  BCC MoveBallRightDone      ;;if ball x < right wall, still on screen, skip next section
  ;here the first pixel to the right of the ball coincides with the leftmost pixel of the wall 
  LDA #$00
  STA ballright
  LDA #$01
  STA ballleft         ;;bounce, ball now moving left
  ;LDA #STATETITLE
  ;STA gamestate
  JMP MoveBallLeftDone  
  ;;in real game, give point to player 1, reset ball
MoveBallRightDone:


MoveBallLeft:
  LDA ballleft
  BEQ MoveBallLeftDone   ;;if ballleft=0, skip this section

  LDA ballx
  SEC
  SBC ballspeedx        ;;ballx position = ballx - ballspeedx
  STA ballx

  LDA ballx
  CMP #LEFTWALL
  BCS MoveBallLeftDone      ;;if ball x > left wall, still on screen, skip next section
  LDA #$01
  STA ballright
  LDA #$00
  STA ballleft         ;;bounce, ball now moving right
  ;;in real game, give point to player 2, reset ball
MoveBallLeftDone:


MoveBallUp:
  LDA ballup
  BEQ MoveBallUpDone   ;;if ballup=0, skip this section

  LDA bally
  SEC
  SBC ballspeedy        ;;bally position = bally - ballspeedy
  STA bally

  LDA #TOPWALL
  ADC ballspeedy
  SBC #$01
  CMP bally
  BCS BounceDown    ;; if bally -ballyspeed <= topwall then bounce ball
  JMP MoveBallDownDone
BounceDown:

  LDA #$01
  STA balldown
  LDA #$00
  STA ballup         ;;bounce, ball now moving down
  ;LDA #STATETITLE
  ;STA gamestate
  JMP MoveBallDownDone
MoveBallUpDone:


MoveBallDown:
  LDA balldown
  BEQ MoveBallDownDone   ;;if ballup=0, skip this section

  LDA bally
  CLC
  ADC ballspeedy        ;;bally position = bally + ballspeedy
  STA bally
  LDA bally
  ADC #BALLLENGTH
  ADC ballspeedy
  CMP #BOTTOMWALL
  BCC MoveBallDownDone      ;;if ball y < bottom wall, still on screen, skip next section
  LDA #$00
  STA balldown
  LDA #$01
  STA ballup         ;;bounce, ball now moving down
  ;LDA #STATETITLE
  ;STA gamestate
MoveBallDownDone:

MovePaddle:
  LDA buttons1
  ORA #%11110011; up or down button mask
  CMP #%11110011 ; if the up or down buttin is not pressed then skip 
  BEQ MovePaddleDone 
  ;we must have up or down pressed
  LDA buttons1
  AND #%00001000 ; up mask
  CMP #%00001000
  BNE MovePaddleDown
  LDA paddle1ytop
  SEC
  SBC #$03
  STA paddle1ytop
  JMP MovePaddleDone
MovePaddleDown:
  LDA paddle1ytop
  CLC
  ADC #03
  STA paddle1ytop
MovePaddleDone:

MovePaddle2:
  LDA buttons2
  ORA #%11110011; up or down button mask
  CMP #%11110011 ; if the up or down buttin is not pressed then skip 
  BEQ MovePaddle2Done 
  ;we must have up or down pressed
  LDA buttons2
  AND #%00001000 ; up mask
  CMP #%00001000
  BNE MovePaddle2Down
  LDA paddle2ytop
  SEC
  SBC #$03
  STA paddle2ytop
  JMP MovePaddle2Done
MovePaddle2Down:
  LDA paddle2ytop
  CLC
  ADC #03
  STA paddle2ytop
MovePaddle2Done:


  LDA paddle1ytop
  ADC #$20
  STA paddle1ybot

  LDA paddle2ytop
  ADC #$20
  STA paddle2ybot

;call check region collision for paddle1
  LDA #$00 
  STA rect1leftx
  LDA #PADDLE1X
  STA rect1rightx 
  LDA paddle1ytop
  STA rect1topy 
  LDA paddle1ybot
  STA rect1bottomy 
  LDA ballx
  STA rect2leftx 
  ADC #BALLWIDTH
  STA rect2rightx
  LDA bally
  STA rect2topy
  ADC #BALLWIDTH
  STA rect2bottomy 
  JSR CheckRegionCollision
;check region collision for paddle1
  LDA inrect
  AND #$01
  CMP #$00
  BEQ Paddle1CollisionCheckDone
  LDA inrect
  AND #%00001100
  CMP #%00000000
  BEQ Paddle1Miss
  LDA #$01
  STA ballright
  LDA #$00
  STA ballleft






  JMP Paddle1CollisionCheckDone
Paddle1Miss:
;increase score of other player
  LDA score2
  CLC
  ADC #$01
  STA score2
  LDA score2
  STA tempBinary
  JSR BinaryToDecimal

  LDX #$03             ; start out at 0
CopyScore2Loop:
  DEX
  LDA decimalResult,X
  STA decimalScore2,X
  CPX #$00
  BNE CopyScore2Loop


 ;reset ball position 
  LDA #$50
  STA bally 
  LDA #$80
  STA ballx 

  LDA #%10000000
  STA ballresetdelay


Paddle1CollisionCheckDone:


;call check region collision for paddle2
  LDA #PADDLE2X
  STA rect1leftx
  LDA #$FE
  STA rect1rightx 
  LDA paddle2ytop
  STA rect1topy 
  LDA paddle2ybot
  STA rect1bottomy 
  LDA ballx
  STA rect2leftx
  ADC #BALLWIDTH
  
  STA rect2rightx 
  LDA bally
  STA rect2topy
  ADC #BALLWIDTH
  STA rect2bottomy 
  JSR CheckRegionCollision
;check region collision for paddle2
  LDA inrect
  AND #%00000010
  CMP #%00000010
  BNE Paddle2CollisionCheckDone
  LDA inrect
  AND #%00001100
  CMP #%00000000
  BEQ Paddle2Miss
  LDA #$00
  STA ballright
  LDA #$01
  STA ballleft








  JMP Paddle2CollisionCheckDone

Paddle2Miss:
;increase other players score
  LDA score1
  CLC
  ADC #$01
  STA score1
  LDA score1
  STA tempBinary
  JSR BinaryToDecimal

  LDX #$03             ; start out at 0
CopyScore1Loop:
  DEX
  LDA decimalResult,X
  STA decimalScore1,X
  CPX #$00
  BNE CopyScore1Loop

  LDA #$50
  STA bally 
  LDA #$80
  STA ballx 

  LDA #%10000000
  STA ballresetdelay

Paddle2CollisionCheckDone: 

  JMP GameEngineDone

;checks that rect1 collides with rect2
; inrect returns param 01234567
; 7 rect1leftx <= rect2leftx <= rect1rightx
; 6 rect1leftx <= rect2righttx <= rect1rightx
; 5 rect1topy <= rect2topy <= rect1bottomy
; 4 rect1topy <= rect2bottomy <= rect1bottomy
CheckRegionCollision:
  LDA #$00
  STA inrect
Rect2LeftInRect1Check:
  LDA rect2leftx
  CMP rect1leftx
  BCC Rect2RightInRect1
  LDA rect1rightx
  CMP rect2leftx
  BCC Rect2RightInRect1
  LDA inrect
  ORA #%00000001
  STA inrect
Rect2RightInRect1:
  LDA rect2rightx
  CMP rect1leftx
  BCC Rect2TopInRect1
  LDA rect1rightx
  CMP rect2rightx
  BCC Rect2TopInRect1
  LDA inrect
  ORA #%00000010
  STA inrect
Rect2TopInRect1: 
  LDA rect2topy
  CMP rect1topy
  BCC Rect2BottomInRect1
  LDA rect1bottomy
  CMP rect2topy
  BCC Rect2BottomInRect1
  LDA inrect
  ORA #%00000100
  STA inrect
Rect2BottomInRect1:
  LDA rect2bottomy
  CMP rect1topy
  BCC CheckRegionCollisionDone
  LDA rect1bottomy
  CMP rect2bottomy
  BCC CheckRegionCollisionDone
  LDA inrect
  ORA #%00001000
  STA inrect
CheckRegionCollisionDone:
  RTS 
 
BinaryToDecimal:
  lda #$00
  sta decimalResult+0
  sta decimalResult+1
  sta decimalResult+2
  ldx #$08
BitLoop:
  asl tempBinary
  ldy decimalResult+0
  lda BinTable, y
  rol a
  sta decimalResult+0
  ldy decimalResult+1
  lda BinTable, y
  rol a
  sta decimalResult+1
  rol decimalResult+2
  dex
  bne BitLoop
  rts 
 
UpdateSprites:
  LDA bally  ;;update all ball sprite info
  STA $0200
  
  ;LDA #$
  ;STA $0201
  
  ;LDA #$01
  ;STA $0202
  
  LDA ballx
  STA $0203
 
  ;TODO fix this nasty bodge to get x position of paddle at 0 
  LDA #$00
  STA $0207
  STA $020B
  STA $020F
  STA $0213

;MovePaddleSpritesPaddle1: 
;set init Paddle y pos mem location to 0204
  LDA #$04
  STA mvpTileYMemLoc
  ;set high order byte 02 to mem location paddlesprites + 1  
  LDA #$02
  LDY #$01 ;set offset into y register
  STA mvpTileYMemLoc, Y
;set init y pos to paddleytop calcualted in game engine move
  LDA paddle1ytop
  STA mvpY
  JSR MovePaddleSprites

  LDX #$00
MovePaddleSpritesPaddle2: 
;set init Paddle y pos mem location to 02014
  LDA #$14
  STA mvpTileYMemLoc
  ;set high order byte 02 to mem location paddlesprites + 1  
  LDA #$02
  LDY #$01 ;set offset into y register
  STA mvpTileYMemLoc, Y
;set init y pos to paddleytop calcualted in game engine move
  LDA paddle2ytop
  STA mvpY
  JSR MovePaddleSprites

  ;;update paddle sprites
  RTS

;subroutine (kind of)
; moves a tile to a specified y position
;param1: mvpY - the y position to place the paddle tile
;param2 mvpTileYMemLoc - the mem loc of y position of tile to move.
MovePaddleSprites:
  ;set new tile y position
  LDY #$00; this is set to 0 to allow indirect addressing
  LDA mvpY 
  STA [mvpTileYMemLoc], Y

  ;move to next mem loc of y position of tile
  LDA mvpTileYMemLoc
  CLC
  ADC #$04; paddle sprites are stroed consecutively, 4 bytes per sprite, so we add 4 to get the y pos of the next sprite
  STA mvpTileYMemLoc
  LDY #$01
  LDA mvpTileYMemLoc, Y
  ADC #$00
  STA mvpTileYMemLoc, Y

  ;increase y position
  LDA mvpY
  ADC #$08; we want the sprites to be separated by a vertical distance of 8px
  STA mvpY

  ; finish the paddle sprite update when we have updated all 4 sprites comprising the paddle
  INX
  CPX #$04 
  BNE MovePaddleSprites

  RTS

 
DrawInfo:
  ;;draw score on screen using background tiles
  ;;or using many sprites
  LDA $2002             ; read PPU status to reset the high/low latch
  LDA #$22
  STA $2006             ; write the high byte of background score address
  LDA #$0B
  STA $2006             ; write the low byte of background score address
  LDX #$00              ; start out at 0
DrawTitleLoop:
  LDA gamestate
  CMP #STATEPLAYING
  BNE LoadInfoText
  LDA #$24
  JMP LoadTitleTileDone
LoadInfoText:
  CMP #STATEGAMEOVER
  BNE LoadTitleTile
  LDA gameover,X
  JMP LoadTitleTileDone
LoadTitleTile:
  LDA title,X
LoadTitleTileDone: 
  STA $2007
  INX
  CPX #$09
  BNE DrawTitleLoop   
  RTS

DrawScore:
  ;;draw score on screen using background tiles
  ;;or using many sprites
  LDA $2002             ; read PPU status to reset the high/low latch
  LDA #$20
  STA $2006             ; write the high byte of background score address
  LDA #$1D
  STA $2006             ; write the low byte of background score address
  
;  LDA score2 
;  STA $2007

  LDX #$03             ; start out at 0
DrawScore2Loop:
  DEX
  LDA decimalScore2,X
  STA $2007
  CPX #$00
  BNE DrawScore2Loop 
 
  ;;draw lives on screen using background tiles
  ;;or using many sprites
  LDA $2002             ; read PPU status to reset the high/low latch
  LDA #$20
  STA $2006             ; write the high byte of background score address
  LDA #$06
  STA $2006             ; write the low byte of background score address



  LDX #$03             ; start out at 0
DrawScore1Loop:
  DEX
  LDA decimalScore1,X
  STA $2007
  CPX #$00
  BNE DrawScore1Loop 

   
  RTS

 
ReadController1:
  LDA buttons1
  STA buttonStateCache
  LDA #$01
  STA $4016
  LDA #$00
  STA $4016
  LDX #$08
ReadController1Loop:
  LDA $4016
  LSR A            ; bit0 -> Carry
  ROL buttons1     ; bit0 <- Carry
  DEX
  BNE ReadController1Loop
  RTS
  
ReadController2:
  LDA #$01
  STA $4016
  LDA #$00
  STA $4016
  LDX #$08
ReadController2Loop:
  LDA $4017
  LSR A            ; bit0 -> Carry
  ROL buttons2     ; bit0 <- Carry
  DEX
  BNE ReadController2Loop
  RTS  
  
  
    
        
;;;;;;;;;;;;;;  
  
  
  
  .bank 1
  .org $E000
title:
  .db $19 ,$18 ,$17 ,$10 ,$12 ,$17 ,$0A ,$15 ,$0D
gameover:
  .db $10, $0A, $16, $0E, $24, $18, $1F, $0E, $1B 
palette:
  .db $22,$29,$1A,$0F,  $22,$36,$17,$0F,  $22,$30,$21,$0F,  $22,$27,$17,$0F   ;;background palette
  .db $22,$0F,$27,$17,  $22,$36,$17,$0F,  $22,$30,$21,$0F,  $22,$27,$17,$0F
sprites:
     ;vert tile attr horiz
  .db $80, $85, $00, $80   ;sprite 0 ball
  .db $80, $85, $00, $04   ;sprite 1 paddle top
  .db $88, $86, $00, $04   ;sprite 2 paddle middle
  .db $90, $86, $00, $04   ;sprite 3 paddle middle
  .db $98, $86, $00, $04   ;sprite 4 paddle bottom
  .db $80, $85, $00, $F8   ;sprite 1 paddle2 top
  .db $80, $86, $00, $F8   ;sprite 2 paddle2 top
  .db $80, $86, $00, $F8   ;sprite 3 paddle2 top
  .db $80, $86, $00, $F8   ;sprite 4 paddle2 top


background:
  .db $1C,$0C,$18,$1B,$0E,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 1
  .db $24,$24,$24,$24,$24,$24,$24,$1C,$0C,$18,$1B,$0E,$24,$24,$24,$24  ;;all sky

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 2
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;all sky

  .db $24,$24,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45  ;;row 3
  .db $45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$24,$24  ;;some brick tops

  .db $24,$24,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47  ;;row 4
  .db $47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$24,$24  ;;brick bottoms

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 5 
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 6
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 7 
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 8
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 9
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 10
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 11
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 12
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 13
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 14
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 15
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 16
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 17
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 18
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 19
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 20
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 21
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24 ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 22
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 23
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 24
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24 ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 25
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 26
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 27
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 28
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;end brick bottom

  .db $24,$24,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45  ;;row 29
  .db $45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$24,$24  ;;end brick bottom

  .db $24,$24,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47  ;;row 30
  .db $47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$24,$24  ;;end brick bottom



attribute:
  .db %01010000, %01010000, %01010000, %01010000, %01010000, %01010000, %01010000, %01010000
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100
  .db %00000100, %00000101, %00000101, %00000101, %00000101, %00000101, %00000101, %01010101

BinTable:
  .db $00, $01, $02, $03, $04, $80, $81, $82, $83, $84




  .org $FFFA     ;first of the three vectors starts here
  .dw NMI        ;when an NMI happens (once per frame if enabled) the 
                   ;processor will jump to the label NMI:
  .dw RESET      ;when the processor first turns on or is reset, it will jump
                   ;to the label RESET:
  .dw 0          ;external interrupt IRQ is not used in this tutorial
  
  
;;;;;;;;;;;;;;  
  
  
  .bank 2
  .org $0000
  .incbin "mario.chr"   ;includes 8KB graphics file from SMB1
