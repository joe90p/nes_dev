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
paddle2ybot   .rs 1  ; player 2 paddle bottom vertical position
buttons1   .rs 1  ; player 1 gamepad buttons, one bit per button
buttons2   .rs 1  ; player 2 gamepad buttons, one bit per button
score1     .rs 1  ; player 1 score, 0-15
score2     .rs 1  ; player 2 score, 0-15
mvpTileYMemLoc .rs 2 ; MovePaddleSprites param - mem location of y position of tile
mvpY .rs 1; MovePaddleSprites param - y position
paddle1ybot .rs 1
bckgrndtlmemloc .rs 2
;; DECLARE SOME CONSTANTS HERE
STATETITLE     = $00  ; displaying title screen
STATEPLAYING   = $01  ; move paddles/ball, check for collisions
STATEGAMEOVER  = $02  ; displaying game over screen
  
RIGHTWALL      = $F4  ; when ball reaches one of these, do something
TOPWALL        = $20
BOTTOMWALL     = $E0
LEFTWALL       = $04
  
PADDLE1X       = $08  ; horizontal position for paddles, doesnt move. TODO using this as paddle1 surface x pos. Change so this is not a constant.
PADDLE2X       = $F0
PADDLELENGTH   = $04
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
;LoadBackgroundLoop:
;  LDA background, x     ; load data from address (background + the value in x)
;  STA $2007             ; write to PPU
;  INX                   ; X = X + 1
;  CPX #$FF              ; Compare X to hex $80, decimal 128 - copying 128 bytes
;  BNE LoadBackgroundLoop  ; Branch to LoadBackgroundLoop if compare was Not Equal to zero
                        ; if compare was equal to 128, keep going down
 
  LDA #low(background)
  STA bckgrndtlmemloc
  LDA #high(background)
  LDY #$01
  STA bckgrndtlmemloc, Y
  LDX #$00 
LoadBackgroundLoopOuter:
  LDY #0
LoadBackgroundLoopInner:
  LDA [bckgrndtlmemloc], Y
  STA $2007
  INY ; the zero flag is set if the resukt is zero
  BNE LoadBackgroundLoopInner ; we can use the conditional branch here as INY affects the zero flag
  INX
  CPX #$02
  BEQ LoadBackgroundLoopEnd
  LDY #$01
  LDA bckgrndtlmemloc, Y
  CLC
  ADC #$01
  STA bckgrndtlmemloc, Y
  BNE LoadBackgroundLoopOuter
LoadBackgroundLoopEnd:
             
              
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
  CPX #$10              ; Compare X to hex $08, decimal 8 - copying 8 bytes
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


;;:Set starting game state
  LDA #STATEPLAYING
  STA gamestate

  LDX #$00                ; start out at 0
PositionSprite:
  LDA sprites, x 
  STA $0200, x
  INX
  CPX #$14
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

  JSR DrawScore

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
  JMP GameEngineDone

;;;;;;;;; 
 
EngineGameOver:
  ;;if start button pressed
  ;;  turn screen off
  ;;  load title screen
  ;;  go to Title State
  ;;  turn screen on 
  JMP GameEngineDone
 
;;;;;;;;;;;
 
EnginePlaying:

MoveBallRight:
  LDA ballright
  BEQ MoveBallRightDone   ;;if ballright=0, skip this section

  LDA ballx
  CLC
  ADC ballspeedx        ;;ballx position = ballx + ballspeedx
  STA ballx

  LDA ballx
  CMP #RIGHTWALL
  BCC MoveBallRightDone      ;;if ball x < right wall, still on screen, skip next section
  LDA #$00
  STA ballright
  LDA #$01
  STA ballleft         ;;bounce, ball now moving left
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

  LDA bally
  CMP #TOPWALL
  BCS MoveBallUpDone      ;;if ball y > top wall, still on screen, skip next section
  LDA #$01
  STA balldown
  LDA #$00
  STA ballup         ;;bounce, ball now moving down
MoveBallUpDone:


MoveBallDown:
  LDA balldown
  BEQ MoveBallDownDone   ;;if ballup=0, skip this section

  LDA bally
  CLC
  ADC ballspeedy        ;;bally position = bally + ballspeedy
  STA bally

  LDA bally
  CMP #BOTTOMWALL
  BCC MoveBallDownDone      ;;if ball y < bottom wall, still on screen, skip next section
  LDA #$00
  STA balldown
  LDA #$01
  STA ballup         ;;bounce, ball now moving down
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

  LDA paddle1ytop
  ADC #$20
  STA paddle1ybot
 
CheckPaddleCollision:
  LDA ballx
  CMP #PADDLE1X
  BCS CheckPaddleCollisionDone
  LDA bally
  CMP paddle1ytop
  BCC CheckPaddleCollisionDone 
  LDA bally
  CMP paddle1ybot
  BCS CheckPaddleCollisionDone  
;;if ball x < paddle1x
  ;;  if ball y > paddle y top
  ;;    if ball y < paddle y bottom
  ;;      bounce, ball now moving left
  LDA #$01
  STA ballright
  LDA #$00
  STA ballleft
CheckPaddleCollisionDone:

  JMP GameEngineDone
 
 
 
 
UpdateSprites:
  LDA bally  ;;update all ball sprite info
  STA $0200
  
  LDA #$45
  STA $0201
  
  LDA #$01
  STA $0202
  
  LDA ballx
  STA $0203
 
  ;TODO fix this nasty bodge to get x position of paddle at 0 
  LDA #$00
  STA $0207
  STA $020B
  STA $020F
  STA $0213

 
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
MovePaddleSpritesDone:
  ;;update paddle sprites
  RTS
 
 
DrawScore:
  ;;draw score on screen using background tiles
  ;;or using many sprites
  RTS
 
 
 
ReadController1:
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
palette:
  .db $22,$29,$1A,$0F,  $22,$36,$17,$0F,  $22,$30,$21,$0F,  $22,$27,$17,$0F   ;;background palette
  .db $22,$0F,$27,$17,  $22,$36,$17,$0F,  $22,$30,$21,$0F,  $22,$27,$17,$0F
sprites:
     ;vert tile attr horiz
  .db $80, $32, $00, $80   ;sprite 0
  .db $80, $85, $00, $04   ;sprite 1
  .db $88, $86, $00, $04   ;sprite 2
  .db $90, $86, $00, $04   ;sprite 3
  .db $98, $86, $00, $04   ;sprite 4

background:
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 1
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;all sky

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 2
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;all sky

  .db $24,$24,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45  ;;row 3
  .db $45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45,$45  ;;some brick tops

  .db $24,$24,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47  ;;row 4
  .db $47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47,$47  ;;brick bottoms

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 5 
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 6
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 7 
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 8
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 9
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 10
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 11
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 12
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 13
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 14
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 15
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24  ;;row 16
  .db $24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$24,$47  ;;end brick bottom

attribute:
  .db %01010000, %01010000, %01010000, %01010000, %01010000, %01010000, %01010000, %01010000
  .db %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %00000000, %01010100



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
