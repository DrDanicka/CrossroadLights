.equ UCSR0A, 0xC0
.equ UCSR0B, 0xC1
.equ UCSR0C, 0xC2
.equ UBRR0L, 0xC4
.equ UBRR0H, 0xC5
.equ TXEN0, 3
.equ RXEN0, 4
.equ RXC0, 7
.equ UDR0, 0xC6
.equ USBS0, 3
.equ EECR,  0x3F
.equ EEARL, 0x41
.equ EEARH, 0x42
.equ EEDR,  0x40
.equ EEPE,  1
.equ EERE,  0
.equ EEMPE, 2
.equ SPMEN, 0
.equ SPMCSR, 0x57



.global USART_init
.global USART_ReadConfig
.global EEPROM_ReadByte
.global USART_Transmit


.equ CONFIG_START, 0x0100  ; Place buffer at RAM address 0x0100


USART_Receive:
    lds r17, UCSR0A
    sbrs r17, RXC0
    rjmp USART_Receive

    lds r16, UDR0
    ret

USART_Transmit:
    lds r17, UCSR0A       ; Read UCSR0A to r17
    sbrs r17, 5       ; Skip if UDRE is set (buffer ready)
    rjmp USART_Transmit

    ; Send data in r16
    sts UDR0, r16
    ret

USART_init:
    push r16

    mov r16, r22
    subi r16, 5
    rol r16

    sts UBRR0L, r24
    sts UBRR0H, r25

    cpi r23, 2
    breq set_2_stop_bits
    cpi r23, 1
    breq set_1_stop_bit

    sts UCSR0C, r16

    ldi r16, (1 << RXEN0) | (1 << TXEN0)
    sts UCSR0B, r16

    pop r16
    ret

set_2_stop_bits:
    ori r16, (1 << USBS0)
    ret

set_1_stop_bit:
    andi r16, ~(1 << USBS0)
    ret

USART_ReadConfig:
    ldi r18, 0           ; Counter (0 to 9)
    ldi r30, lo8(CONFIG_START)  ; Load buffer base address into Z (low byte)
    ldi r31, hi8(CONFIG_START)  ; Load buffer base address into Z (high byte)

Read_Loop:
    call USART_Receive   ; Get byte in r16
    st X+, r16           ; Store in RAM buffer (post-increment X)
    
    inc r18              ; Increment counter
    cpi r18, 10          ; Stop after 10 bytes
    brlo Read_Loop       ; Continue loop if less than 10 bytes

    ; Now we have 10 bytes in RAM buffer ? Write directly to EEPROM

    ldi r18, 0           ; Reset counter for EEPROM write
    ldi r30, lo8(CONFIG_START)  ; Load buffer base address into Z (low byte)
    ldi r31, hi8(CONFIG_START)  ; Load buffer base address into Z (high byte)

Write_Loop:
    ld r16, Z+           ; Load byte from RAM buffer
    mov r24, r18         ; Set EEPROM address
    call EEPROM_WriteByte ; Write byte to EEPROM

    inc r18              ; Increment counter
    cpi r18, 10          ; Stop after writing 10 bytes
    brlo Write_Loop      ; Continue if more bytes left

    ldi r18, 0           ; Reset counter for next USART read
    ret


EEPROM_WriteByte:
    cli                  ; Disable interrupts (important!)

    ; 1?? Wait for completion of previous EEPROM write
EEPROM_Wait:
    lds r17, EECR
    sbrc r17, EEPE       ; If EEPE is still set, wait
    rjmp EEPROM_Wait

    ; 2?? Ensure no Flash programming is in progress
SPM_Wait:
    lds r17, SPMCSR      ; Read SPMCSR (Self-programming Control Register)
    sbrc r17, SPMEN      ; If SPMEN is set, wait
    rjmp SPM_Wait

    ; 3?? Set EEPROM address (r24 = EEARL, r25 = EEARH)
    sts EEARL, r24
    sts EEARH, r25

    ; 4?? Load data into EEPROM Data Register
    sts EEDR, r16

    ; 5?? Enable EEPROM Master Write (EEMPE)
    lds r17, EECR
    ori r17, (1 << EEMPE)  ; Set EEMPE bit

	mov r19, r17
	ori r19, (1 << EEPE)

    sts EECR, r17
	sts EECR, r19

    sei                  ; Re-enable interrupts
    ret


EEPROM_ReadByte:
    ; Wait for EEPROM ready
    ; sbic EECR, EEPE -> this was in documentation, but invalid becasue operand is out of range
	lds r17, EECR
	sbrc r17, EEPE
    rjmp EEPROM_ReadByte

    ; Set address
    sts EEARL, r24
    sts EEARH, r25

    ; Start read
    ; sbi EECR, EERE -> this was in documentation, but invalid becasue operand is out of range
	lds r17, EECR
	ori r17, (1 << EERE)
	sts EERE, r17
    lds r16, EEDR			; Read data register to r16
    ret
